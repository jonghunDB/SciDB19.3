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
 * @file PhysicalStub.cpp
 * @author roman.simakov@gmail.com
 *
 * @brief Stub for writing plugins with physical operators
 */

#include "query/PhysicalOperator.h"

namespace scidb
{

class PhysicalStub: public PhysicalOperator
{
public:
    PhysicalStub(const std::string& logicalName, const std::string& physicalName, const Parameters& parameters, const ArrayDesc& schema):
	    PhysicalOperator(logicalName, physicalName, parameters, schema)
	{
	}

    std::shared_ptr<Array> execute(std::vector<std::shared_ptr<Array> >& inputArrays, std::shared_ptr<Query> query)
	{
        /**
         * See built-in operators implementation for example
         */
        return std::shared_ptr<Array>();
	}
};

REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalStub, "stub", "stub_impl");

} //namespace
