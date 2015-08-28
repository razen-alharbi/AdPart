#ifndef REPLICA_INDEX_NODE_H
#define REPLICA_INDEX_NODE_H
#include <unordered_set>
#include "engine/walk_edge.h"
#include "common/profiler.h"
#include "engine/index.h"
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
class replica_index_node;
class walk_edge;
struct replica_index_node_hash:std::unary_function<replica_index_node*, std::size_t>
{
	std::size_t operator()(replica_index_node const * x) const;
};
struct replica_index_node_equal:std::binary_function<replica_index_node*, replica_index_node*, bool>
{
	bool operator()(replica_index_node const* x,replica_index_node const* y) const;
};
class replica_index_node {
public:
	string predicate;
	string binding;
	string propagating;
	bool reversed;
	replica_index_node * parent;
	Index * index;
	bool exact;
	unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal> children;
	replica_index_node();
	replica_index_node(string &binding, string &predicate, string &propagating, bool reversed);
	replica_index_node(walk_edge *edge);
	replica_index_node(walk_edge *edge, bool exact);
	string print()const;
	string print2() const;
};
#endif
