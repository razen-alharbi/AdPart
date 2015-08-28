#ifndef QUERY_INDEX_H_
#define QUERY_INDEX_H_

#include "query_count.h"
#include "plan.h"
#include "query_index_node.h"
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
static float max_replication_ratio = 1;
static int qft = 2;
typedef unordered_set<query_count*, query_count_hash, query_count_equal> q_count_t;
typedef unordered_set<query_index_node*, distrib_hash, distrib_equal> query_index_t;
class Master;
class Plan;
class Query_Index {
public:
    Query_Index(MasterGUI * master);
	virtual ~Query_Index();
	q_count_t query_counter;
	bool is_parallel_query(Plan * plan);
	bool is_parallel_const(Plan * plan);
	bool is_parallel_var(Plan * plan);
	void check_for_frequency(Plan * plan);
	void update_query_count(query_count * parent, walk_edge * edge, int &min_count, unsigned int& var_count, Plan * plan, bool &subset);
	void add_distributed_query(Plan &plan);
	void update_time_stamps(boost::unordered_set<query_index_node*> &leaves);
	unordered_set<query_index_node*> qi_leaves;
	query_index_t * distrib_queries;

private:
    MasterGUI * master;
	double startup_time;
};

#endif /* QUERY_INDEX_H_ */
