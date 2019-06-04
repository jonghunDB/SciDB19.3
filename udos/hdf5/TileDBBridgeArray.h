//
// Created by snmyj on 4/23/17.
//

#ifndef SCIDB_TILEDBARRAY_H_H
#define SCIDB_TILEDBARRAY_H_H

#include <array/Array.h>
#include <query/Parser.h>
#include <util/Arena.h>
#include <memory>
#include <cstdint>
#include "ExternalArray.h"
#include "TileDBArrayDesc.h"
#include "ExternalArrayIterator.h"
#include <smgr/io/TileDBStorage.h>

namespace scidb
{
namespace hdf5gateway
{



class TileDBBridgeArray : public std::enable_shared_from_this<TileDBBridgeArray>, public ExternalArray
{
public:
    TileDBBridgeArray(const ArrayDesc& desc, TileDBArrayDesc& edesc,
                const std::shared_ptr<Query>& query, arena::ArenaPtr arena, int64_t chunkAllocation,
                int64_t chunkAllocParam);
    virtual std::shared_ptr<ConstArrayIterator> getConstIterator(AttributeID attrID) const override;
    const TileDBArray& getTileDBArray() const { return *_tarray; }
    const TileDBArrayDesc& getTileDBArrayDesc() const { return _edesc; }
private:
    TileDBArrayDesc _edesc;
    std::unique_ptr<TileDBArray> _tarray;
};

}
}

#endif //SCIDB_TILEDBARRAY_H_H
