#ifndef DISTRIB_QUERY_H
#define DISTRIB_QUERY_H
#include <unordered_set>
#include "engine/walk_edge.h"
#include "common/profiler.h"
//---------------------------------------------------------------------------
// AdHash
// (c) 2015 Razen M. AlHarbi. Web site: http://cloud.kaust.edu.sa/Pages/adhash.aspx
//
// This work is licensed under the Creative Commons
// Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy
// of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300,
// San Francisco, California, 94105, USA.
//---------------------------------------------------------------------------
class walk_edge;
class query_index_node;
struct distrib_hash:std::unary_function<query_index_node*, std::size_t>
{
	std::size_t operator()(query_index_node const * x) const;
};
struct distrib_equal:std::binary_function<query_index_node*, query_index_node*, bool>
{
	bool operator()(query_index_node const* x,query_index_node const* y) const;
};
class query_index_node {
public:
	string predicate;
	string binding;
	string propagating;
	bool reversed;
	query_index_node * parent;
	unordered_set<query_index_node*, distrib_hash, distrib_equal> children;
	int count;
	double ts;
	bool exact;
	query_index_node();
	query_index_node(string &binding, string &predicate, string &propagating, bool reversed);
	query_index_node(walk_edge *edge);
	query_index_node(walk_edge *edge, bool exact);
	string print()const;
	string encode()const;
};
#endif
