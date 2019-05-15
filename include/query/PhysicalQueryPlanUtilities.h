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
 * @file QueryPlanUtilities.h
 *
 * @brief Plan helper functions
 *
 * @author Oleg Tsarev <oleg@oleg.sh>
 */


#ifndef QUERYPLANUTILITIES_H_
#define QUERYPLANUTILITIES_H_

#include <iostream>
#include <memory>
#include <log4cxx/logger.h>

namespace scidb
{

class IndentBy
{
public:
    const int count;
    const char by;

public:
    IndentBy(int _count, char _by) : count(_count), by(_by)
    {
    }
};

class Indent
{
public:
    const int count;

public:
    Indent(int _count) : count(_count)
    {
    }

    IndentBy operator()(char by, bool extra = true) const
    {
        return IndentBy(count + (extra ? 1 : 0), by);
    }
};

inline std::ostream& operator<<(std::ostream& str, IndentBy const& indent)
{
    std::string pad(indent.count, indent.by);
    str << pad;
    return str;
}

class PhysicalQueryPlanNode;
typedef std::shared_ptr<PhysicalQueryPlanNode> PhysNodePtr;

const PhysNodePtr getRoot(PhysNodePtr node);

void printPlan(PhysNodePtr, int indent = 0, bool children = true);
void logPlanDebug(log4cxx::LoggerPtr, PhysNodePtr, int indent = 0, bool children = true);
void logPlanTrace(log4cxx::LoggerPtr, PhysNodePtr, int indent = 0, bool children = true);

} // namespace

#endif
