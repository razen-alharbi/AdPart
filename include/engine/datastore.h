#ifndef DATASTORE_H_
#define DATASTORE_H_

#include "common/utils.h"
#include "engine/index.h"
#include "engine/predicate_stat.h"
#include "engine/communicator.h"
#include "common/profiler.h"
#include "engine/local_node.h"
#include "engine/replica_index_node.h"
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
typedef unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal> replica_index_t;
class Datastore {

public:
	Datastore();
	virtual ~Datastore();
	Data_record * insertTriple(int s, int p, int o);
	void index_triple(int predicate, Data_record * rec);
	long long getSize();
	void get_all_predicates(vector<string>& preds);
	void compute_stats(int pred, predicate_stat & stat, Communicator *comm);
	void accurate_stat(int pred, predicate_stat & stat, Communicator *comm);
	void exchange_verts_stats(Communicator *comm);
	long long calc_sub_mass(Node &subquery);
	long long join_two_queries(Node &q1, Node &q2,
			vector<pair<int, int> > &join_cols, vector<int> &q1_data,
			Index * q2_index, int locality, int prep_col,
			vector<vector<int> > &all_jc, int num_workers, int rank,
			simple_index_t *tmp_index, int current_tmp_index, int q2_const_subject);
	long long join_two_queries_star(Node &q1, Node &q2,	vector<pair<int, int> > &join_cols, vector<int> &q1_data, bool join_on_const, string &join_const_val);
	void answer_sub(Node &sub_query, Index *index, vector<int> &result,
			vector<vector<int> > &all_jc, bool prep_jc, int prep_locality,
			int jc_index, int rank, int num_workers, simple_index_t *tmp_index, int current_tmp_index, int q2_const_subject);
	void build_candidate_list(Node &query, int join_col, vector<int> &jc_data,
			vector<int> &result);
	long long finalize_join(Node &q1, Node &q2,
			vector<pair<int, int> > &join_cols, int prep_locality, int prep_col,
			vector<vector<int> > &all_candidate_lists,
			simple_index_t *tmp_index, int current_tmp_index, vector<vector<int> > &all_join_cols,
			vector<int> &result, int num_workers, int rank, int q2_const_subject);
	void hash_distribute_core_edge(replica_index_node * edge, Communicator * comm, long long &replicated_records_count);
	void redistribute_internal_edge(replica_index_node * parent, replica_index_node * edge, Communicator * comm, long long &replicated_records_count);
    long long evaluate_without_communication(vector<Local_Node> &nodes, vector<int> &results, Local_Node &res_node);
	void answer_sub_local(Local_Node &sub_query, Index *index, vector<int> &result);
	void join_two_queries_local(Local_Node & q1, Local_Node & q2, Local_Node &res, vector<int> &q1_data);
	boost::unordered_map<int, int> verts_counter;
	Index * main_index;
	replica_index_t rep_index;
};
#endif /* DATASTORE_H_ */
