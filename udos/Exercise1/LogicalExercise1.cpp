/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2015 SciDB, Inc.
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




#include <memory>
#include <log4cxx/logger.h>

#include <query/Expression.h>
#include <query/Query.h>
#include <query/LogicalOperator.h>

#include "system/Exceptions.h"
#include "Exercise1.h"

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.Exercise1.LogicalExercise1"));

namespace scidb {

    using namespace std;

/**
 * @brief The operator: Exercise1().
 *
 * @par Synopsis:

 * @par Summary:

 *
 * @par Input:

 *
 * @par Output array:

 *
 * @par Examples:

 * @par Errors:
 *   n/a
 *
 * @par Notes:
 *   n/a
 *
 */
    class LogicalExercise1 : public LogicalOperator {

    public:


        LogicalExercise1(const string& logicalName, const string& alias):
                LogicalOperator(logicalName, alias)
        {
        }
        //OperatorParamPlaceholder PP
        //PlistRegex RE
        static PlistSpec const* makePlistSpec()
        {
            static PlistSpec argSpec {
                    { "", // positionals
                               RE(PP(PLACEHOLDER_INPUT)),
                    },
                    {"log", RE(PP(PLACEHOLDER_CONSTANT, TID_BOOL))},
                    {"global", RE(PP(PLACEHOLDER_CONSTANT, TID_BOOL))},
            };
            return &argSpec;
        }



        /**
         *  @see LogicalOperator::inferSchema()
         */
        ArrayDesc inferSchema(std::vector<ArrayDesc> schemas, std::shared_ptr<Query> query) {
            SCIDB_ASSERT(schemas.size() == 1);//한개의 array

            ArrayDesc const &desc = schemas[0];

            size_t nDims = desc.getDimensions().size();

            /// read input parameters (starting cell, ending cell)
            vector<int64_t> startingCell;
            vector<int64_t> endingCell;
            for (size_t i = 0; i < nDims; i ++) {
                int64_t dimension =
                        evaluate(((std::shared_ptr<OperatorParamLogicalExpression> &) _parameters[i])->getExpression(), TID_INT64).getInt64();
                startingCell.push_back(dimension);
                LOG4CXX_DEBUG(logger,"starting cell : "<<dimension);
            }
            for (size_t i = nDims; i < nDims * 2; i ++) {
                int64_t dimension =
                        evaluate(((std::shared_ptr<OperatorParamLogicalExpression> &) _parameters[i])->getExpression(), TID_INT64).getInt64();
                endingCell.push_back(dimension);
                LOG4CXX_DEBUG(logger,"ending cell : "<<dimension);
            }

            // output array schema 정의-----------------------------------------------------------
            Attributes outputAttributes;
            outputAttributes.push_back(
                    AttributeDesc("attribute_name",TID_DOUBLE,0,CompressorType::NONE));

            outputAttributes.addEmptyTagAttribute();

            Dimensions outputDimensions;
            for(size_t i = nDims; i < nDims; i++){
                outputDimensions.push_back(DimensionDesc("i", startingCell[i], endingCell[i], endingCell[i]-startingCell[i]+1, 0));
            }
            return ArrayDesc("outputArray", outputAttributes, outputDimensions,createDistribution(getSynthesizedDistType()) , query->getDefaultArrayResidency());
            //-----------------------------------------------------------------------------
        }

    private:
        static const std::string PROBE;
        static const std::string MATERIALIZE;
    };

    REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalExercise1, "Exercise1");

}  // namespace scidb
