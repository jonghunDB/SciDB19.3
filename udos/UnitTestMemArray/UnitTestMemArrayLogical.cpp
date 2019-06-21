/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2019 SciDB, Inc.
* All Rights Reserved.
*
* SciDB is free software: you can redistribute it and/or modify
* it under the terms of the AFFERO GNU General Public License as published by
* the Free Software Foundation.
*
* SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
* INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
* NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
* the AFFERO GNU General Public License for the complete license terms.
*
* You should have received a copy of the AFFERO GNU General Public License
* along with SciDB.  If not, see <http://www.gnu.org/licenses/agpl-3.0.html>
*
* END_COPYRIGHT
*/

/*
 * @file UnitTestMemArrayLogical.cpp
 *
 * @brief The logical operator interface for testing deep-chunk merge.
 */

#include <query/Query.h>
#include <array/Array.h>
#include <query/LogicalOperator.h>
#include <query/Expression.h>

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.udos.UitTestMemArray.UnitTestMemArrayLogical.cpp"));

namespace scidb
{
    using namespace std;

/**
 * @brief The operator: test_memarray().
 *
 * @par Synopsis:
 *   test_memarray()
 *
 * @par Summary:
 *   This operator performs unit tests for memarray. It returns an empty string. Upon failures exceptions are thrown.
 *
 * @par Input:
 *   n/a
 *
 * @par Output array:
 *        <
 *   <br>   dummy_attribute: string
 *   <br> >
 *   <br> [
 *   <br>   dummy_dimension: start=end=chunk_interval=0.
 *   <br> ]
 *
 * @par Examples:
 *   n/a
 *
 * @par Errors:
 *   n/a
 *
 * @par Notes:
 *
 */
    class UnitTestMemArrayLogical: public LogicalOperator
    {
    public:
        UnitTestMemArrayLogical(const string& logicalName, const std::string& alias):
                LogicalOperator(logicalName, alias)
        {
            LOG4CXX_DEBUG(logger,"UnitTestMemArrayLogical::UnitTestMemArrayLogical called");

        }

        //OperatorParamPlaceholder PP
        //PlistRegex RE : in logical ops less cumbersome.


        // test_memarray( ow_coord1 [,low_coord2,...], high_coord1 [,high_coord2,...] , nDimension, chunkInterval, density, threshold{지금은 Iter인지 map인지 나타냄});
        static PlistSpec const* makePlistSpec()
        {
            static PlistSpec argSpec {
                    { "", // positionals
                            RE(RE::LIST, {
                                    //RE(PP(PLACEHOLDER_INPUT)),
                                    RE(RE::PLUS, {
                                        RE(PP(PLACEHOLDER_CONSTANT, TID_INT64)),
                                        RE(PP(PLACEHOLDER_CONSTANT, TID_INT64))

                                    }),
                                    RE(PP(PLACEHOLDER_CONSTANT)),
                                    RE(PP(PLACEHOLDER_CONSTANT)),
                                    RE(PP(PLACEHOLDER_CONSTANT)),
                                    RE(PP(PLACEHOLDER_CONSTANT))
                            })
                    }
            };
            return &argSpec;
        }


        ArrayDesc inferSchema(std::vector<ArrayDesc> schemas, std::shared_ptr< Query> query)
        {
            LOG4CXX_DEBUG(logger,"UnitTestMemArrayLogical::inferSchema called");


            if(_parameters.size() == 0 )
            {
                stringstream ss; ss << "zero  ";
                throw USER_EXCEPTION(SCIDB_SE_INFER_SCHEMA, SCIDB_LE_WRONG_OPERATOR_ARGUMENTS_COUNT3)
                        << "test_memarray" << ss.str();
            }

            for(Parameters::const_iterator it = _parameters.begin(); it != _parameters.end(); it++ )
            {
                assert(((std::shared_ptr<OperatorParam>&)*it)->getParamType() == PARAM_LOGICAL_EXPRESSION);
                assert(((std::shared_ptr<OperatorParamLogicalExpression>&)*it)->isConstant());
            }

            size_t nDims = (_parameters.size()-4) /2 ;

            Value const& ndimms = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[2*nDims])->getExpression(), TID_INT64);
            Value const& chunkInterval = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[2*nDims+1])->getExpression(), TID_INT64);
            Value const& density = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[2*nDims+2])->getExpression(), TID_INT64);
            Value const& threshold = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[2*nDims+3])->getExpression(), TID_INT64);


            if(nDims != ndimms.getUint64())
            {
                stringstream ss; ss << "dimension size is not correct  nDims : " << nDims ;
                throw USER_EXCEPTION(SCIDB_SE_INFER_SCHEMA, SCIDB_LE_WRONG_OPERATOR_ARGUMENTS_COUNT3)
                        << "test_memarray" << ss.str();
            }

            Coordinates startC(nDims);
            Coordinates endC(nDims);

            for(size_t i =0 ;i <nDims ;i++)
            {
                Value const& start = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[i])->getExpression(), TID_INT64);
                if(!start.isNull())
                {
                    startC[i] = start.getInt64();
                }
                Value const& end = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&) _parameters[i+ nDims])->getExpression(),TID_INT64);
                if(!end.isNull())
                {
                    endC[i] = end.getInt64();
                }

            }

            /* Make the output schema.*/
            Attributes attrs;
            attrs.push_back(AttributeDesc(
                    string("X"), TID_INT64, 0, CompressorType::NONE));

            Dimensions dims(nDims);
            for(size_t i =0 ; i < nDims ; i++)
            {
                std::string arrayName = "" + to_string(i);
                dims[i] = DimensionDesc( arrayName ,0,0,endC[i],endC[i],chunkInterval.getInt64(),0 );
            }



            return ArrayDesc("test_memarray", attrs, dims, createDistribution(getSynthesizedDistType()), query->getDefaultArrayResidency());

        }
        /* Make the output schema.*/
//        //여기서는 입력받은 schema를 그대로 사용하기 때문에 따로 변경할 필요가 없음. 하지만 일반적으로 udo를 만들때 schema를 설정해야 함
//        //schemas vector를 불러와서 하나의 array인지 확인.
//        assert(schemas.size() == 1);
//        //example) filter(array ,attribute1 < 100 ) 하나의 attribute이나 dimension을 parameter로 씀
//        assert(_parameters.size() == 1);
//        //
//        assert(_parameters[0]->getParamType() == PARAM_LOGICAL_EXPRESSION);
//
//        if(global)
//        {
//
//        }
//
//        return schemas[0].addEmptyTagAttribute();


    };

    REGISTER_LOGICAL_OPERATOR_FACTORY(UnitTestMemArrayLogical, "test_memarray");
}  // namespace scidb
