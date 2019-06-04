//
// Created by snmyj on 4/20/17.
//

#ifndef SCIDB_LOGICALSCANEXTERNALARRAY_H
#define SCIDB_LOGICALSCANEXTERNALARRAY_H

#include <query/Operator.h>

namespace scidb
{


class LogicalScanExternalArray
        : public ::scidb::LogicalOperator
{

public:
    LogicalScanExternalArray(const std::string& logicalName, const std::string& alias):
            LogicalOperator(logicalName, alias)
    {
        ADD_PARAM_IN_ARRAY_NAME2(PLACEHOLDER_ARRAY_NAME_VERSION | PLACEHOLDER_ARRAY_NAME_INDEX_NAME);
        _properties.tile = true;
        ADD_PARAM_VARIES();          //2
    }



    std::string inferPermissions(std::shared_ptr<Query>& query)
    {
        std::string permissions;
        permissions.push_back(scidb::permissions::namespaces::ReadArray);
        return permissions;
    }

    void inferArrayAccess(std::shared_ptr<Query>& query)
    {
        LogicalOperator::inferArrayAccess(query);

        const std::string& arrayNameOrg =
                ((std::shared_ptr<OperatorParamReference>&) _parameters.front())->getObjectName();

        std::string namespaceName;
        std::string arrayName;
        query->getNamespaceArrayNames(arrayNameOrg, namespaceName, arrayName);
                ArrayDesc srcDesc;
        scidb::namespaces::Communicator::getArrayDesc(
                namespaceName, arrayName, SystemCatalog::ANY_VERSION, srcDesc);
    }

    ArrayDesc inferSchema(std::vector<ArrayDesc> inputSchemas, std::shared_ptr<Query> query)
    {
        assert(inputSchemas.size() == 0);
        assert(_parameters.size() == 1);
        assert(_parameters[0]->getParamType() == PARAM_ARRAY_REF);

        std::shared_ptr<OperatorParamArrayReference>& arrayRef = (std::shared_ptr<OperatorParamArrayReference>&) _parameters[0];
        assert(arrayRef->getArrayName().find('@') == std::string::npos);
        assert(ArrayDesc::isNameUnversioned(arrayRef->getObjectName()));

        if (arrayRef->getVersion() == ALL_VERSIONS) {
            throw USER_QUERY_EXCEPTION(SCIDB_SE_INFER_SCHEMA, SCIDB_LE_WRONG_ASTERISK_USAGE2, _parameters[0]->getParsingContext());
        }

        ArrayDesc schema;
        const std::string &arrayNameOrg = arrayRef->getObjectName();
        std::string namespaceName;
        std::string arrayName;
        query->getNamespaceArrayNames(arrayNameOrg, namespaceName, arrayName);

        ArrayID arrayId = query->getCatalogVersion(namespaceName, arrayName);
        scidb::namespaces::Communicator::getArrayDesc(
            namespaceName,
            arrayName,
            arrayId,
            arrayRef->getVersion(),
            schema);

        schema.addAlias(arrayNameOrg);
        schema.setNamespaceName(namespaceName);
        //we set the distribution to psUndefined for the time being
        schema.setDistribution(ArrayDistributionFactory::getInstance()
                                       ->construct(psUndefined, DEFAULT_REDUNDANCY));
        return schema;
    }
};

}


#endif //SCIDB_LOGICALSCANEXTERNALARRAY_H
