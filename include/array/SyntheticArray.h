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

#ifndef SYNTHETIC_ARRAY_H_
#define SYNTHETIC_ARRAY_H_

#include <array/ArrayDesc.h>
#include <array/ConstArrayIterator.h>
#include <array/ConstChunk.h>
#include <query/Value.h>

namespace scidb {

class Array;
class Query;
class SyntheticChunk;

class SyntheticChunkIterator : public ConstChunkIterator
{
public:
    SyntheticChunkIterator(std::shared_ptr<Array> parray,
                           const AttributeDesc& attr,
                           const SyntheticChunk& chunk,
                           int iterationMode,
                           std::shared_ptr<ConstChunkIterator> ebmChunkIter);
    virtual ~SyntheticChunkIterator();

    bool end() override;
    void operator ++() override;
    Coordinates const& getPosition() override;
    bool setPosition(Coordinates const& pos) override;
    void restart() override;

    int getMode() const override;
    Value const& getItem() override;
    bool isEmpty() const override;
    ConstChunk const& getChunk() override;

private:
    std::shared_ptr<Array> _parray;
    AttributeDesc _attr;
    const SyntheticChunk& _chunk;
    int _iterationMode;
    Coordinates _pos;
    std::shared_ptr<ConstChunkIterator> _ebmChunkIter;
    Value _tileValue;
};

class SyntheticChunk : public ConstChunk
{
public:
    SyntheticChunk(std::shared_ptr<Array> parray,
                   const AttributeDesc& attr);
    virtual ~SyntheticChunk();

    const ArrayDesc& getArrayDesc() const override;
    const AttributeDesc& getAttributeDesc() const override;
    Coordinates const& getFirstPosition(bool withOverlap) const override;
    Coordinates const& getLastPosition(bool withOverlap) const override;
    std::shared_ptr<ConstChunkIterator> getConstIterator(int iterationMode) const override;
    CompressorType getCompressionMethod() const override;
    Array const& getArray() const override;

    void setEbmChunk(const ConstChunk* pebmChunk);

private:
    std::shared_ptr<Array> _parray;
    AttributeDesc _attr;
    const ConstChunk* _ebmChunk;
};

class SyntheticIterator : public ConstArrayIterator
{
public:
    SyntheticIterator(std::shared_ptr<Array> parray,
                      const AttributeDesc& attr,
                      std::shared_ptr<Query> query);
    virtual ~SyntheticIterator();

    ConstChunk const& getChunk() override;
    bool end() override;
    void operator++() override;
    Coordinates const& getPosition() override;
    bool setPosition(Coordinates const& pos) override;
    void restart() override;

private:
    std::shared_ptr<Array> _parray;
    SyntheticChunk _chunk;
    std::shared_ptr<ConstArrayIterator> _ebmIter;
};

}  // namespace scidb

#endif  // SYNTHETIC_ARRAY_H_
