//
// Created by snmyj on 4/20/17.
//

#ifndef SCIDB_EXTERNALARRAY_H
#define SCIDB_EXTERNALARRAY_H

#include <array/Array.h>
#include <array/Metadata.h>
#include <array/MemChunk.h>
#include <query/Parser.h>
#include <query/Query.h>
#include <sstream>
#include "HDF5ArrayDesc.h"

namespace scidb
{
namespace hdf5gateway
{

static log4cxx::LoggerPtr external_array_logger(log4cxx::Logger::getLogger("scidb.query.ops.external_array"));

class ExternalArray
        : public Array
{
public:
    ExternalArray(const ArrayDesc& desc, const std::shared_ptr<Query>& query,
                  int64_t chunkAllocation = 1, int64_t chunkAllocParam = 0
    ) : _desc(desc), _coordinateSet(new CoordinateSet)
    {
        setQuery(query);
        auto& dims = getArrayDesc().getDimensions();
        auto instance_id = query->getInstanceID();
        auto total_instances = query->getInstancesCount();
        auto chunk_id = 0;
        std::cerr << "Chunk Allocation = " << chunkAllocation << std::endl;
        std::cerr << "Chunk Allocation Parameter = " << chunkAllocParam << std::endl;

        /* Generates all the chunks, and save the chunks that are allocated to this instance. */
        /* Right now we use a simple round-robin algorithm. */
        Coordinates coordinates(dims.size(), 0);
        while(lessThanOrEqual(coordinates, dims)) {
            LOG4CXX_TRACE(external_array_logger,
                    "ExternalArray: adding chunk " << to_string(coordinates));

            if(distributeChunk(chunkAllocation, chunkAllocParam, total_instances,
                  chunk_id, coordinates, dims) == instance_id) {
                _coordinateSet->insert(coordinates);
            }
            getCoordinatesOfNextChunk(coordinates, dims);
            ++chunk_id;
        }
    }
    ExternalArray(const ExternalArray &other) = delete;

    ExternalArray(ExternalArray &&other) = delete;

    ExternalArray &operator=(const ExternalArray &array) = delete;

    ExternalArray&& operator=(ExternalArray &&array) = delete;

    virtual ~ExternalArray() {}

    /* inherited methods */
    virtual const ArrayDesc& getArrayDesc() const override { return _desc; }

    virtual bool hasChunkPositions() const override { return true; }

    virtual bool isCountKnown() const override { return true; }

    virtual size_t count() const override;

    /* As HDF5 library is not thread-safe as of version 1.10, a mtuex is used
         * to synchroize HDF5 calls. */
    Mutex& getMutex() const { return _mutex; }

    virtual std::shared_ptr<CoordinateSet> getChunkPositions() const override;

    /**
         * @brief return the upper bound of elements in a chunk.
         */
    int64_t getMaxElementsInChunk(bool withOverlap = false) const;

    /**
         * @brief return the number of elements in a chunk starting at start
         */
    int64_t getMaxElementsInChunk(Coordinates start, bool withOverlap = false) const;

protected:
    ArrayDesc _desc;
    /* At array construction time, we calculate all the position of chunks
            * according to the dimension, and save the local instances to the set
            * for iteration. */
    std::shared_ptr<CoordinateSet> _coordinateSet;
    std::weak_ptr<Query> _query;

    inline InstanceID distributeChunk(int64_t chunkAllocation, int64_t chunkAllocParam,
                                      int64_t numInstances, int64_t chunkID, Coordinates const& coordinates,
                                      Dimensions const& dims);

    inline static void getCoordinatesOfNextChunk(Coordinates& coordinates, const Dimensions &dims);

    /* check if a coordinate is within the dimensions provided. */
    inline static bool lessThanOrEqual(Coordinates coordinates, const Dimensions &dims);

private:
    std::string to_string(const Coordinates& coordinates) {
        std::stringstream ss;
        ss << "[";
        for(auto v : coordinates)
            ss << v << ",";
        ss << "]";
        return ss.str();
    }
    Mutex mutable _mutex;
};

inline bool ExternalArray::lessThanOrEqual(const Coordinates coordinates,
                                                               const Dimensions& dims)
{
    SCIDB_ASSERT(dims.size() != 0);
    SCIDB_ASSERT(coordinates.size() == dims.size());
    for (size_t i = 0; i < dims.size(); i++) {
        if (coordinates[i] > dims[i].getEndMax())
            return false;
    }
    return true;
}

inline void ExternalArray::getCoordinatesOfNextChunk(Coordinates& coordinates,
                                                                         const Dimensions& dims)
{
    SCIDB_ASSERT(dims.size() != 0);
    SCIDB_ASSERT(coordinates.size() == dims.size());
    bool carry = false;
    long i = coordinates.size();
    do {
        --i;
        coordinates[i] += dims[i].getChunkInterval();
        if (i > 0 && coordinates[i] > dims[i].getEndMax()) {
            coordinates[i] = dims[i].getStartMin();
            carry = true;
        } else {
            carry = false;
        }
    } while (carry && i > 0);
}

inline scidb::InstanceID
ExternalArray::distributeChunk(int64_t chunkAllocation, int64_t chunkAllocParam,
                                                   int64_t numInstances, int64_t chunkID,
                                                   Coordinates const& coordinates,
                                                   Dimensions const& dims)
{
    if (chunkAllocation == 1) {
        return chunkID % numInstances;
    } else if (chunkAllocation == -1) {
        return (chunkID / chunkAllocParam) % numInstances;
    } else if (chunkAllocation == 2) {
        //from psRow
        const uint64_t dim0Length = dims[0].getLength();
        InstanceID destInstanceId =
                (coordinates[0] - dims[0].getStartMin()) / dims[0].getChunkInterval()
                / (((dim0Length + dims[0].getChunkInterval() - 1) / dims[0].getChunkInterval() +
                    numInstances - 1) / numInstances);

        return destInstanceId % numInstances;
    }
    return 0;
}

}
}


#endif //SCIDB_EXTERNALARRAY_H
