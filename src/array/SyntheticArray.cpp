/*
**
* BEGIN_COPYRIGHT
*
* Copyright (C) 2008-2019 SciDB, Inc.
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

#include <array/Array.h>
#include <array/SyntheticArray.h>
#include <query/Query.h>
#include <query/TypeSystem.h>

namespace scidb {

SyntheticChunkIterator::SyntheticChunkIterator(std::shared_ptr<Array> parray,
                                               const AttributeDesc& attr,
                                               const SyntheticChunk& chunk,
                                               int iterationMode,
                                               std::shared_ptr<ConstChunkIterator> ebmChunkIter)
        : _parray(parray)
        , _attr(attr)
        , _chunk(chunk)
        , _iterationMode(iterationMode)
        , _pos(_parray->getArrayDesc().getDimensions().size())
        , _ebmChunkIter(ebmChunkIter)
        , _tileValue(makeTileConstant(_attr.getType(),
                                      _attr.getDefaultValue()))
{
    restart();
}

SyntheticChunkIterator::~SyntheticChunkIterator()
{
}

bool SyntheticChunkIterator::end()
{
    return _ebmChunkIter->end();
}

void SyntheticChunkIterator::operator ++()
{
    ++(*_ebmChunkIter);
}

Coordinates const& SyntheticChunkIterator::getPosition()
{
    return _ebmChunkIter->getPosition();
}

bool SyntheticChunkIterator::setPosition(Coordinates const& pos)
{
    return _ebmChunkIter->setPosition(pos);
}

void SyntheticChunkIterator::restart()
{
    _ebmChunkIter->restart();
}

int SyntheticChunkIterator::getMode() const
{
    return _iterationMode;
}

Value const& SyntheticChunkIterator::getItem()
{
    if (_iterationMode & TILE_MODE) {
        return _tileValue;
    }
    else {
        return _attr.getDefaultValue();
    }
}

bool SyntheticChunkIterator::isEmpty() const
{
    return false;  // TODO consider iteration mode?
}

ConstChunk const& SyntheticChunkIterator::getChunk()
{
    return _chunk;
}

SyntheticChunk::SyntheticChunk(std::shared_ptr<Array> parray,
                               const AttributeDesc& attr)
        : _parray(parray)
        , _attr(attr)
{
}

SyntheticChunk::~SyntheticChunk()
{
}

const ArrayDesc& SyntheticChunk::getArrayDesc() const
{
    return _parray->getArrayDesc();
}

const AttributeDesc& SyntheticChunk::getAttributeDesc() const
{
    return _attr;
}

Coordinates const& SyntheticChunk::getFirstPosition(bool withOverlap) const
{
    SCIDB_ASSERT(_ebmChunk);
    return _ebmChunk->getFirstPosition(withOverlap);
}

Coordinates const& SyntheticChunk::getLastPosition(bool withOverlap) const
{
    SCIDB_ASSERT(_ebmChunk);
    return _ebmChunk->getLastPosition(withOverlap);
}

std::shared_ptr<ConstChunkIterator> SyntheticChunk::getConstIterator(int iterationMode) const
{
    auto ebmChunkIter = _ebmChunk->getConstIterator(iterationMode);
    return std::make_shared<SyntheticChunkIterator>(_parray,
                                                    _attr,
                                                    *this,
                                                    iterationMode,
                                                    ebmChunkIter);
}

CompressorType SyntheticChunk::getCompressionMethod() const
{
    return _attr.getDefaultCompressionMethod();
}

Array const& SyntheticChunk::getArray() const
{
    return *_parray;
}

void SyntheticChunk::setEbmChunk(const ConstChunk* pebmChunk)
{
    _ebmChunk = pebmChunk;
}

SyntheticIterator::SyntheticIterator(std::shared_ptr<Array> parray,
                                     const AttributeDesc& attr,
                                     std::shared_ptr<Query> query)
        : _parray(parray)
        , _chunk(parray, attr)
        , _ebmIter(nullptr)
{
    restart();
}

SyntheticIterator::~SyntheticIterator()
{
}

ConstChunk const& SyntheticIterator::getChunk()
{
    const auto& ebmChunk = _ebmIter->getChunk();
    const auto* pebmChunk = &ebmChunk;
    _chunk.setEbmChunk(pebmChunk);
    return _chunk;
}

bool SyntheticIterator::end()
{
    return !_ebmIter || _ebmIter->end();
}

void SyntheticIterator::operator++()
{
    SCIDB_ASSERT(_ebmIter);
    ++(*_ebmIter);
}

Coordinates const& SyntheticIterator::getPosition()
{
    SCIDB_ASSERT(_ebmIter);
    return _ebmIter->getPosition();
}

bool SyntheticIterator::setPosition(Coordinates const& pos)
{
    SCIDB_ASSERT(_ebmIter);
    bool success = false;
    success = _ebmIter->setPosition(pos);
    return success;
}

void SyntheticIterator::restart()
{
    const auto ebmAttr = _parray->getArrayDesc().getEmptyBitmapAttribute();
    if (ebmAttr) {
        _ebmIter = _parray->getConstIterator(*ebmAttr);
    }
}

}  // namespace scidb
