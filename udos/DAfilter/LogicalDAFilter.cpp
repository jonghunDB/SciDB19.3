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
 * LogicalFilter.cpp
 *
 *  Created on: Apr 11, 2010
 *      Author: Knizhnik
 */

#include <query/LogicalOperator.h>
#include <query/Expression.h>

namespace scidb {

using namespace std;

/**
 * @brief The operator: filter().
 *
 * @par Synopsis:
 *   DAfilter( srcArray, expression )
 *
 * @par Summary:
 *   The filter operator returns an array the with the same schema as the input
 *   array. The result is identical to the input except that those cells for
 *   which the expression evaluates either false or null are marked as being
 *   empty.
 *
 * @par Input:
 *   - srcArray: a source array with srcAttrs and srcDims.
 *   - expression: an expression which takes a cell in the source array as input and evaluates to either True or False.
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
class LogicalDAFilter : public LogicalOperator
{
public:
    LogicalDAFilter(const std::string& logicalName,
                  const std::string& alias)
            : LogicalOperator(logicalName, alias)
    {
        _properties.tile = true;
    }

    static PlistSpec const* makePlistSpec()
    {
        static PlistSpec argSpec {
            { "", // positionals
              RE(RE::LIST, {
                 RE(PP(PLACEHOLDER_INPUT)),
                 RE(PP(PLACEHOLDER_EXPRESSION, TID_BOOL))
              })
            }
        };
        return &argSpec;
    }

    bool compileParamInTileMode(PlistWhere const& where,
                                string const&) override
    {
        return where[0] == 0;
    }

    /**
     * @note all the parameters are assembled in the _parameters member variable
     */

    ArrayDesc inferSchema(std::vector<ArrayDesc> schemas, std::shared_ptr<Query> query)
    {
        /* Check for a "global: true" parameter. */
        bool global = false;

        Parameter globalParam = findKeyword("global");
        if(globalParam)
        {
            global = evaluate(((std::shared_ptr<OperatorParamLogicalExpression>&)globalParam)->getExpression(),TID_BOOL).getBool();
        }

        /* Make the output schema.*/
        //여기서는 입력받은 schema를 그대로 사용하기 때문에 따로 변경할 필요가 없음. 하지만 일반적으로 udo를 만들때 schema를 설정해야 함
        //schemas vector를 불러와서 하나의 array인지 확인.
        assert(schemas.size() == 1);
        //example) filter(array ,attribute1 < 100 ) 하나의 attribute이나 dimension을 parameter로 씀
        assert(_parameters.size() == 1);
        assert(_parameters[0]->getParamType() == PARAM_LOGICAL_EXPRESSION);


        return schemas[0].addEmptyTagAttribute();
    }
};
REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalDAFilter,"DAFilter");
//DECLARE_LOGICAL_OPERATOR_FACTORY(LogicalDAFilter, "DAFilter" , "logicalDAFilter" )

}  // namespace scidb
