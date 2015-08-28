#ifndef PLANNER_HP
#define	 PLANNER_HP

#include "engine/query.h"
#include "engine/mastergui.h"
#include "engine/local_node.h"
#include "engine/query_index.h"
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
class MasterGUI;
class query_index_node;
struct dynamic_state{
	float cost;
	float communication;
	float cardinality;
	float cum_cardinality;
	vector<int> ordered_nodes;
	boost::unordered_map<string, float> variables_bindings;
	Node res;

	dynamic_state(){
		cost = 0;
		communication = 0;
		cardinality = 0;
		cum_cardinality = 0;
	}
	dynamic_state(vector<string> &variables){
		cost = 0;
		communication = 0;
		cardinality = 0;
		cum_cardinality = 0;
		for(unsigned i = 0 ; i < variables.size(); i++){
			variables_bindings[variables[i]] = std::numeric_limits<int>::max();
		}
	}
    void addNode(int i){
        this->ordered_nodes.push_back(i);
    }

	string print(){
		string result = "";
		result+= print_list(this->ordered_nodes, "|")+"\n";
		result += "Cost: "+toString(cost)+"\nCommunication= "+toString(communication)+"\nCardinality= "+toString(cardinality)+"\nCummulative: "+toString(cum_cardinality)+"\n";
		for(boost::unordered_map<string, float>::iterator it = variables_bindings.begin(); it != variables_bindings.end() ; it++){
			result += it->first+"--->"+toString(it->second)+"\n";
		}
		result += res.encode_query();
		return result;
	}
};
typedef boost::unordered_map<string, dynamic_state> DT;
//===============================================================================================================
struct ComparePairs {
	bool operator()(pair<string, int> const & p1, pair<string, int> const & p2) {
		return p1.second < p2.second;
	}
};
typedef std::priority_queue<pair<string, int>, vector<pair<string, int> >, ComparePairs > sorted_pairs_t;
//===============================================================================================================
struct ComparePairsDesc {
	bool operator()(pair<string, long long> const & p1, pair<string, long long> const & p2) {
		return p1.second > p2.second;
	}
};
typedef std::priority_queue<pair<string, long long>, vector<pair<string, long long> >, ComparePairsDesc > sorted_pairs_desc_t;
//===============================================================================================================
struct ComparePairs2 {
	bool operator()(pair<int, int> const & p1, pair<int, int> const & p2) {
		return p1.second > p2.second;
	}
};
typedef std::priority_queue<pair<int, int>, vector<pair<int, int> >, ComparePairs2 > sorted_int_pairs_t;
//===============================================================================================================
struct compare_cardinalities {
    bool operator()(pair<int, long long> const & p1, pair<int, long long> const & p2) {
        return p1.second > p2.second;
    }
};
typedef std::priority_queue<pair<int, long long>, vector<pair<int, long long> >, compare_cardinalities > soreted_cardinalities;
//===============================================================================================================

class Plan {
public:
    Plan(MasterGUI* master, Query q, bool adaptivity_enabled);
	Plan(const Plan& orig);
	Plan(vector<string> &params, bool redistribution);
    Plan(vector<string> &params);
	virtual ~Plan();
	PLAN_OP operation;
	Query query;
    MasterGUI* master;
	vector<Node> ordered_subqueries;
	string initial_vertex;
	vector<walk_edge*> ODE;
	string core;
	map<string, float> walk_scores;
	boost::unordered_set<query_index_node*> leaves;
	void plan_semi_join();
	void order_subqueries_for_parallel_execution(vector<Local_Node> &nodes, boost::unordered_map<string, predicate_stat> & stat_table);
	void order_subqueries_for_semi_join();
	void update_supqueries_cardinality();
    void get_projection_pos(vector<string> &res, vector<int> &pos);
private:
	void generatePlan();
	void find_plan_type();
	void order_subqueries();
	void update_subquery_cardinality(Node &subquery);
	void encode_subqueries();
	void branch(string state_string, int next, vector<int> remaining);
	void branch_local(vector<Local_Node> &nodes, string state_string, int next, set<int> remaining, boost::unordered_map<string, predicate_stat> & stat_table);
	void compute_walk_scores();
	void compute_walk_scores_max();
	void compute_walk_scores_sum();
	void walk_query();
	void get_triple_by_ends(vector<walk_edge*> &res, string &end1, string &end2, boost::unordered_set<walk_edge*, walk_edge_hash, walk_edge_equal> &filter);
	void plan_for_redistribution(vector<string> &params);
	void plan_for_parallel_execution(vector<string> &params);
	bool is_empty();
    bool is_star();
	DT order_table;
    bool adaptivity_enabled;
};
#endif
