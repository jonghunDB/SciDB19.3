//
// Created by snmyj on 4/23/17.
//

#include "TileDBBridgeArray.h"

namespace scidb
{
namespace hdf5gateway
{

class TileDBBridgeIterator : public ExternalArrayIterator<TileDBBridgeArray>
{
public:
    TileDBBridgeIterator(std::weak_ptr<const TileDBBridgeArray> array, AttributeID attrId,
            const std::weak_ptr<Query> query)
            : ExternalArrayIterator<TileDBBridgeArray>(array, attrId, query)
    {
        auto arrayPtr = _array.lock();
        if(_isEmptyBitmap == false) {
            _attributeName = arrayPtr->getTileDBArrayDesc().
                getAttribute(attrId).getName();
//            std::cerr << "attribute name: " << _attributeName << std::endl;
            _attributes[0] = _attributeName.c_str();
        }
        _needRead = true;
        _reader.reset(new TileDBArrayDenseReader<int64_t>(arrayPtr->getTileDBArray(),
                _attributes, 1, TILEDB_ARRAY_READ));
    }

    ConstChunk const& getChunk();
private:
    std::unique_ptr<TileDBArrayDenseReader<int64_t>> _reader;
    std::unique_ptr<MemChunk> _chunk;
    std::string _attributeName;
    const char* _attributes[1];

    void readChunk();
};

TileDBBridgeArray::TileDBBridgeArray(const ArrayDesc &desc, TileDBArrayDesc &edesc,
                         const std::shared_ptr<Query> &query, arena::ArenaPtr arena,
                         int64_t chunkAllocation, int64_t chunkAllocParam)
    : ExternalArray(desc, query, chunkAllocation, chunkAllocParam), _edesc(edesc) {
    _tarray.reset(new TileDBArray(_edesc.getPath()));
}

std::shared_ptr<ConstArrayIterator> TileDBBridgeArray::getConstIterator(AttributeID attrID) const {
    auto shared_ptr = shared_from_this();
    return std::make_shared<TileDBBridgeIterator>
            (std::weak_ptr<const TileDBBridgeArray>(shared_ptr), attrID, _query);
}

ConstChunk const &TileDBBridgeIterator::getChunk() {
    if(_needRead)
        readChunk();
    return *_chunk;
}

void TileDBBridgeIterator::readChunk() {
    auto arrayPtr = _array.lock();

    bool isEmptyBitmap = (_attrId == arrayPtr->getArrayDesc().getEmptyBitmapAttribute()->getId());
    _chunk.reset(new MemChunk);

    auto attrDesc = arrayPtr->getArrayDesc().getAttributes()[_attrId];
    int64_t nElems = arrayPtr->getMaxElementsInChunk(getPosition());
    Address address(_attrId, getPosition());
    _chunk->initialize(arrayPtr.get(), &arrayPtr->getArrayDesc(), address, 0);

    if(isEmptyBitmap) {
        auto emptyBitmap = std::make_shared<RLEEmptyBitmap>(nElems);
        _chunk->allocate(emptyBitmap->packedSize());
        auto data = _chunk->getData();
        emptyBitmap->pack((char *)data);
    } else {
        size_t elemSize = attrDesc.getSize();
        size_t dataSize = elemSize * nElems;
        size_t rawSize = ConstRLEPayload::perdictPackedize(1, dataSize);
        _chunk->allocate(rawSize);
        auto data = _chunk->getData();
        auto rawData = ConstRLEPayload::initDensePackedChunk((char *) data, dataSize, 0, elemSize, nElems, false);
        auto firstCoordinates = _chunk->getFirstPosition(true);
        auto lastCoordinates = _chunk->getLastPosition(true);
        /* right now we initalize a reader for every chunk */
        /* right now we assume that the coordinate is of type
         * int_64t */


        _reader->updatePosition(firstCoordinates, lastCoordinates);
        void* buffers[1] = {rawData};
        size_t sizes[1] = {dataSize};
        _reader->readToBuffer(buffers, sizes);
    }
    _needRead = false;
}

}
}
