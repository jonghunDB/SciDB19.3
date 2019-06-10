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

/**
 * @file DAFilterArray.h
 *
 * @brief The implementation of the array iterator for the DAFilter operator
 *
 *
 */

/*
 *  ArrayBridge의 코드 구조를 파악한 뒤에 이를 이용해서 코드를 구현해보자.
 *
 *  ExternalArray
 *    ㄴ  HDF5Array
 *        HDF5ArrayDesc
 *
 *  physicalScanHDF5
 *  LogicalScanHDF5
 *
 *
 * */

/* DAFilterArray
 *  ㄴ DAFilterArrayIterator
 *     ㅣ DAFilterChunkIterator
 *     ㅣ ExistedBitmapChunkIterator
 *     ㄴ NewBitmapChunkIterator
 *
 * */

/*
 *  1. ArrayIterator은 그대로 사용, ChunkIterator을 key value로 저장, 로드하는 함수 구현
 *  2.
 *
 * */

#ifndef DAFilter_ARRAY_H_
#define DAFilter_ARRAY_H_

#include "array/Array.h"
#include <string>
#include <vector>
#include "array/DelegateArray.h"
#include "query/LogicalExpression.h"
#include "query/Expression.h"
#include "util/CoordinatesMapper.h"

namespace scidb
{


using namespace std;

class DAFilterArray;
class DAFilterArrayIterator;
class DAFilterChunkIterator;
class DAFilterValueMap;

class DAFilterValueMap
{
public:
    CoordinatesMapper _coordinateMapper();
private:

};


class DAFilterChunkIterator : public DelegateChunkIterator
{
  protected:
    Value& evaluate();
    Value& buildBitmap();
    // dense한 부분에 대해 bitmap을 만든다.
    Value& buildDenseBitmap();
    bool DAFilter();
    void moveNext();
    void nextVisible();

  public:
    //Payload에서 RLE데이터에 대해 한 아이템을 가져온다.
    Value const& getItem() override;
    Value const& getDAItem();
    void operator ++() override;
    void restart() override;
    bool end() override;
    bool setPosition(Coordinates const& pos) override;
    virtual std::shared_ptr<Query> getQuery() { return _query; }
    DAFilterChunkIterator(DAFilterArrayIterator const& arrayIterator, DelegateChunk const* chunk, int iterationMode);

  protected:
    MemChunk _shapeChunk;
    DAFilterArray const& _array;
    std::vector< std::shared_ptr<ConstChunkIterator> > _iterators;
    std::shared_ptr<ConstChunkIterator> _emptyBitmapIterator;
    ExpressionContext _params;
    bool _hasCurrent;
    int _mode;
    Value _tileValue;
    TypeId _type;
 private:
    std::shared_ptr<Query> _query;
};


class ExistedBitmapChunkIterator : public DAFilterChunkIterator
{
public:
    Value& getItem() override;
    ExistedBitmapChunkIterator(DAFilterArrayIterator const& arrayIterator, DelegateChunk const* chunk, int iterationMode);

private:
     Value _value;
};


class NewBitmapChunkIterator : public DAFilterChunkIterator
{
public:
    Value& getItem() override;

    NewBitmapChunkIterator(DAFilterArrayIterator const& arrayIterator, DelegateChunk const* chunk, int iterationMode);
};


class DAFilterArrayIterator : public DelegateArrayIterator
{
    friend class DAFilterChunkIterator;
  public:
    bool setPosition(Coordinates const& pos) override;
    void restart() override;
    void operator ++() override;
    ConstChunk const& getChunk() override;
    DAFilterArrayIterator(DAFilterArray const& array,
                        const AttributeDesc& attrID,
                        const AttributeDesc& inputAttrID);

  private:
    std::vector< std::shared_ptr<ConstArrayIterator> > iterators;
    std::shared_ptr<ConstArrayIterator> emptyBitmapIterator;
    AttributeID inputAttrID;
};

class DAFilterArray : public DelegateArray
{
    friend class DAFilterArrayIterator;
    friend class DAFilterChunkIterator;
    friend class NewBitmapChunkIterator;
    friend class DAFilterValueMap;

  public:
    virtual DelegateChunk* createChunk(DelegateArrayIterator const* iterator, AttributeID id) const;
    virtual DelegateChunkIterator* createChunkIterator(DelegateChunk const* chunk, int iterationMode) const;
    DelegateArrayIterator* createArrayIterator(const AttributeDesc& id) const override;

    DAFilterArray(ArrayDesc const& desc, std::shared_ptr<Array> const& array,
                std::shared_ptr< Expression> expr, std::shared_ptr<Query>& query,
                bool tileMode);

  private:
    std::map<Coordinates, std::shared_ptr<DelegateChunk>, CoordinatesLess > cache;
    Mutex mutex;
    std::shared_ptr<Expression> expression;
    std::vector<BindInfo> bindings;
    bool _tileMode;
    size_t cacheSize;
    AttributeID emptyAttrID;

};

}

#endif
