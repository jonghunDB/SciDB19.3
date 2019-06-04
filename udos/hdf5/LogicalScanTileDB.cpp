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

/*
 * LogicalScanTileDB.cpp
 *
 *  Created on: April 20 2017
 *      Author: Haoyuan Xing<hbsnmyj@gmail.com>
 */

#include <log4cxx/logger.h>
#include <query/Operator.h>
#include <system/SystemCatalog.h>
#include <usr_namespace/NamespacesCommunicator.h>
#include <usr_namespace/Permissions.h>
#include "LogicalScanExternalArray.h"

namespace scidb
{
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.logical_scan_tiledb"));
/**
 * @brief Operator: scan_tiledb()
 */
class LogicalScanTileDB : public LogicalScanExternalArray
{
public:
    LogicalScanTileDB(const std::string& logicalName, const std::string& alias)
            : LogicalScanExternalArray(logicalName, alias)
    {
    }

    std::vector<std::shared_ptr<OperatorParamPlaceholder> >
    nextVaryParamPlaceholder(const std::vector< ArrayDesc> &schemas)
    {
        std::vector<std::shared_ptr<OperatorParamPlaceholder> > res;
        res.push_back(END_OF_VARIES_PARAMS());
        switch (_parameters.size()) {
          case 0:
            assert(false);
            break;
          case 1:
            res.push_back(PARAM_CONSTANT("string"));
            break;
        }
        return res;
    }
};

DECLARE_LOGICAL_OPERATOR_FACTORY(LogicalScanTileDB, "scan_tiledb")
}// namespace scidb