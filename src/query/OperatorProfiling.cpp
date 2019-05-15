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
 * @file OperatorProfiling.cpp
 * @brief Stub for profiling the execution of Operators
 *
 *        executeWrapper is in a separate file so it is easy to replace this
 *        stub with profiling code, without comitting a particular set of
 *        profiling code to the trunk.
 *
 *        This stub version merely calls execute().
 */

#include <query/PhysicalOperator.h>


using namespace std;

namespace scidb
{

std::shared_ptr< Array> PhysicalOperator::executeWrapper(std::vector< std::shared_ptr< Array> >& arrays,
                                                           std::shared_ptr<Query> query)
{
        return execute(arrays, query);
}

} // namespace scidb
