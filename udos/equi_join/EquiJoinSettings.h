/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2016 SciDB, Inc.
* All Rights Reserved.
*
* equi_join is a plugin for SciDB, an Open Source Array DBMS maintained
* by Paradigm4. See http://www.paradigm4.com/
*
* equi_join is free software: you can redistribute it and/or modify
* it under the terms of the AFFERO GNU General Public License as published by
* the Free Software Foundation.
*
* equi_join is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
* INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
* the AFFERO GNU General Public License for the complete license terms.
*
* You should have received a copy of the AFFERO GNU General Public License
* along with equi_join.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
*
* END_COPYRIGHT
*/

#ifndef EQUI_JOIN_SETTINGS
#define EQUI_JOIN_SETTINGS

#include <query/Operator.h>
#include <query/Expression.h>
#include <query/AttributeComparator.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace scidb
{

std::shared_ptr<LogicalExpression>    parseExpression(const std::string&);

namespace equi_join
{

using std::string;
using std::vector;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::ostringstream;
using std::stringstream;
using boost::algorithm::trim;
using boost::starts_with;
using boost::lexical_cast; // 정수나 실수 자료형을 string 자료형 또는 char array로 바꿀 때 쉽고 간편하게 사용할 수 있다.
using boost::bad_lexical_cast;

// Logger for operator. static to prevent visibility of variable outside of file
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.operators.equi_join"));

/**
 * Table sizing considerations:
 *
 * We'd like to see a load factor of 4 or less. A group occupies at least 32 bytes in the structure,
 * usually more - depending on how many values and states there are and also whether they are variable sized.
 * An empty bucket is an 8-byte pointer. So the ratio of group data / bucket overhead is at least 16.
 * With that in mind we just pick a few primes for the most commonly used memory limits.
 * We start with that many buckets and, at the moment, we don't bother rehashing:
 *
 * memory_limit_MB     max_groups    desired_buckets   nearest_prime   buckets_overhead_MB
 *             128        4194304            1048576         1048573                     8
 *             256        8388608            2097152         2097143                    16
 *             512       16777216            4194304         4194301                    32
 *           1,024       33554432            8388608         8388617                    64
 *           2,048       67108864           16777216        16777213                   128
 *           4,096      134217728           33554432        33554467                   256
 *           8,192      268435456           67108864        67108859                   512
 *          16,384      536870912          134217728       134217757                 1,024
 *          32,768     1073741824          268435456       268435459                 2,048
 *          65,536     2147483648          536870912       536870909                 4,096
 *         131,072     4294967296         1073741824      1073741827                 8,192
 *            more                                        2147483647                16,384
 */

static const size_t NUM_SIZES = 12;
static const size_t memLimits[NUM_SIZES]  = {    128,     256,     512,    1024,     2048,     4096,     8192,     16384,     32768,     65536,     131072,  ((size_t)-1) };
static const size_t tableSizes[NUM_SIZES] = {1048573, 2097143, 4194301, 8388617, 16777213, 33554467, 67108859, 134217757, 268435459, 536870909, 1073741827,    2147483647 };

static size_t chooseNumBuckets(size_t maxTableSize)
{
   for(size_t i =0; i<NUM_SIZES; ++i)
   {
       if(maxTableSize <= memLimits[i])
       {
           return tableSizes[i];
       }
   }
   return tableSizes[NUM_SIZES-1];
}

//For hash join purposes, the handedness refers to which array is copied into a hash table and redistributed
enum Handedness
{
    LEFT,
    RIGHT
};

class Settings
{
public:
    enum algorithm
    {
        HASH_REPLICATE_LEFT,
        HASH_REPLICATE_RIGHT,
        MERGE_LEFT_FIRST,
        MERGE_RIGHT_FIRST
    };

private:
    ArrayDesc                     _leftSchema;
    ArrayDesc                     _rightSchema;
    size_t                        _numLeftAttrs;
    size_t                        _numLeftDims;
    size_t                        _numRightAttrs;
    size_t                        _numRightDims;
    vector<ssize_t>               _leftMapToTuple;   //maps all attributes and dimensions from left to tuple, -1 if not used
    vector<ssize_t>               _rightMapToTuple;
    size_t                        _leftTupleSize;
    size_t                        _rightTupleSize;
    size_t                        _numKeys;
    vector<AttributeComparator>   _keyComparators;   //one per key
    vector<size_t>                _leftIds;          //key indeces in the left array:  attributes start at 0, dimensions start at numAttrs
    vector<size_t>                _rightIds;        //key indeces in the right array: attributes start at 0, dimensions start at numAttrs
    vector<bool>                  _keyNullable;      //one per key, in the output
    size_t                        _hashJoinThreshold;
    size_t                        _numHashBuckets;
    size_t                        _chunkSize;
    size_t                        _numInstances;
    algorithm                     _algorithm;
    bool                          _algorithmSet;
    bool                          _keepDimensions;
    size_t                        _bloomFilterSize;
    size_t                        _readAheadLimit;
    size_t                        _varSize;
    string                        _filterExpressionString;
    shared_ptr<Expression>        _filterExpression;
    vector<string>                _leftNames;
    vector<string>                _rightNames;
    bool                          _leftOuter;
    bool                          _rightOuter;
    vector<string>                _outNames;

    static string paramToString(shared_ptr <OperatorParam> const& parameter, shared_ptr<Query>& query, bool logical)
    {
        if(logical)
        {
            return evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&)parameter)->getExpression(), TID_STRING).getString();
        }
        return ((shared_ptr<OperatorParamPhysicalExpression>&) parameter)->getExpression()->evaluate().getString();
    }

    void setParamIds(string trimmedContent, vector<size_t> &keys, size_t shift)
    {
        stringstream ss(trimmedContent);
        string tok;
        while(getline(ss, tok, ','))
        {
            try
            {
                uint64_t id;
                if(tok[0] == '~')
                {
                    id = lexical_cast<uint64_t>(tok.substr(1)) + shift;
                }
                else
                {
                    id = lexical_cast<uint64_t>(tok);
                }
                keys.push_back(id);
            }
            catch (bad_lexical_cast const& exn)
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse keys";
            }
        }
    }

    void setParamLeftIds(string trimmedContent)
    {
        setParamIds(trimmedContent, _leftIds, _numLeftAttrs);
    }

    void setParamRightIds(string trimmedContent)
    {
        setParamIds(trimmedContent, _rightIds, _numRightAttrs);
    }

    void setParamNames(string trimmedContent, vector<string> &names)
    {
        stringstream ss(trimmedContent);
        string tok;
        while(getline(ss, tok, ','))
        {
            try
            {
                trim(tok);
                names.push_back(tok);
            }
            catch (bad_lexical_cast const& exn)
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse keys";
            }
        }
    }

    void setParamLeftNames(string trimmedContent)
    {
        setParamNames(trimmedContent, _leftNames);
    }

    void setParamRightNames(string trimmedContent)
    {
        setParamNames(trimmedContent, _rightNames);
    }

    void setParamOutNames(string trimmedContent)
    {
        setParamNames(trimmedContent, _outNames);
    }

    void setParamHashJoinThreshold(string trimmedContent)
    {
        try
        {
            int64_t res = lexical_cast<int64_t>(trimmedContent);
            if(res < 0)
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "hash join threshold must be non negative";
            }
            _hashJoinThreshold = res * 1024 * 1204;
            _numHashBuckets = chooseNumBuckets(_hashJoinThreshold / (1024*1024));
        }
        catch (bad_lexical_cast const& exn)
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse hash join threshold";
        }
    }

    void setParamChunkSize(string trimmedContent)
    {
        try
        {
            int64_t res = lexical_cast<int64_t>(trimmedContent);
            if(res <= 0)
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "chunk size must be positive";
            }
            _chunkSize = res;
        }
        catch (bad_lexical_cast const& exn)
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse chunk size";
        }
    }

    void setParamAlgorithm(string trimmedContent)
    {
        if(trimmedContent == "hash_replicate_left")
        {
            _algorithm = HASH_REPLICATE_LEFT;
        }
        else if (trimmedContent == "hash_replicate_right")
        {
            _algorithm = HASH_REPLICATE_RIGHT;
        }
        else if (trimmedContent == "merge_left_first")
        {
            _algorithm = MERGE_LEFT_FIRST;
        }
        else if (trimmedContent == "merge_right_first")
        {
            _algorithm = MERGE_RIGHT_FIRST;
        }
        else
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse algorithm";
        }
    }

    bool setParamBool(string trimmedContent, bool& value)
    {
        if(trimmedContent == "1" || trimmedContent == "t" || trimmedContent == "T" || trimmedContent == "true" || trimmedContent == "TRUE")
        {
            value = true;
            return true;
        }
        else if (trimmedContent == "0" || trimmedContent == "f" || trimmedContent == "F" || trimmedContent == "false" || trimmedContent == "FALSE")
        {
            value = false;
            return true;
        }
        return false;
    }

    void setParamKeepDimensions(string trimmedContent)
    {
        if(!setParamBool(trimmedContent, _keepDimensions))
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse keep_dimensions";
        }
    }

    void setParamBloomFilterSize(string trimmedContent)
    {
        try
        {
            int64_t res = lexical_cast<int64_t>(trimmedContent);
            if(res <= 0)
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "bloom filter size size must be positive";
            }
            _bloomFilterSize = res;
        }
        catch (bad_lexical_cast const& exn)
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse bloom filter size";
        }
    }

    void setParamFilterExpression(string trimmedContent)
    {
        _filterExpressionString = trimmedContent;
    }

    void setParamLeftOuter(string trimmedContent)
    {
        if(!setParamBool(trimmedContent, _leftOuter))
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse left_outer";
        }
    }

    void setParamRightOuter(string trimmedContent)
    {
        if(!setParamBool(trimmedContent, _rightOuter))
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "could not parse right_outer";
        }
    }

    void setParam (string const& parameterString, bool& alreadySet, string const& header, void (Settings::* innersetter)(string) )
    {
        string paramContent = parameterString.substr(header.size());
        if (alreadySet)
        {
            string h = parameterString.substr(0, header.size()-1);
            ostringstream error;
            error<<"illegal attempt to set "<<h<<" multiple times";
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << error.str().c_str();
        }
        trim(paramContent);
        (this->*innersetter)(paramContent); //TODO:.. tried for an hour with a template first. #thisIsWhyWeCantHaveNiceThings
        alreadySet = true;
    }

public:
    static size_t const MAX_PARAMETERS = 11;

    Settings(vector<ArrayDesc const*> inputSchemas,
             vector< shared_ptr<OperatorParam> > const& operatorParameters,
             bool logical,
             shared_ptr<Query>& query):
        _leftSchema(*(inputSchemas[0])),
        _rightSchema(*(inputSchemas[1])),
        _numLeftAttrs(_leftSchema.getAttributes(true).size()),
        _numLeftDims(_leftSchema.getDimensions().size()),
        _numRightAttrs(_rightSchema.getAttributes(true).size()),
        _numRightDims(_rightSchema.getDimensions().size()),
        _hashJoinThreshold(Config::getInstance()->getOption<int>(CONFIG_MERGE_SORT_BUFFER) * 1024 * 1024 ),
        _numHashBuckets(chooseNumBuckets(_hashJoinThreshold / (1024*1024))),
        _chunkSize(1000000),
        _numInstances(query->getInstancesCount()),
        _algorithm(HASH_REPLICATE_RIGHT),
        _algorithmSet(false),
        _keepDimensions(false),
        _bloomFilterSize(33554467), //about 4MB, why not?
        _filterExpressionString(""),
        _filterExpression(NULL),
        _leftOuter(false),
        _rightOuter(false),
        _outNames(0)
    {
        string const leftIdsHeader                 = "left_ids=";
        string const rightIdsHeader                = "right_ids=";
        string const leftNamesHeader               = "left_names=";
        string const rightNamesHeader              = "right_names=";
        string const hashJoinThresholdHeader       = "hash_join_threshold=";
        string const chunkSizeHeader               = "chunk_size=";
        string const algorithmHeader               = "algorithm=";
        string const keepDimensionsHeader          = "keep_dimensions=";
        string const bloomFilterSizeHeader         = "bloom_filter_size=";
        string const filterExpressionHeader        = "filter:";
        string const leftOuterHeader               = "left_outer=";
        string const rightOuterHeader              = "right_outer=";
        string const outNamesHeader                = "out_names=";
        bool leftIdsSet            = false;
        bool rightIdsSet           = false;
        bool leftNamesSet          = false;
        bool rightNamesSet         = false;
        bool hashJoinThresholdSet  = false;
        bool chunkSizeSet          = false;
        bool keepDimensionsSet     = false;
        bool bloomFilterSizeSet    = false;
        bool filterExpressionSet   = false;
        bool leftOuterSet          = false;
        bool rightOuterSet         = false;
        bool outNamesSet           = false;
        size_t const nParams = operatorParameters.size();
        if (nParams > MAX_PARAMETERS)
        {   //assert-like exception. Caller should have taken care of this!
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "illegal number of parameters passed to equi_join";
        }
        for (size_t i= 0; i<nParams; ++i)
        {
            string parameterString = paramToString(operatorParameters[i], query, logical);
            if (starts_with(parameterString, leftIdsHeader))
            {
                setParam(parameterString, leftIdsSet, leftIdsHeader, &Settings::setParamLeftIds);
            }
            else if (starts_with(parameterString, rightIdsHeader ))
            {
                setParam(parameterString, rightIdsSet, rightIdsHeader, &Settings::setParamRightIds);
            }
            else if (starts_with(parameterString, leftNamesHeader))
            {
                setParam(parameterString, leftNamesSet, leftNamesHeader, &Settings::setParamLeftNames);
            }
            else if (starts_with(parameterString, rightNamesHeader))
            {
                setParam(parameterString, rightNamesSet, rightNamesHeader, &Settings::setParamRightNames);
            }
            else if (starts_with(parameterString, hashJoinThresholdHeader))
            {
                setParam(parameterString, hashJoinThresholdSet, hashJoinThresholdHeader, &Settings::setParamHashJoinThreshold);
            }
            else if (starts_with(parameterString, chunkSizeHeader))
            {
                setParam(parameterString, chunkSizeSet, chunkSizeHeader, &Settings::setParamChunkSize);
            }
            else if (starts_with(parameterString, algorithmHeader))
            {
                setParam(parameterString, _algorithmSet, algorithmHeader, &Settings::setParamAlgorithm);
            }
            else if (starts_with(parameterString, keepDimensionsHeader))
            {
                setParam(parameterString, keepDimensionsSet, keepDimensionsHeader, &Settings::setParamKeepDimensions);
            }
            else if (starts_with(parameterString, bloomFilterSizeHeader))
            {
                setParam(parameterString, bloomFilterSizeSet, bloomFilterSizeHeader, &Settings::setParamBloomFilterSize);
            }
            else if (starts_with(parameterString, filterExpressionHeader))
            {
                setParam(parameterString, filterExpressionSet, filterExpressionHeader, &Settings::setParamFilterExpression);
            }
            else if (starts_with(parameterString, leftOuterHeader))
            {
                setParam(parameterString, leftOuterSet, leftOuterHeader, &Settings::setParamLeftOuter);
            }
            else if (starts_with(parameterString, rightOuterHeader))
            {
                setParam(parameterString, rightOuterSet, rightOuterHeader, &Settings::setParamRightOuter);
            }
            else if (starts_with(parameterString, outNamesHeader))
            {
                setParam(parameterString, outNamesSet, outNamesHeader, &Settings::setParamOutNames);
            }
            else
            {
                ostringstream error;
                error << "Unrecognized token '"<<parameterString<<"'";
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << error.str().c_str();
            }
        }
        verifyInputs();
        mapAttributes();
        checkOutputNames();
        if(filterExpressionSet)
        {
            compileExpression(query);
        }
        logSettings();
    }

private:
    void throwIf(bool const cond, char const* errorText)
    {
        if(cond)
        {
            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << errorText;
        }
    }

    void verifyInputs()
    {
        throwIf(_leftIds.size() && _leftNames.size(),     "both left_ids and left_names are set; use one or the other");
        throwIf(_rightIds.size() && _rightNames.size(),   "both left_ids and left_names are set; use one or the other");
        throwIf(_leftIds.size() == 0 && _leftNames.size() == 0,   "no left join-on fields provided");
        throwIf(_rightIds.size() == 0 && _rightNames.size() == 0, "no right join-on fields provided");
        if(_leftNames.size())
        {
            for(size_t i=0; i<_leftNames.size(); ++i)
            {
                string const& name = _leftNames[i];
                bool found = false;
                for(AttributeID j = 0; j<_numLeftAttrs; ++j)
                {
                    AttributeDesc const& attr = _leftSchema.getAttributes()[j];
                    if(attr.getName() == name)
                    {
                        if(!found)
                        {
                            _leftIds.push_back(j);
                            found = true;
                        }
                        else
                        {
                            ostringstream err;
                            err<<"Left join field '"<<name<<"' is ambiguous; use ids or cast";
                            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                        }
                    }
                }
                for(size_t j = 0; j<_numLeftDims; ++j)
                {
                    DimensionDesc const& dim = _leftSchema.getDimensions()[j];
                    if(dim.getBaseName() == name)
                    {
                        if(!found)
                        {
                            _leftIds.push_back(j+_numLeftAttrs);
                            found = true;
                        }
                        else
                        {
                            ostringstream err;
                            err<<"Left join field '"<<name<<"' is ambiguous; use ids or cast";
                            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                        }
                    }
                }
                if(!found)
                {
                    ostringstream err;
                    err<<"Left join field '"<<name<<"' not found in the left array";
                    throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                }
            }
        }
        if(_rightNames.size())
        {
            for(size_t i=0; i<_rightNames.size(); ++i)
            {
                string const& name = _rightNames[i];
                bool found = false;
                for(AttributeID j = 0; j<_numRightAttrs; ++j)
                {
                    AttributeDesc const& attr = _rightSchema.getAttributes()[j];
                    if(attr.getName() == name)
                    {
                        if(!found)
                        {
                            _rightIds.push_back(j);
                            found = true;
                        }
                        else
                        {
                            ostringstream err;
                            err<<"Right join field '"<<name<<"' is ambiguous; use ids or cast";
                            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                        }
                    }
                }
                for(size_t j = 0; j<_numRightDims; ++j)
                {
                    DimensionDesc const& dim = _rightSchema.getDimensions()[j];
                    if(dim.getBaseName() == name)
                    {
                        if(!found)
                        {
                            _rightIds.push_back(j+_numRightAttrs);
                            found = true;
                        }
                        else
                        {
                            ostringstream err;
                            err<<"Right join field '"<<name<<"' is ambiguous; use ids or cast";
                            throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                        }
                    }
                }
                if(!found)
                {
                    ostringstream err;
                    err<<"Right join field '"<<name<<"' not found in the right array";
                    throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << err.str().c_str();
                }
            }
        }
        throwIf(_leftIds.size() != _rightIds.size(),    "mismatched numbers of join-on fields provided");
        for(size_t i =0; i<_leftIds.size(); ++i)
        {
            size_t leftKey  = _leftIds[i];
            size_t rightKey = _rightIds[i];
            throwIf(leftKey  >= _numLeftAttrs + _numLeftDims,  "left id out of bounds");
            throwIf(rightKey >= _numRightAttrs + _numRightDims, "right id out of bounds");
            TypeId leftType   = leftKey  < _numLeftAttrs  ? _leftSchema.getAttributes(true)[leftKey].getType()   : TID_INT64;
            TypeId rightType  = rightKey < _numRightAttrs ? _rightSchema.getAttributes(true)[rightKey].getType() : TID_INT64;
            throwIf(leftType != rightType, "key types do not match");
        }
        throwIf( _algorithmSet && _algorithm == HASH_REPLICATE_LEFT  && isLeftOuter(),  "left replicate algorithm cannot be used for left  outer join");
        throwIf( _algorithmSet && _algorithm == HASH_REPLICATE_RIGHT && isRightOuter(), "right replicate algorithm cannot be used for right outer join");
    }

    void mapAttributes()
    {
        _numKeys = _leftIds.size();
        _leftMapToTuple.resize(_numLeftAttrs + _numLeftDims, -1);
        _rightMapToTuple.resize(_numRightAttrs + _numRightDims, -1);
        for(size_t i =0; i<_numKeys; ++i)
        {
            size_t leftKey  = _leftIds[i];
            size_t rightKey = _rightIds[i];
            throwIf(_leftMapToTuple[leftKey] != -1, "left keys not unique");
            throwIf(_rightMapToTuple[rightKey] != -1, "right keys not unique");
            _leftMapToTuple[leftKey]   = i;
            _rightMapToTuple[rightKey] = i;
            TypeId leftType   = leftKey  < _numLeftAttrs  ? _leftSchema.getAttributes(true)[leftKey].getType()   : TID_INT64;
            bool leftNullable  = leftKey  < _numLeftAttrs  ?  _leftSchema.getAttributes(true)[leftKey].isNullable()   : false;
            bool rightNullable = rightKey < _numRightAttrs  ? _rightSchema.getAttributes(true)[rightKey].isNullable() : false;
            _keyComparators.push_back(AttributeComparator(leftType));
            _keyNullable.push_back( leftNullable || rightNullable );
        }
        size_t j=_numKeys;
        for(size_t i =0; i<_numLeftAttrs + _numLeftDims; ++i)
        {
            if(_leftMapToTuple[i] == -1 && (i<_numLeftAttrs || _keepDimensions))
            {
                _leftMapToTuple[i] = j++;
            }
        }
        _leftTupleSize = j;
        j = _numKeys;
        for(size_t i =0; i<_numRightAttrs + _numRightDims; ++i)
        {
            if(_rightMapToTuple[i] == -1 && (i<_numRightAttrs || _keepDimensions))
            {
                _rightMapToTuple[i] = j++;
            }
        }
        _rightTupleSize = j;
    }

    void checkOutputNames()
    {
        if(_outNames.size())
        {
            if(_outNames.size() != getNumOutputAttrs())
            {
                throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "Incorrect number of output names provided";
            }
            for(size_t i =0; i<_outNames.size(); ++i)
            {
                string const& t = _outNames[i];
                if(t.size()==0)
                {
                    throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << "Improper output names provided";
                }
                for(size_t j=0; j<t.size(); ++j)
                {
                    char ch = t[j];
                    if( !( (j == 0 && ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch == '_')) ||
                           (j > 0  && ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || (ch>='0' && ch <= '9') || ch == '_' ))))
                    {
                        ostringstream error;
                        error<<"invalid name '"<<t<<"'";
                        throw SYSTEM_EXCEPTION(SCIDB_SE_INTERNAL, SCIDB_LE_ILLEGAL_OPERATION) << error.str();;
                    }
                }
            }
        }
    }

    void compileExpression(shared_ptr<Query>& query)
    {
        shared_ptr<LogicalExpression> logicalExpr = parseExpression(_filterExpressionString);
        ArrayDesc inputDesc = getOutputSchema(query);
        vector<ArrayDesc> inputDescs;
        inputDescs.push_back(inputDesc);
        ArrayDesc outputDesc =inputDesc;
        _filterExpression.reset(new Expression());
        _filterExpression->compile(logicalExpr, false, TID_BOOL, inputDescs, outputDesc);
    }

    void logSettings()
    {
        ostringstream output;
        for(size_t i=0; i<_numKeys; ++i)
        {
            output<<_leftIds[i]<<"->"<<_rightIds[i]<<" ";
        }
        output<<"buckets "<< _numHashBuckets;
        output<<" chunk "<<_chunkSize;
        output<<" keep_dimensions "<<_keepDimensions;
        output<<" bloom filter size "<<_bloomFilterSize;
        output<<" expression "<<_filterExpressionString;
        output<<" left outer "<<_leftOuter;
        output<<" right outer "<<_rightOuter;
        LOG4CXX_DEBUG(logger, "EJ keys "<<output.str().c_str());
    }

public:
    size_t getNumKeys() const
    {
        return _numKeys;
    }

    size_t getNumLeftAttrs() const
    {
        return _numLeftAttrs;
    }

    size_t getNumLeftDims() const
    {
        return _numLeftDims;
    }

    size_t getNumRightAttrs() const
    {
        return _numRightAttrs;
    }

    size_t getNumRightDims() const
    {
        return _numRightDims;
    }

    size_t getNumOutputAttrs() const
    {
        return _leftTupleSize + _rightTupleSize - _numKeys;
    }

    size_t getLeftTupleSize() const
    {
        return _leftTupleSize;
    }

    size_t getRightTupleSize() const
    {
        return _rightTupleSize;
    }

    size_t getNumHashBuckets() const
    {
        return _numHashBuckets;
    }

    size_t getChunkSize() const
    {
        return _chunkSize;
    }

    bool isLeftKey(size_t const i) const
    {
        if(_leftMapToTuple[i] < 0)
        {
            return false;
        }
        return static_cast<size_t>(_leftMapToTuple[i]) < _numKeys;
    }

    bool isRightKey(size_t const i) const
    {
        if(_rightMapToTuple[i] < 0)
        {
            return false;
        }
        return static_cast<size_t>(_rightMapToTuple[i]) < _numKeys;
    }

    bool isKeyNullable(size_t const keyIdx) const
    {
        return _keyNullable[keyIdx];
    }

    ssize_t mapLeftToTuple(size_t const leftField) const
    {
        return _leftMapToTuple[leftField];
    }

    ssize_t mapRightToTuple(size_t const rightField) const
    {
        return _rightMapToTuple[rightField];
    }

    ssize_t mapLeftToOutput(size_t const leftField) const
    {
        return _leftMapToTuple[leftField];
    }

    ssize_t mapRightToOutput(size_t const rightField) const
    {
        if(_rightMapToTuple[rightField] == -1)
        {
            return -1;
        }
        return  isRightKey(rightField) ? _rightMapToTuple[rightField] : _rightMapToTuple[rightField] + _leftTupleSize - _numKeys;
    }

    bool keepDimensions() const
    {
        return _keepDimensions;
    }

    vector <AttributeComparator> const& getKeyComparators() const
    {
        return _keyComparators;
    }

    algorithm getAlgorithm() const
    {
        return _algorithm;
    }

    bool algorithmSet() const
    {
        return _algorithmSet;
    }

    size_t getHashJoinThreshold() const
    {
        return _hashJoinThreshold;
    }

    size_t getBloomFilterSize() const
    {
        return _bloomFilterSize;
    }

    shared_ptr<Expression> const& getFilterExpression() const
    {
        return _filterExpression;
    }

    bool isLeftOuter() const
    {
        return _leftOuter;
    }

    bool isRightOuter() const
    {
        return _rightOuter;
    }

    ArrayDesc const& getLeftSchema() const
    {
        return _leftSchema;
    }

    ArrayDesc const& getRightSchema() const
    {
        return _rightSchema;
    }

    ArrayDesc getOutputSchema(shared_ptr< Query> const& query) const
    {
        Attributes outputAttributes(getNumOutputAttrs());
        ArrayDesc const& leftSchema = getLeftSchema();
        size_t const numLeftAttrs = getNumLeftAttrs();
        size_t const numLeftDims  = getNumLeftDims();
        ArrayDesc const& rightSchema = getRightSchema();
        size_t const numRightAttrs = getNumRightAttrs();
        size_t const numRightDims  = getNumRightDims();
        for(AttributeID i =0; i<numLeftAttrs; ++i)
        {
            AttributeDesc const& input = leftSchema.getAttributes(true)[i];
            AttributeID destinationId = mapLeftToOutput(i);
            uint16_t flags = input.getFlags();
            if( isRightOuter() || (isLeftKey(i) && isKeyNullable(destinationId)))
            {
                flags |= AttributeDesc::IS_NULLABLE;
            }
            string const& name = _outNames.size() ? _outNames[destinationId] : input.getName();
            outputAttributes[destinationId] = AttributeDesc(destinationId, name, input.getType(), flags, CompressorType::NONE, input.getAliases());
        }
        for(size_t i =0; i<numLeftDims; ++i)
        {
            ssize_t destinationId = mapLeftToOutput(i + numLeftAttrs);
            if(destinationId < 0)
            {
                continue;
            }
            DimensionDesc const& inputDim = leftSchema.getDimensions()[i];
            uint16_t flags = 0;
            if( isRightOuter() || (isLeftKey(i + _numLeftAttrs) && isKeyNullable(destinationId))) //is it joined with a nullable attribute?
            {
                flags = AttributeDesc::IS_NULLABLE;
            }
            string const& name = _outNames.size() ? _outNames[destinationId] : inputDim.getBaseName();
            outputAttributes[destinationId] = AttributeDesc(destinationId, name, TID_INT64, flags, CompressorType::NONE);
        }
        for(AttributeID i =0; i<numRightAttrs; ++i)
        {
            if(isRightKey(i)) //already in the schema
            {
                continue;
            }
            AttributeDesc const& input = rightSchema.getAttributes(true)[i];
            AttributeID destinationId = mapRightToOutput(i);
            uint16_t flags = input.getFlags();
            if(isLeftOuter())
            {
                flags |= AttributeDesc::IS_NULLABLE;
            }
            string const& name = _outNames.size() ? _outNames[destinationId] : input.getName();
            outputAttributes[destinationId] = AttributeDesc(destinationId, name, input.getType(), flags, CompressorType::NONE, input.getAliases());
        }
        for(size_t i =0; i<numRightDims; ++i)
        {
            ssize_t destinationId = mapRightToOutput(i + _numRightAttrs);
            if(destinationId < 0 || isRightKey(i + _numRightAttrs))
            {
                continue;
            }
            DimensionDesc const& inputDim = rightSchema.getDimensions()[i];
            string const& name = _outNames.size() ? _outNames[destinationId] : inputDim.getBaseName();
            outputAttributes[destinationId] = AttributeDesc(destinationId, name, TID_INT64, isLeftOuter() ? AttributeDesc::IS_NULLABLE : 0, CompressorType::NONE);
        }
        outputAttributes = addEmptyTagAttribute(outputAttributes);
        Dimensions outputDimensions;
        outputDimensions.push_back(DimensionDesc("instance_id", 0, _numInstances-1,            1,          0));
        outputDimensions.push_back(DimensionDesc("value_no",    0, CoordinateBounds::getMax(), _chunkSize, 0));
        return ArrayDesc("equi_join", outputAttributes, outputDimensions, defaultPartitioning(), query->getDefaultArrayResidency());
    }
};

} } //namespaces

#endif //EQUI_JOIN_SETTINGS

