#ifndef CLIENT_H
#define CLIENT_H

#include "common/utils.h"
#include "engine/communicator.h"
#include "common/profiler.h"
#include "engine/datastore.h"
#include "engine/predicate_stat.h"
#include "engine/local_node.h"
#include "engine/plan.h"
#include "engine/statisticsManager.h"
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

class Client{
public:
    Client(string &adhash_home, string &db_folder, string &file_name);
	~Client();
	void start();
	int get_rank();
	Datastore store;
	pred_stat_t predicates_stats;
    Logger logger;
private:
	void wait_for_commands();
	void load_data();
	void resolve_command(net_cmd cmd, vector<string> &params, int src);
	void compute_predicate_stats(vector<string> &params);
	void clean_verts_data();
	void get_triple_load(int src);
	void exchange_verts_stats(vector<string> &params);
	void get_subquery_cardinality(vector<string> &params);
	void join_without_communication(vector<string> &params);
	void join_with_communication(vector<string> &params);
	void distribute_join_column(bool locality_aware);
	void build_candidate_lists(Node &q2, int join_col);
	void distribute_candidate_lists();
	long long finalize_join(Node &q1, Node &q2, vector<pair<int, int> > &join_cols, int prep_locality, int prep_col, int q2_const_subject);
	void solve_star_query(vector<string> &params);
	void start_phd(vector<string> &params);
	void execute_query_in_parallel(vector<string> &params);
	void associate_edges_with_replica_index_nodes(Plan *plan);
	void associate_edges_with_replica_index_nodes_const(Plan *plan);
	void associate_edges_with_replica_index_nodes_var(Plan *plan);
	void store_stats(vector<string> &params);
	void evict(vector<string> &params);
	void get_communication_volume(vector<string> &params);
	//static void *wait_for_transactions(Client * client, int thread_id);
	//void start_an_executer(vector<string> params);
	//void terminate_process();
	//void initialize_threads();
	//void initialize_comm();
    int rank;
	Communicator * communicator;
	Profiler profiler;
	hash_type ht;
	vector<int> tmp_data;
	vector<vector<int> > all_join_cols;
	vector<vector<int> > all_candidate_lists;
	simple_index_t tmp_index[2];
	int current_tmp_index;
    string db_folder;
    string file_name;
    string adhash_home;
};

#endif  /*CLIENT_H*/

