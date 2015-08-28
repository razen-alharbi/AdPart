#ifndef QUERY_COUNT_H
#define QUERY_COUNT_H

#include "engine/walk_edge.h"
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
class query_count;
struct query_count_hash:std::unary_function<query_count*, std::size_t>
{
	std::size_t operator()(query_count const * x) const;
};
struct query_count_equal:std::binary_function<query_count*, query_count*, bool>
{
	bool operator()(query_count const* e1,	query_count const* e2) const;
};
class query_count {
public:
	string predicate;
	bool reversed;
	query_count * parent;
	unordered_set<query_count*, query_count_hash, query_count_equal> children;
	boost::unordered_map<string, int> binding_count;
	boost::unordered_map<string, int> propagating_count;
	int count;

	query_count();
	query_count(string &_predicate, bool _reversed);
	query_count(walk_edge * edge);
	void update_proactivity_info(walk_edge * edge, unsigned int& var_count, map<string, float> &walk_scores);
	string print()const;
};
#endif
