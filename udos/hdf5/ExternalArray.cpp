//
// Created by snmyj on 4/20/17.
//

#include <query/Query.h>
#include "HDF5Array.h"
#include "ExternalArray.h"

namespace scidb
{
namespace hdf5gateway
{

std::shared_ptr<scidb::CoordinateSet> ExternalArray::getChunkPositions() const
{
    return _coordinateSet;
}

int64_t ExternalArray::getMaxElementsInChunk(bool withOverlap) const
{
    using namespace std;
    auto& dims = getArrayDesc().getDimensions();
    size_t result = 1;
    for (auto& dim : dims) {
        if (withOverlap) {
            result *= dim.getChunkInterval();
        } else {
            result *= dim.getChunkInterval() + dim.getChunkOverlap();
        }
    }
    return result;
}

int64_t ExternalArray::getMaxElementsInChunk(Coordinates start, bool withOverlap) const
{
    using namespace std;
    auto& dims = getArrayDesc().getDimensions();
    size_t result = 1;
    auto i = 0;
    for (auto& dim : dims) {
        auto dimInterval = dim.getChunkInterval();
        if (withOverlap) dimInterval += dim.getChunkOverlap();
        if (dim.getEndMax() - start[i] + 1 < dimInterval)
            dimInterval = dim.getEndMax() - start[i] + 1;
        result *= dimInterval;
        ++i;
    }
    return result;
}

size_t ExternalArray::count() const
{
    return _coordinateSet->size();
}

}
}