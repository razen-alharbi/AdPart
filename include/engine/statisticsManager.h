#ifndef STATISTICS_MANAGER_H_
#define STATISTICS_MANAGER_H_
#include "engine/query.h"
#include "common/profiler.h"
#include "engine/mastergui.h"
#include "engine/predicate_stat.h"
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
typedef boost::unordered_map<string, predicate_stat> pred_stat_t;
class MasterGUI;
class Statistics_Manager {
public:
	Statistics_Manager();
    Statistics_Manager(MasterGUI * master, unsigned nodes);
	virtual ~Statistics_Manager();
	void update_triple_load();
	void add_predicates(vector<string>& preds);
	void analyze_predicates();
	void analyze_predicates_avg();
	void analyze_predicates_elimination();
	int workers;
	pred_stat_t predicates_stats;
	vector<long long> original_triples_load;
	vector<long long> replicated_triples_load;
	vector<long long> total_triples_load;
	float replication_ratio;
	double gini;
	int max_predicate_id;
	int max_vertex_id;
private:
    MasterGUI * master;
};

#endif /* STATISTICS_MANAGER_H_ */
