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
 * @file PerfTime.h
 *
 * @brief timing methods for performance reporting
 */

#ifndef PERF_TIME_H_
#define PERF_TIME_H_

#include <algorithm>
#include <atomic>

#include <sys/time.h>             // linux specific
#include <sys/resource.h>         // linux specific

namespace scidb
{

/**
 * @brief categories of accumulated time
 * the following macro defines a string with all the information required
 * to generate enumerations and a table containing a stringification and their meaning
 */

#define PTC_TABLE_ENTRIES \
    PTC_ENTRY(PTC_ACTIVE,     "query thread scheduled (e.g. excludes when client not requesting data"), \
    /* the following should sum to active */ \
    PTC_ENTRY(PTC_CPU,        "use of cpu excluding below"), \
    PTC_ENTRY(PTC_PG,         "waits for postgres queries"), \
    /* FileIO */ \
    PTC_ENTRY(PTC_FS_RD,      "waits for read"),               /* TBD: split by array, array-meta & non-array */ \
    PTC_ENTRY(PTC_FS_WR,      "waits for write"),              /* TBD: ditto */ \
    PTC_ENTRY(PTC_FS_WR_SYNC, "waits for write with O_SYNC"),  /* TBD: split ditto */ \
    PTC_ENTRY(PTC_FS_FL,      "waits for write flush"),        /* TBD: ditto */ \
    PTC_ENTRY(PTC_FS_PH,      "waits for fallocate punch hole"), \
    PTC_ENTRY(PTC_SM_LOAD,    "waits for chunk load"),              \
    PTC_ENTRY(PTC_BF_RD,      "waits for buffered file read"),      \
    PTC_ENTRY(PTC_SM_CMEM,    "smgr waits for chunk mem (unpins & writes)"), /* PTC_ZERO? */ \
    /* storage */ \
    PTC_ENTRY(PTC_BUF_CACHE,  "waits for space in the buffer cache"), \
    /* networking */ \
    PTC_ENTRY(PTC_REP,        "waits for replication (throttling)"), \
    PTC_ENTRY(PTC_NET_RCV,    "waits for point-to-point receive"),   \
    PTC_ENTRY(PTC_NET_SND,    "waits for point-to-point send"), \
    PTC_ENTRY(PTC_NET_RCV_R,  "waits for remote array)"), \
    PTC_ENTRY(PTC_NET_RCV_C,  "waits for rcv-from-client)"), \
    PTC_ENTRY(PTC_SG_RCV,     "waits for SG chunks receive"),  \
    PTC_ENTRY(PTC_SG_BAR,     "waits for SG sync barrier"), \
    PTC_ENTRY(PTC_BAR,        "waits for other (spmd/bsp) barriers"), \
    /* client communication manager*/                                                     \
    PTC_ENTRY(PTC_CCM_SESS,   "waits for client communication manager session management"), \
    /*other */ \
    PTC_ENTRY(PTC_SORT_JOB,   "waits for sort jobs"), \
    PTC_ENTRY(PTC_EXT,        "waits for managed processes"), \
    PTC_ENTRY(PTC_SEMA,       "waits for other sempahores"),  \
    PTC_ENTRY(PTC_EVENT,      "waits for other events"), \
    PTC_ENTRY(PTC_LATCH,      "waits for data latches/locks"), \
    PTC_ENTRY(PTC_RARE,       "waits that are rare"), \
    PTC_ENTRY(PTC_IGNORE,     "e.g. some waits for query arrival"), /* not attributable to a query */ \
    PTC_ENTRY(PTC_ZERO,       "waits that should equal 0") /*no comma */

/**
 * enumerations for categories of wait time.
 * the more detailed PTW_ enumerations are grouped into these categories.
 */
#define PTC_ENTRY(PTC, MEANING) PTC
enum perfTimeCategory {         // enumerations of wait categories
    PTC_TABLE_ENTRIES,
    PTC_NUM,                    // must follow _TABLE_ENTRIES
    PTC_FIRST=0
};
typedef enum perfTimeCategory perfTimeCategory_t ;
#undef PTC_ENTRY

/**
 * explanation of PTW_ prefixes
 *
 * PTW_SPCL_  waits calculated without using Scoped{WaitTimer,MutexLock}, RWLock, Semaphore, or Event
 * PTW_SWT_   measured using ScopedWaitTime
 * PTW_SML_   measured using ScopedMutexLock
 * PTW_RWL_   measured using ReadWrite
 * PTW_SEM_   measured with Semaphore
 * PTW_EVENT  measured with Event
 * PTW_MUTEX  measured with Mutex
 */

#define PTW_TABLE_ENTRIES \
    PTW_ENTRY(PTW_UNTIMED,             PTC_IGNORE,     "NEVER included in timings"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE,         PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_A,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_B,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_C,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_D,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_E,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_F,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_G,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_H,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_I,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_J,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_ACTIVE_K,       PTC_ACTIVE,     "Query running on a job thread"), \
    PTW_ENTRY(PTW_SPCL_CPU,            PTC_CPU,        "thread on cpu (according to os), other than as below"), \
    PTW_ENTRY(PTW_SWT_PG1,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG2,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG3,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG4,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG5,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG6,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG7,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG8,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG9,             PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG10,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG11,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG12,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG13,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG14,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG15,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG16,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG17,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG18,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG19,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG20,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG21,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG22,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG23,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG24,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG25,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG26,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG26_A,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG27,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG28,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG29,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_A,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_B,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_C,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_D,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_E,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_F,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_G,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_H,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_I,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG30_J,          PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG31,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG32,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG33,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG34,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG35,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG36,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG37,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SWT_PG38,            PTC_PG,         "communicating with postgres"), \
    PTW_ENTRY(PTW_SML_PG,              PTC_PG,         "latches during communication with postgres s.b. 0"), \
    /* File IO */ \
    PTW_ENTRY(PTW_SWT_FS_RD,           PTC_FS_RD,      "file system read"), \
    PTW_ENTRY(PTW_SWT_FS_WR,           PTC_FS_WR,      "file system write"), \
    PTW_ENTRY(PTW_SWT_FS_WR_SYNC,      PTC_FS_WR_SYNC, "file system write with O_SYNC"), \
    PTW_ENTRY(PTW_SWT_FS_FL,           PTC_FS_FL,      "file system flush"), \
    PTW_ENTRY(PTW_SWT_FS_PH,           PTC_FS_PH,      "file system fallocate punch hole"), \
    PTW_ENTRY(PTW_EVENT_SM_LOAD,       PTC_SM_LOAD,    "waiting on storage manager thread to load chunk"), \
    PTW_ENTRY(PTW_EVENT_SM_CMEM,       PTC_SM_CMEM,    "waiting for freed memory (unpin & write)"), \
    PTW_ENTRY(PTW_EVENT_BFI_GET,       PTC_BF_RD,      "wait for BufferedFileInput data (reader thread)"), \
    PTW_ENTRY(PTW_EVENT_BFI_RA,        PTC_BF_RD,      "wait for BufferedFileINput read ahead"), \
    /* Storage */ \
    PTW_ENTRY(PTW_SWT_BUF_CACHE,       PTC_BUF_CACHE,  "wait for space in the buffer cache"), \
    /* query thread waiting on other threads */ \
    PTW_ENTRY(PTW_EVENT_SORT_JOB,      PTC_SORT_JOB,   "waiting on sorting thread"), \
    /* networking */ \
    PTW_ENTRY(PTW_SWT_NET_RCV,         PTC_NET_RCV,    "waiting to receive pt-to-pt message"), \
    PTW_ENTRY(PTW_SWT_NET_SND,         PTC_NET_SND,    "time to enqueue pt-to-pt message for send"), \
    PTW_ENTRY(PTW_SEM_NET_RCV_R,       PTC_NET_RCV_R,  "wait for remote array"),  \
    PTW_ENTRY(PTW_SEM_NET_RCV_C,       PTC_NET_RCV_C,  "wait for client communication"),  \
    /* Client Communication Manager */                                                       \
    PTW_ENTRY(PTW_CCM_CACHE,           PTC_CCM_SESS,   "wait for Ccm session cache"), \
    /* SG */ \
    PTW_ENTRY(PTW_SWT_SG_RCV,          PTC_SG_RCV,     "wait for SG receive data"), \
    PTW_ENTRY(PTW_SEM_SG_RCV,          PTC_SG_RCV,     "wait for SG receive data sema"), \
    PTW_ENTRY(PTW_EVENT_SG_PULL,       PTC_SG_RCV,     "wait for SG receive attribute event"), \
    PTW_ENTRY(PTW_SEM_BAR_SG,          PTC_SG_BAR,     "wait for SG sync barrier"), \
    /* PTC_REP */ \
    PTW_ENTRY(PTW_SEM_REP,             PTC_REP,        "wait for replication sema"), /* ~6 locs */ \
    PTW_ENTRY(PTW_EVENT_REP,           PTC_REP,        "wait for replication event"), /* 1 loc */ \
    /* PTC_EXT */ \
    PTW_ENTRY(PTW_EVENT_EXT_LAUNCH,    PTC_EXT,        "wait for managed process to start"), \
    PTW_ENTRY(PTW_SWT_EXT,             PTC_EXT,        "wait for managed process to 'return' status"), \
    /* misc. barriers */ \
    PTW_ENTRY(PTW_SML_BAR_DEFAULT,     PTC_BAR,        "wait for unspecified barrier"), \
    PTW_ENTRY(PTW_SEM_JOB_DONE,        PTC_BAR,        "wait at end of job"), \
    PTW_ENTRY(PTW_SEM_RESULTS_QP,      PTC_BAR,        "QueryProcessor results.enter()"), /* 2 locs */ \
    PTW_ENTRY(PTW_SEM_RESULTS_EX,      PTC_BAR,        "executor results.enter()"), /* 1 loc */ \
    /* misc semaphores */ \
    PTW_ENTRY(PTW_SEM_THREAD_TERM,     PTC_SEMA,       "wait for thread term"), \
    PTW_ENTRY(PTW_SEM_RESOURCES,       PTC_SEMA,       "wait for a Resources semaphore" ), \
    /* short-term data locks */ \
    PTW_ENTRY(PTW_MUTEX,               PTC_LATCH,      "wait for a non-specific Mutex"), /* (default parameter) */ \
    PTW_ENTRY(PTW_RWL_MODULE,          PTC_LATCH,      "wait for a module RWLock"), \
    PTW_ENTRY(PTW_SML_ARENA,           PTC_LATCH,      "wait for ThreadedArena latch"), /* class */ \
    PTW_ENTRY(PTW_SML_BFI,             PTC_LATCH,      "wait for a BufferedFileInput latch"), /* class */ \
    PTW_ENTRY(PTW_SML_CON,             PTC_LATCH,      "wait for a Connection latch"), /* class */ \
    PTW_ENTRY(PTW_SML_DFLT,            PTC_LATCH,      "wait for unspecified ScopedMutexLock"), /* ~72 remaining */ \
    PTW_ENTRY(PTW_SML_DS,              PTC_LATCH,      "wait for a Datastore latch"), /* class */ \
    PTW_ENTRY(PTW_SML_JOB_XOQ,         PTC_LATCH,      "wait for a Job latch"), \
    PTW_ENTRY(PTW_SML_MA,              PTC_LATCH,      "wait for a MemArray latch"), /* class */ \
    PTW_ENTRY(PTW_SML_NOTI,            PTC_LATCH,      "wait for a Notification latch"), /* class */ \
    PTW_ENTRY(PTW_SML_NM,              PTC_LATCH,      "wait for a NetworkManager latch"), /* class */ \
    PTW_ENTRY(PTW_SML_MPIL,            PTC_LATCH,      "wait for an MPILauncher latch"), /* class */ \
    PTW_ENTRY(PTW_SML_MPIM,            PTC_LATCH,      "wait for an MPIManager latch"), /* class */ \
    PTW_ENTRY(PTW_SML_OPERATOR,        PTC_LATCH,      "wait for an Operator latch"), /* 1 loc */ \
    PTW_ENTRY(PTW_SML_PM,              PTC_LATCH,      "wait for an PluginManager latch"), /* class */ \
    PTW_ENTRY(PTW_SML_QUANTILE,        PTC_LATCH,      "wait for a Quantile latch"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_A,   PTC_LATCH,      "wait for Query errorMutex a"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_B,   PTC_LATCH,      "wait for Query errorMutex b"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_C,   PTC_LATCH,      "wait for Query errorMutex c"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_D,   PTC_LATCH,      "wait for Query errorMutex d"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_E,   PTC_LATCH,      "wait for Query errorMutex e"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_F,   PTC_LATCH,      "wait for Query errorMutex f"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_G,   PTC_LATCH,      "wait for Query errorMutex g"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_H,   PTC_LATCH,      "wait for Query errorMutex h"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_I,   PTC_LATCH,      "wait for Query errorMutex i"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_J,   PTC_LATCH,      "wait for Query errorMutex j"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_K,   PTC_LATCH,      "wait for Query errorMutex k"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_L,   PTC_LATCH,      "wait for Query errorMutex l"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_M,   PTC_LATCH,      "wait for Query errorMutex m"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_N,   PTC_LATCH,      "wait for Query errorMutex n"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_O,   PTC_LATCH,      "wait for Query errorMutex o"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_P,   PTC_LATCH,      "wait for Query errorMutex p"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_Q,   PTC_LATCH,      "wait for Query errorMutex q"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_R,   PTC_LATCH,      "wait for Query errorMutex r"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_S,   PTC_LATCH,      "wait for Query errorMutex s"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_T,   PTC_LATCH,      "wait for Query errorMutex t"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_U,   PTC_LATCH,      "wait for Query errorMutex u"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_V,   PTC_LATCH,      "wait for Query errorMutex v"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_W,   PTC_LATCH,      "wait for Query errorMutex w"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_X,   PTC_LATCH,      "wait for Query errorMutex x"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_Y,   PTC_LATCH,      "wait for Query errorMutex y"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_Z,   PTC_LATCH,      "wait for Query errorMutex z"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AA,  PTC_LATCH,      "wait for Query errorMutex aa"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AB,  PTC_LATCH,      "wait for Query errorMutex ab"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AC,  PTC_LATCH,      "wait for Query errorMutex ac"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AD,  PTC_LATCH,      "wait for Query errorMutex ad"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AE,  PTC_LATCH,      "wait for Query errorMutex ae"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AF,  PTC_LATCH,      "wait for Query errorMutex af"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AG,  PTC_LATCH,      "wait for Query errorMutex ag"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AH,  PTC_LATCH,      "wait for Query errorMutex ah"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AI,  PTC_LATCH,      "wait for Query errorMutex ai"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AJ,  PTC_LATCH,      "wait for Query errorMutex aj"), \
    PTW_ENTRY(PTW_SML_QUERY_ERROR_AK,  PTC_LATCH,      "wait for Query errorMutex ak"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_A,   PTC_LATCH,      "wait for Query _mutex a"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_B,   PTC_LATCH,      "wait for Query _mutex b"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_C,   PTC_LATCH,      "wait for Query _mutex c"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_D,   PTC_LATCH,      "wait for Query _mutex d"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_E,   PTC_LATCH,      "wait for Query _mutex e"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_F,   PTC_LATCH,      "wait for Query _mutex f"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_G,   PTC_LATCH,      "wait for Query _mutex g"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_H,   PTC_LATCH,      "wait for Query _mutex h"), \
    PTW_ENTRY(PTW_SML_QUERY_MUTEX_I,   PTC_LATCH,      "wait for Query _mutex i"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_A, PTC_LATCH,      "wait for Query queriesMutex a"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_B, PTC_LATCH,      "wait for Query queriesMutex b"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_C, PTC_LATCH,      "wait for Query queriesMutex c"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_D, PTC_LATCH,      "wait for Query queriesMutex d"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_E, PTC_LATCH,      "wait for Query queriesMutex e"), \
    PTW_ENTRY(PTW_SML_QUERY_QUERIES_F, PTC_LATCH,      "wait for Query queriesMutex f"), \
    PTW_ENTRY(PTW_SML_QUERY_WARN_A,    PTC_LATCH,      "wait for Query _warningsMutex a"), \
    PTW_ENTRY(PTW_SML_QUERY_WARN_B,    PTC_LATCH,      "wait for Query _warningsMutex b"), \
    PTW_ENTRY(PTW_SML_QUERY_WARN_C,    PTC_LATCH,      "wait for Query _warningsMutex c"), \
    PTW_ENTRY(PTW_SML_RA,              PTC_LATCH,      "wait for a RemoteArray latch"), /* class */ \
    PTW_ENTRY(PTW_SML_REP,             PTC_LATCH,      "wait for a Replication latch"), \
    PTW_ENTRY(PTW_SML_RWL,             PTC_LATCH,      "wait for a RWLock latch"), /* needs to be replaced with the caller's id */ \
    PTW_ENTRY(PTW_SML_SA,              PTC_LATCH,      "wait for a SortArray latch"), /* class */ \
    PTW_ENTRY(PTW_SML_SG_PULL,         PTC_LATCH,      "wait for SG PULL latch"), /*  */ \
    PTW_ENTRY(PTW_SML_SR,              PTC_LATCH,      "wait for SciDBRemote latch"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_A,          PTC_LATCH,      "wait for a Storage latch a"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_B,          PTC_LATCH,      "wait for a Storage latch b"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_C,          PTC_LATCH,      "wait for a Storage latch c"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_D,          PTC_LATCH,      "wait for a Storage latch d"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_E,          PTC_LATCH,      "wait for a Storage latch e"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_F,          PTC_LATCH,      "wait for a Storage latch f"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_G,          PTC_LATCH,      "wait for a Storage latch g"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_H,          PTC_LATCH,      "wait for a Storage latch h"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_I,          PTC_LATCH,      "wait for a Storage latch i"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_J,          PTC_LATCH,      "wait for a Storage latch j"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_K,          PTC_LATCH,      "wait for a Storage latch k"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_L,          PTC_LATCH,      "wait for a Storage latch l"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_M,          PTC_LATCH,      "wait for a Storage latch m"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_N,          PTC_LATCH,      "wait for a Storage latch n"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_O,          PTC_LATCH,      "wait for a Storage latch o"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_P,          PTC_LATCH,      "wait for a Storage latch p"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_Q,          PTC_LATCH,      "wait for a Storage latch q"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_R,          PTC_LATCH,      "wait for a Storage latch r"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_S,          PTC_LATCH,      "wait for a Storage latch s"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_T,          PTC_LATCH,      "wait for a Storage latch t"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_U,          PTC_LATCH,      "wait for a Storage latch u"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_V,          PTC_LATCH,      "wait for a Storage latch v"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AA,         PTC_LATCH,      "wait for a Storage latch aa"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AB,         PTC_LATCH,      "wait for a Storage latch ab"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AC,         PTC_LATCH,      "wait for a Storage latch ac"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AD,         PTC_LATCH,      "wait for a Storage latch ad"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AE,         PTC_LATCH,      "wait for a Storage latch ae"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AF,         PTC_LATCH,      "wait for a Storage latch af"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AG,         PTC_LATCH,      "wait for a Storage latch ag"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AH,         PTC_LATCH,      "wait for a Storage latch ah"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AI,         PTC_LATCH,      "wait for a Storage latch ai"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AJ,         PTC_LATCH,      "wait for a Storage latch aj"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AK,         PTC_LATCH,      "wait for a Storage latch ak"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AL,         PTC_LATCH,      "wait for a Storage latch al"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AM,         PTC_LATCH,      "wait for a Storage latch am"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AN,         PTC_LATCH,      "wait for a Storage latch an"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AO,         PTC_LATCH,      "wait for a Storage latch ao"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AP,         PTC_LATCH,      "wait for a Storage latch ap"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AQ,         PTC_LATCH,      "wait for a Storage latch aq"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AR,         PTC_LATCH,      "wait for a Storage latch ar"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AS,         PTC_LATCH,      "wait for a Storage latch as"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AT,         PTC_LATCH,      "wait for a Storage latch at"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AU,         PTC_LATCH,      "wait for a Storage latch au"), /* class */ \
    PTW_ENTRY(PTW_SML_STOR_AV,         PTC_LATCH,      "wait for a Storage latch av"), /* class */ \
    PTW_ENTRY(PTW_SML_TS,              PTC_LATCH,      "wait for a TypeSystem latch"), /* class */ \
    PTW_ENTRY(PTW_SML_WQ,              PTC_LATCH,      "wait for a WorkQueue latch"), /* class */ \
    PTW_ENTRY(PTW_SML_RESOURCES,       PTC_LATCH,      "wait for a Resources latch"), /* class */ \
    /* PTC_RARE */ \
    PTW_ENTRY(PTW_EVENT_MATCH,         PTC_RARE,	   "wait in example operator Match" ), \
    PTW_ENTRY(PTW_EVENT_BESTMATCH,     PTC_RARE,	   "wait in example operator BestMatch" ),  \
    /* PTC_ZERO, for these the explanation is can be used as a warning if they occur */ \
    PTW_ENTRY(PTW_SEM_NET_RCV,         PTC_ZERO,       "time accrues to outer scope PTW_SWT_NET_RCV"), \
    PTW_ENTRY(PTW_SEM_BAR_DEFAULT,     PTC_ZERO,       "time accrues to outer scope PTW_SML_BAR_DEFAULT") /*no comma*/

/**
 * enumerations of finer-grained 'sub-categories' of wait time.
 * eventually someday replaced by registered file and line number locations in the code
 * these are grouped into PTC_ categories for coarse-grained reporting
 * their direct use is intended for debugging
 */
#define PTW_ENTRY(PTW, PTC, MEANING) PTW
enum perfTimeWait {
    PTW_TABLE_ENTRIES,
    PTW_NUM,           // must follow _TABLE_ENTRIES
    PTW_FIRST=0
};
#undef PTW_ENTRY

typedef enum perfTimeWait perfTimeWait_t ;

/**
 * low-level call for convenience layers of timing code
 *
 * @note:  should never throw.  failure to time
 *         should not change query execution
 */
double perfTimeGetElapsed();  // no exceptions please


/**
 * another low-level call for convenience layers of timing code
 *
 * @note:  should never throw.  failure to time
 *         should not change query execution
 */
double perfTimeGetCPU();  // no exceptions please


/**
 * Get the 'Elapsed' (wall-clock) time in microseconds
 *
 * @note:  should never throw.  failure to time
 *         should not change query execution
 */
uint64_t perfTimeGetElapsedInMicroseconds();  // no exceptions please

/**
 * Get combined system and user time usage measures in microseconds
 * @param who - one of the following {RUSAGE_SELF, RUSAGE_THREAD, RUSAGE_CHILDREN}
 * See man rusage for more information
 *
 * @note:  should never throw.  failure to time
 *         should not change query execution
*/
uint64_t perfTimeGetCpuInMicroseconds(int who);  // no exceptions please

/**
 * how time used is reported on a per-wait basis
 *
 * @note:  should never throw.  failure to time
 *         should not change query execution
 */
void perfTimeAdd(const perfTimeWait_t tw, const double sec, uint64_t nestingDepth);  // no exceptions please


/**
 * struct that packages up parameters that control a ScopedWaitTimer
 */
struct WaitTimerParams {

    /**
     * @member?  TODO
     */
    perfTimeWait_t  tw;

    /**
     * @member?  TODO
     */
    uint64_t        sampleInterval;

    /**
     * @member?  TODO
     */
    uint64_t*       unsampledCount;

    /**
     * @parm tw - a PTW_ enum relating to the code location
     *
     * @parm sampleInterval - a multiplier of the measured time, used by
     *            performance-sensitive timing locations that
     *            cannot afford to time all calls, some some are time
     *            and the time inflated to compensate.
     *
     * @parm unsampledCount - reference to a call-specific
     *           static int or
     *           static thread_local int
     *
     * example: if can afford only to time 1/100 passes,
     *          set sampleInterval to 100.  That 1-in-100 timing
     *          will be multiplied by weight=100 when recorded.
     */
    WaitTimerParams(perfTimeWait_t tw_, uint64_t sampleInterval_, uint64_t* unsampledCount_=NULL);
};

/**
 * class that times from constuction to destruction
 */
class ScopedWaitTimer {
public:
    /**
     * @parm wti - a WaitTimerInfo controlling how time is measured
     *             see WaitTimerInfo for details
     *
     * @note:  should never throw.  failure to time
     *         should not change query execution
     */
    ScopedWaitTimer(const WaitTimerParams& wtp, bool logOnCompletion = false); // no exceptions please

    /**
     * @parm tw - see WaitTimerParams
     *
     * @brief - convenience ctor. most constructions
     *          do not involve local undersampling control
     *          and can avoid creating a WaitTimerParams
     *          by  using this ctor.
     *
     * @note:  should never throw.  failure to time
     *         should not change query execution
     */
    ScopedWaitTimer(perfTimeWait_t tw, bool logOnCompletion = false); // no exceptions please

    /**
     *
     * @note:  should never throw.  failure to time
     *         should not change query execution
     */
    ~ScopedWaitTimer();                                    // no exceptions please

    /**
     * @note:  called from runSciDB else
     *         infinite recursion (through mutex) can occur.
     */
    static void          adjustWaitTimingEnabled();
private:
    void                 init();
    static bool          isWaitTimingEnabled();

    WaitTimerParams      _wtp;
    double               _secStartElapsed;
    double               _secStartCPU;
    bool                 _isEnabled;
    static bool          _isWaitTimingEnabled;
    bool                 _logOnCompletion;

    static thread_local  uint64_t _nestingDepth;  // std::atomic_unit_fast32_t
                                                  // if it were not thread_local
};


/**
 * @parm tw - a PTW_ enum relating to the code location
 * @returns the PTC_ enum for the category that the PTW_ is in
 *
 */
perfTimeCategory_t twToCategory(const perfTimeWait_t tw);

/**
 * @parm tw - a PTW_ enum relating to the code location
 * @returns a char* string representation of the enumeration
 */
const char* twName(const perfTimeWait_t tw);
/**
 * @parm tc - a PTC_ enum
 * @returns a char* string representation of the enumeration
 *
 */
const char* tcName(const perfTimeCategory_t tc);

} //namespace

#endif /* PERF_TIME_H_ */
