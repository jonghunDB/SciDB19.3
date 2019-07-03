//
// Created by jh on 19. 5. 15.
//
/****************************************************************
 *
Simple C++ static KD-Tree implementation with minimal functionality.

points are given as STL vectors (and inserted in their own STL vector) so supports n-dimensional points for any n
makes full trees, (i.e. does not cut-off the branching at some arbitrary level) giving the nearest neighbor query have (strong) logarithmic complexity.
builds the tree in one go (does not support adding nodes, the tree is built from a list of points and cannot be altered afterwards)
points are assumed to be STL vectors
        it provides the following queries:
nearest neighbor
neighbors within a given distance

 ../src/util/RtreeOfSpatialRange.cpp 참고하여 구현

***************************************************************/
#pragma once


#ifndef SCIDB_KDTREE_H
#define SCIDB_KDTREE_H

/*
 * file: KDTree.hpp
 * author: J. Frederico Carvalho
 *
 * This is an adaptation of the KD-tree implementation in rosetta code
 * https://rosettacode.org/wiki/K-d_tree
 * It is a reimplementation of the C code using C++.
 * It also includes a few more queries than the original
 *
 */

#include <boost/function.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

using namespace std;
using point_t = std::vector< double >;
using indexArr = std::vector< size_t >;
using pointIndex = typename std::pair< std::vector< double >, size_t >;

namespace scidb
{

// KDNode
class KDNode {

public:
    using KDNodePtr = std::shared_ptr<KDNode>;
    size_t index;
    point_t x;
    KDNodePtr left;
    KDNodePtr right;

    // initializer
    KDNode();
    KDNode(const point_t &, const size_t &, const KDNodePtr &,
           const KDNodePtr &);
    KDNode(const pointIndex &, const KDNodePtr &, const KDNodePtr &);
    ~KDNode();

    // getter
    double coord(const size_t &);

    // conversions
    // explicit 컴파일러가 자동 형변환을 하는것을 막고 버그를 방지. 직접 형변환을 해주어야 한다.
    explicit operator bool();
    explicit operator point_t();
    explicit operator size_t();
    explicit operator pointIndex();
};

using KDNodePtr = std::shared_ptr< KDNode >;

KDNodePtr NewKDNodePtr();

inline double dist(const point_t &, const point_t &);
inline double dist(const KDNodePtr &, const KDNodePtr &);

// Need for sorting
class comparer {
public:
    size_t idx;
    explicit comparer(size_t idx_);
    inline bool compare_idx(
            const std::pair< std::vector<double>, size_t > &,  //
            const std::pair< std::vector<double>, size_t > &   //
    );
};

using pointIndexArr = typename std::vector< pointIndex >;

inline void sort_on_idx(const pointIndexArr::iterator &,  //
                        const pointIndexArr::iterator &,  //
                        size_t idx);

using pointVec = std::vector< point_t >;

class KDTree {
    KDNodePtr root;
    KDNodePtr leaf;

    KDNodePtr make_tree(const pointIndexArr::iterator &begin,  //
                        const pointIndexArr::iterator &end,    //
                        const size_t &length,                  //
                        const size_t &level                    //
    );

public:
    KDTree() = default;
    explicit KDTree(pointVec point_array);

private:
    KDNodePtr nearest_(           //
            const KDNodePtr &branch,  //
            const point_t &pt,        //
            const size_t &level,      //
            const KDNodePtr &best,    //
            const double &best_dist   //
    );

    // default caller
    KDNodePtr nearest_(const point_t &pt);

public:
    point_t nearest_point(const point_t &pt);
    size_t nearest_index(const point_t &pt);
    pointIndex nearest_pointIndex(const point_t &pt);

private:
    pointIndexArr neighborhood_(  //
            const KDNodePtr &branch,  //
            const point_t &pt,        //
            const double &rad,        //
            const size_t &level       //
    );

public:
    pointIndexArr neighborhood(  //
            const point_t &pt,       //
            const double &rad);

    pointVec neighborhood_points(  //
            const point_t &pt,         //
            const double &rad);

    indexArr neighborhood_indices(  //
            const point_t &pt,          //
            const double &rad);
};


} // namespace scidb
#endif //SCIDB_KDTREE_H

