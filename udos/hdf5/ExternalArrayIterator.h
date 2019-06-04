//
// Created by snmyj on 4/21/17.
//

#ifndef SCIDB_EXTERNALARRAYITERATOR_H
#define SCIDB_EXTERNALARRAYITERATOR_H

#include <query/Query.h>
#include <system/Exceptions.h>

namespace scidb
{
namespace hdf5gateway
{

inline std::string to_string(const Coordinates& coordniates) {
    std::stringstream ss;
    std::ostream_iterator<Coordinates::value_type> oIter(ss, ", ");
    std::copy(coordniates.begin(), coordniates.end(), oIter);
    return ss.str();
}

template<typename TArray>
class ExternalArrayIterator
        : public ::scidb::ConstArrayIterator
{
public:
    ExternalArrayIterator(std::weak_ptr<const TArray> array, AttributeID attrId,
            const std::weak_ptr<Query> query) : _array(array), _attrId(attrId),
                                                _query(query), _needRead(true)  {
        auto arrayPtr = _array.lock();
        _coordinateSet = arrayPtr->getChunkPositions();
        _coordinateIter = arrayPtr->getChunkPositions()->begin();
        _isEmptyBitmap = (_attrId == arrayPtr->getArrayDesc().getEmptyBitmapAttribute()->getId());
    }

    ~ExternalArrayIterator(){}

    ExternalArrayIterator(const ExternalArrayIterator&) = delete;

    ExternalArrayIterator(ExternalArrayIterator&&) = default;

    ExternalArrayIterator& operator=(const ExternalArrayIterator&) = delete;

    ExternalArrayIterator& operator=(ExternalArrayIterator&&) = default;

    virtual bool end() override;

    virtual void operator++() override;

    virtual std::vector <Coordinate> const& getPosition() override;

    virtual bool setPosition(std::vector <Coordinate> const& pos) override;

    virtual void reset() override;

    bool isEmptyBitmap() { return _isEmptyBitmap; }

protected:
    std::weak_ptr<const TArray> _array;
    AttributeID _attrId;
    std::weak_ptr<Query> _query;
    std::shared_ptr<CoordinateSet> _coordinateSet;
    CoordinateSet::iterator _coordinateIter;
    bool _isEmptyBitmap;
    bool _needRead;

    /*
         * returns the first chunk that contains position pos.
         */
    CoordinateSet::iterator findChunk(Coordinates pos);

    bool isCoordinatesInChunk(const Coordinates &chunkStart, const Coordinates &pos);
};

template<typename TArray>
bool ExternalArrayIterator<TArray>::end()
{
    return _coordinateIter == _coordinateSet->end();
}

template<typename TArray>
void ExternalArrayIterator<TArray>::operator++()
{
    ++_coordinateIter;
    _needRead = true;
}

template<typename TArray>
std::vector<Coordinate> const& ExternalArrayIterator<TArray>::getPosition()
{
    return *_coordinateIter;
}

template<typename TArray>
bool ExternalArrayIterator<TArray>::setPosition(std::vector <Coordinate> const& pos)
{
    static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.physical_scan_external"));
    LOG4CXX_TRACE(logger, "ExternalArrayIterator: setting pos " << to_string(*_coordinateIter) <<
                                                            " " << to_string(pos) << " attrId=" << _attrId);
    auto chunkPosIter = findChunk(pos);
    if(chunkPosIter == _coordinateSet->end()) return false;
    else {
        _coordinateIter = chunkPosIter;
        _needRead = true;
        return true;
    }
}

template<typename TArray>
void ExternalArrayIterator<TArray>::reset()
{
    _coordinateIter = _coordinateSet->begin();
    _needRead = true;
}

template<typename TArray>
CoordinateSet::iterator ExternalArrayIterator<TArray>::findChunk(
        Coordinates pos)
{
    auto arrayPtr = _array.lock();
    auto desc = arrayPtr->getArrayDesc();
    desc.getChunkPositionFor(pos);
    return _coordinateSet->find(pos);
}

template<typename TArray>
bool ExternalArrayIterator<TArray>::isCoordinatesInChunk(const Coordinates &chunkStart, const Coordinates &pos)
{
    auto arrayPtr = _array.lock();
    auto dims = arrayPtr->getArrayDesc().getDimensions();
    for(unsigned i=0;i<chunkStart.size();++i) {
        if(chunkStart[i] > pos[i] || pos[i] >= chunkStart[i] + dims[i].getChunkInterval()) {
            return false;
        }
    }
    return true;
}
}
}


#endif //SCIDB_EXTERNALARRAYITERATOR_H
