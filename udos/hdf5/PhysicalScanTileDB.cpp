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
 * PhysicalScanTileDB.cpp
 *
 *  Created on: April 20 2017
 *      Author: Haoyuan Xing<hbsnmyj@gmail.com>
 */


#include <memory>
#include <log4cxx/logger.h>
#include <query/Operator.h>
#include <fstream>
#include "PhysicalScanExternalArray.h"
#include "TileDBBridgeArray.h"

namespace scidb
{
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.physical_scan_tiledb"));

class PhysicalScanTileDB: public PhysicalScanExternalArray
{
public:
    PhysicalScanTileDB(const std::string& logicalName,
                              const std::string& physicalName,
                              const std::vector <std::shared_ptr<OperatorParam>>& parameters,
                              const ArrayDesc& schema):
            PhysicalScanExternalArray(logicalName, physicalName, parameters, schema) {
    }

    std::shared_ptr<Array> execute(std::vector<std::shared_ptr<Array>>& inputArrays,
                                   std::shared_ptr<Query> query)
    {

        using hdf5gateway::TileDBArrayDesc;
        using hdf5gateway::TileDBBridgeArray;

        std::string config_file;
        if(_parameters.size() >= 2) {
            config_file = ((std::shared_ptr<OperatorParamPhysicalExpression>&)
                    _parameters[1])->getExpression()->evaluate().getString();
        } else {
            char *home = getenv("HOME");
            config_file = std::string(home) + "/scidb_hdf5.config";
        }

        std::ifstream infile(config_file);
        std::string tiledb_path;
        infile >> tiledb_path;
        std::string dataset_name, attribute_name;
        std::map<std::string, std::string> dataset_names;
        while(infile >> attribute_name >> dataset_name) {
            dataset_names[attribute_name] = dataset_name;
        }

        TileDBArrayDesc edesc(tiledb_path);
        auto attributes = _schema.getAttributes(true);
        for(auto& attribute: attributes) {
            if(dataset_names.find(attribute.getName()) != dataset_names.end()) {
                edesc.addAttribute(dataset_names[attribute.getName()], attribute.getType());
            } else {
                edesc.addAttribute(attribute.getName(), attribute.getType());
            }
        }

        int64_t chunkAllocation = 1;
        int64_t chunkAllocParam = 8;

        return std::make_shared<TileDBBridgeArray>(_schema, edesc, query,
                                                   _arena, chunkAllocation, chunkAllocParam);
    }

};

DECLARE_PHYSICAL_OPERATOR_FACTORY(PhysicalScanTileDB, "scan_tiledb", "impl_scan_tiledb")

} // namespace scidb
