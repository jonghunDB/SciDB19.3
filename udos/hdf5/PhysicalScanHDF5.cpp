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
 * PhysicalScanHDF5.cpp
 *
 *  Created on: June 8 2016
 *      Author: Haoyuan Xing<hbsnmyj@gmail.com>
 */


#include <memory>
#include <log4cxx/logger.h>
#include <query/Operator.h>
#include <fstream>
#include "HDF5Array.h"
#include "PhysicalScanExternalArray.h"

namespace scidb
{
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.physical_scan_hdf5"));

class PhysicalScanHDF5: public PhysicalScanExternalArray
{
public:
    PhysicalScanHDF5(const std::string& logicalName,
                              const std::string& physicalName,
                              const std::vector <std::shared_ptr<OperatorParam>>& parameters,
                              const ArrayDesc& schema):
            PhysicalScanExternalArray(logicalName, physicalName, parameters, schema) {
        LOG4CXX_DEBUG(logger, "hdf5gateway::scan_hdf5() array = " << _arrayName.c_str() << "\n" );
    }
    std::shared_ptr<Array> execute(std::vector<std::shared_ptr<Array>>& inputArrays,
                                   std::shared_ptr<Query> query)
    {
        using hdf5gateway::HDF5Array;
        using hdf5gateway::HDF5ArrayDesc;
        using hdf5gateway::HDF5Type;
        HDF5ArrayDesc h5desc;

        std::string config_file;
        if(_parameters.size() >= 2) {
            config_file = ((std::shared_ptr<OperatorParamPhysicalExpression>&)
                    _parameters[1])->getExpression()->evaluate().getString();
        } else {
            char *home = getenv("HOME");
            config_file = std::string(home) + "/scidb_hdf5.config";
        }

        int64_t chunkAllocation = 1;
        int64_t chunkAllocParam = 8;
        if(_parameters.size() >= 3) {
            chunkAllocation = ((std::shared_ptr<OperatorParamPhysicalExpression>&)
                    _parameters[2])->getExpression()->evaluate().getInt64();
        }
        if(_parameters.size() >= 4) {
            chunkAllocParam = ((std::shared_ptr<OperatorParamPhysicalExpression>&)
                    _parameters[3])->getExpression()->evaluate().getInt64();
        }

        std::ifstream infile(config_file);
        std::string hdf5_file;
        infile >> hdf5_file;
        std::string dataset_name, attribute_name;
        std::map<std::string, std::string> dataset_names;
        while(infile >> attribute_name >> dataset_name) {
            dataset_names[attribute_name] = dataset_name;
        }

        auto attributes = _schema.getAttributes(true);
        for(auto& attribute: attributes) {
            if(dataset_names.find(attribute.getName()) != dataset_names.end()) {
                h5desc.addAttribute(hdf5_file, dataset_names[attribute.getName()],
                                    HDF5Type(attribute.getType()));
            } else {
                h5desc.addAttribute(hdf5_file, attribute.getName(), HDF5Type(attribute.getType()));
            }
        }

        return std::make_shared<HDF5Array>(_schema, h5desc, query, _arena, chunkAllocation, chunkAllocParam);
    }

};

DECLARE_PHYSICAL_OPERATOR_FACTORY(PhysicalScanHDF5, "scan_hdf5", "impl_hdf5_scan")

} // namespace scidb