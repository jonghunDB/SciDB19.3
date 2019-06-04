//
// Created by snmyj on 4/20/17.
//

#ifndef SCIDB_PHYSICALSCANEXTERNALARRAY_H
#define SCIDB_PHYSICALSCANEXTERNALARRAY_H

#include <query/Operator.h>

namespace scidb
{


class PhysicalScanExternalArray
        : public ::scidb::PhysicalOperator
{
public:
    PhysicalScanExternalArray(const std::string& logicalName,
                              const std::string& physicalName,
                              const std::vector <std::shared_ptr<OperatorParam>>& parameters,
                              const ArrayDesc& schema):
            PhysicalOperator(logicalName, physicalName, parameters, schema)
    {
        _arrayName = std::dynamic_pointer_cast<OperatorParamReference>(parameters[0])->getObjectName();
    }

    virtual RedistributeContext getOutputDistribution(const std::vector<RedistributeContext> & inputDistributions,
                                                      const std::vector< ArrayDesc> & inputSchemas) const
    {
        return RedistributeContext(_schema.getDistribution(), _schema.getResidency());
    }

    virtual PhysicalBoundaries getOutputBoundaries(const std::vector<PhysicalBoundaries>& inputBoundaries,
                                                   const std::vector<ArrayDesc>& inputSchemas) {
        std::vector <Coordinate> lowBoundary = _schema.getLowBoundary();
        std::vector <Coordinate> highBoundary = _schema.getHighBoundary();

        return PhysicalBoundaries(lowBoundary, highBoundary);
    }
protected:
    std::string _arrayName;
};

}


#endif //SCIDB_PHYSICALSCANEXTERNALARRAY_H
