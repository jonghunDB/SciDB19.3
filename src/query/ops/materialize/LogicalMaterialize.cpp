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
 * @file LogicalMaterialize.cpp
 *
 * @author roman.simakov@gmail.com
 * @brief This file implement logical operator get temp for
 * getting result of previous iteration.
 */

#include "query/LogicalOperator.h"
#include "query/QueryProcessor.h"

namespace scidb
{

/**
 * @brief The operator: materialize().
 *
 * @par Synopsis:
 *   materialize( srcArray, format )
 *
 * @par Summary:
 *   Produces a materialized version of an source array.
 *
 * @par Input:
 *   - srcArray: the sourcce array with srcDims and srcAttrs.
 *   - format: uint32, the materialize format.
 *
 * @par Output array:
 *        <
 *   <br>   srcAttrs
 *   <br> >
 *   <br> [
 *   <br>   srcDims
 *   <br> ]
 *
 * @par Examples:
 *   n/a
 *
 * @par Errors:
 *   n/a
 *
 * @par Notes:
 *   n/a
 *
 */
class LogicalMaterialize: public LogicalOperator
{
public:
    LogicalMaterialize(const std::string& logicalName, const std::string& alias):
        LogicalOperator(logicalName, alias)
    {
    }

    static PlistSpec const* makePlistSpec()
    {
        static PlistSpec argSpec {
            { "", // positionals
              RE(RE::LIST, {
                 RE(PP(PLACEHOLDER_INPUT)),
                 RE(PP(PLACEHOLDER_CONSTANT, TID_UINT32))
              })
            }
        };
        return &argSpec;
    }

    /**
     * The schema of output array is the same as input
     */
    ArrayDesc inferSchema(std::vector< ArrayDesc> inputSchemas, std::shared_ptr< Query> query)
    {
        assert(inputSchemas.size() == 1);
        return inputSchemas[0];
    }

};

DECLARE_LOGICAL_OPERATOR_FACTORY(LogicalMaterialize, "_materialize")

} //namespace
