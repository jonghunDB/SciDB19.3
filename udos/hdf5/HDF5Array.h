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
 * HDF5Array.h
 *
 *  Created on: June 8 2016
 *      Author: Haoyuan Xing<hbsnmyj@gmail.com>
 */
#include <array/Array.h>
#include <array/MemChunk.h>
#include <util/Arena.h>
#include <query/Parser.h>
#include "HDF5ArrayDesc.h"
#include "ExternalArray.h"

#ifndef SCIDB_HDF5ARRAY_H
#define SCIDB_HDF5ARRAY_H

namespace scidb
{
namespace hdf5gateway
{

/**
 * @brief A read-only HDF5 array.
 * This array allocate the chunks to the instances at run time, hench
 * assumes that the HDF5 file could be accessed by all the instances.
 */
class HDF5Array : public std::enable_shared_from_this<HDF5Array>, public ExternalArray
{
public:
    /*
     * @param desc SciDB Array Description.
     * @param h5desc HDF5 meta data for the array.
     * @param query Query context.
     */
    HDF5Array(const ArrayDesc& desc, HDF5ArrayDesc& h5desc,
              const std::shared_ptr<Query> &query, arena::ArenaPtr arena,
              int64_t chunkAllocation, int64_t chunkAllocParam);

    virtual std::shared_ptr<ConstArrayIterator> getConstIterator(AttributeID attr) const override;

    /**
     * @brief return the HDF5-only metadata.
     */
    const HDF5ArrayDesc& getHDF5ArrayDesc() const { return _h5desc; }

private:
    arena::ArenaPtr _arena;
    HDF5ArrayDesc _h5desc;

};

} //namespace hdf5gateway
} //namespace scidb

#endif //SCIDB_HDF5ARRAY_H
