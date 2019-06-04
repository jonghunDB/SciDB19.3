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
 * HDF5Array.cpp
 *
 *  Created on: June 8 2016
 *      Author: Haoyuan Xing<hbsnmyj@gmail.com>
 */

#include "HDF5Array.h"
#include "ExternalArrayIterator.h"

#ifdef INSTRUMENT_HDF5_ARRAY_ITERATOR
#include <ctime>
#endif

namespace scidb
{
namespace hdf5gateway
{

#ifdef INSTRUMENT_HDF5_ARRAY_ITERATOR
struct LogEntry
{
	pthread_t tid;
	struct timespec ts;
    char event;

	std::string serialize()
	{
		std::ostringstream oss;
		oss << this->event << "\t"
			<< this->tid << "\t"
			<< this->ts.tv_sec << "\t"
			<< this->ts.tv_nsec;
		return oss.str();
	}

};

template <typename T>
inline
T atomic_increment(volatile T* ptr, T by=1)
{
	T ret;
#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
	ret = __sync_fetch_and_add(ptr, by);
#else
#error Compiler not supported
#endif
	return ret;
}

struct Log
{
	Log()
	{
		CurrentLogEntry = 0;
		entries = new LogEntry[MaxEntries];
	};

	~Log()
	{
		assert(CurrentLogEntry < LogEntry::MaxEntries);
        std::string bytesString = "HDF5BytesRead = " + std::to_string(BytesRead);
        std::cerr << bytesString << std::endl;
        std::string sizeString = "LogSize = " + std::to_string(CurrentLogEntry);
        std::cerr << sizeString << std::endl;
		for (auto i=0; i<CurrentLogEntry; ++i)
		{
			std::cerr << entries[i].serialize() << std::endl;
		}

		CurrentLogEntry = 0;
		delete[] entries;
	};

	inline void append(char ev)
	{
		unsigned long long entry = atomic_increment(&CurrentLogEntry);
		assert(entry < MaxEntries);
		LogEntry* rec = &entries[entry];

		rec->event = ev;
		rec->tid = pthread_self();
		clock_gettime(CLOCK_MONOTONIC, &rec->ts);
	}

    inline void addBytes(unsigned long long bytes) {
        atomic_increment(&BytesRead,bytes);
    }

	const unsigned long long MaxEntries = 1ull * 1024 * 1024;
	volatile unsigned long long CurrentLogEntry;
    volatile unsigned long long BytesRead;
	LogEntry* entries;
};
#define TRACE( x ) log.append( x )
#define TRACE_BYTES( bytes ) log.addBytes( bytes )
#else
#define TRACE( x )
#define TRACE_BYTES( bytes )
#endif

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("scidb.query.ops.physical_scan_hdf5"));



class HDF5ArrayIterator : public ExternalArrayIterator<HDF5Array>
{
public:
    HDF5ArrayIterator(std::weak_ptr<const HDF5Array> array,
                      AttributeID attId,
                      const std::weak_ptr<Query> query);;

    virtual ConstChunk const& getChunk() override;

private:

    std::unique_ptr<HDF5File> _hdf5File;
    std::unique_ptr<HDF5ActualDataset> _hdf5Dataset;
    std::unique_ptr<MemChunk> _chunk;

    /* check if we need to read the data from disk */
    void readChunk();

#ifdef INSTRUMENT_HDF5_ARRAY_ITERATOR
	Log log;
#endif
};

HDF5ArrayIterator::HDF5ArrayIterator(std::weak_ptr<const HDF5Array> array, AttributeID attId,
                                     const std::weak_ptr<Query> query)
    : ExternalArrayIterator<HDF5Array>(array, attId, query)
{
    auto arrayPtr = _array.lock();
    LOG4CXX_TRACE(logger, "HDF5ArrayIterator: Initializing Iterator array = "
                          << arrayPtr->getName() <<" attrId=" << attId);


    if(_isEmptyBitmap == false) {
        const HDF5ArrayAttribute& h5Attr = arrayPtr->getHDF5ArrayDesc().getAttribute(_attrId);
        ScopedMutexLock lock(arrayPtr->getMutex());

        _hdf5File = std::make_unique<HDF5File>(h5Attr.getFilePath(), HDF5File::OpenOption::kRDONLY);
        _hdf5Dataset = std::make_unique<HDF5ActualDataset>(*_hdf5File, h5Attr.getDatasetName());
    }
}

ConstChunk const& HDF5ArrayIterator::getChunk()
{
    LOG4CXX_TRACE(logger, "HDF5ArrayIterator: getting chunk " << to_string(*_coordinateIter)
                          << " attrId=" << _attrId);
    if(_needRead)
        readChunk();

    return *_chunk;
}

void HDF5ArrayIterator::readChunk()
{
	TRACE('1');
    auto arrayPtr = _array.lock();
	TRACE('G');
    bool isEmptyBitmap = (_attrId == arrayPtr->getArrayDesc().getEmptyBitmapAttribute()->getId());

    _chunk.reset(new MemChunk);

    auto attrDesc = arrayPtr->getArrayDesc().getAttributes()[_attrId];
    int64_t nElems = arrayPtr->getMaxElementsInChunk(getPosition());
    Address address(_attrId, getPosition());
    _chunk->initialize(arrayPtr.get(), &arrayPtr->getArrayDesc(), address, 0);



    if(isEmptyBitmap) {
        /* Just create a dense bitmap */
        auto emptyBitmap = std::make_shared<RLEEmptyBitmap>(nElems);
        _chunk->allocate(emptyBitmap->packedSize());
        auto data = _chunk->getData();
        emptyBitmap->pack((char *)data);
    } else {
        /* initalize a fully dense chunk, allocate the space, and then read data */
        size_t elemSize = attrDesc.getSize();
        size_t dataSize = elemSize * nElems;
        size_t rawSize = ConstRLEPayload::perdictPackedize(1, dataSize);
        _chunk->allocate(rawSize);
        auto data = _chunk->getData();
        auto rawData = ConstRLEPayload::initDensePackedChunk((char *) data, dataSize, 0, elemSize, nElems, false);
        auto firstCoordinates = _chunk->getFirstPosition(true);
        H5Coordinates target_pos(firstCoordinates.begin(), firstCoordinates.end());

		TRACE('2');
        ScopedMutexLock lock(arrayPtr->getMutex());
		TRACE('R');
        TRACE_BYTES(dataSize);
        _hdf5Dataset->readChunk(*_chunk, target_pos, (void*)rawData);
    }
    _needRead = false;

	TRACE('F');
}

HDF5Array::HDF5Array(scidb::ArrayDesc const& desc,
                     scidb::hdf5gateway::HDF5ArrayDesc& h5desc,
                     const std::shared_ptr<Query>& query,
                     arena::ArenaPtr arena,
                     int64_t chunkAllocation,
                     int64_t chunkAllocParam)
        : ExternalArray(desc, query, chunkAllocation, chunkAllocParam), _arena(arena), _h5desc(h5desc)
{
}

std::shared_ptr<ConstArrayIterator> HDF5Array::getConstIterator(AttributeID attr) const
{
    auto shared_ptr = shared_from_this();
    return std::make_shared<HDF5ArrayIterator>
            (std::weak_ptr<const HDF5Array>(shared_ptr), attr, _query);
}

}
} // namespace scidb

