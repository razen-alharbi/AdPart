#ifndef MASTER_EXECUTER_H
#define MASTER_EXECUTER_H
#include "engine/mastergui.h"
#include "engine/communicator.h"
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
using namespace std;

struct Exec_info{
    double wall_time;
    PLAN_OP exec_type;
    long long cardinality;
    int distributed_joins;
    int parallel_joins;

    Exec_info(){
        this->wall_time = 0;
        this->exec_type = SEMI_JOIN;
        this->cardinality = 0;
        this->distributed_joins = 0;
        this->parallel_joins = 0;
    }

    void reset(){
        this->wall_time = 0;
        this->exec_type = SEMI_JOIN;
        this->cardinality = 0;
        this->distributed_joins = 0;
        this->parallel_joins = 0;
    }
};

class Plan;
class MasterGUI;
class Master_Executer{
public:
    Master_Executer(MasterGUI * master, Query &query, bool adaptivity_enabled, Exec_info *info, bool silent);
	~Master_Executer();
    void execute();
	void distributed_execution(Plan &plan);
	void parallel_execution(Plan &plan);
	long long join_two_queries(Plan &plan, Node &q1, Node &q2, Node *ahead, Node &res, int final);
	void star_query_execution(Plan &plan);
    PLAN_OP start_phd(Plan &plan);
	void start_distribution(Plan &plan);
    PLAN_OP evict_replicated_data(Plan &plan);
    long long get_aggregate_comm_volume();
	/*void end_execution(string encoded_q1);
	void get_aggregate_query_mass(Statistics_Manager *manager, string &encoded_querystring);
	void wait_for_clients();
	void check_triples_exist(vector<string> &triples, vector<bool> & exist);*/
private:
	//map<string, long long> queries_masses;
	Profiler profiler;
    MasterGUI * master;
	Query query;
    bool adaptivity_enabled;
    Exec_info * info;
    bool silent;
};


#endif /* MASTER_EXECUTER_H */
