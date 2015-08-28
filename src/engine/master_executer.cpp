#include "engine/master_executer.h"
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
Master_Executer::Master_Executer(MasterGUI * master, Query &query, bool adaptivity_enabled, Exec_info * info, bool silent){
    this->master = master;
    this->query = query;
    this->adaptivity_enabled = adaptivity_enabled;
    this->info = info;
    this->silent = silent;
}

Master_Executer::~Master_Executer(){
}


void Master_Executer::execute(){
    master->logger.writeToLog(part_string, false);
    this->profiler.startTimer("wall_time");
    string encoded_q1;
    this->profiler.startTimer("planing");
    Plan plan(this->master, this->query, adaptivity_enabled);
    this->profiler.pauseTimer("planing");
    double planing = this->profiler.readPeriod("planing");
    master->logger.writeToLog("Done Planing!", false);
    //master->logger.writeToLog("Planing time: " + toString(planing), false);
    this->profiler.clearTimer("planing");
    if(plan.operation == EMPTY){
        master->logger.writeToLog("Empty Results", false);
    }
    else if (plan.operation == SEMI_JOIN){
        master->logger.writeToLog("SEMI_JOIN", false);
        this->distributed_execution(plan);
    }
    else if(plan.operation == STAR){
        master->logger.writeToLog("STAR", false);
        this->star_query_execution(plan);
    }
    else if(plan.operation == PARALLEL){
        master->logger.writeToLog("PARALLEL", false);
        this->parallel_execution(plan);
    }
    else if(plan.operation == PHD_SEMI_JOIN || plan.operation == PHD_PARALLEL){
        master->logger.writeToLog("PHD", false);
        plan.operation = this->start_phd(plan);
    }
    this->profiler.pauseTimer("wall_time");
    double wall_time = this->profiler.readPeriod("wall_time");
    master->logger.writeToLog("Planing time: " + toString(planing)+"\nWall time: " + toString(wall_time), false);
#ifdef COMM_VOLUME
    master->logger.writeToLog("Communication Volume: "+toString(get_aggregate_comm_volume()), false);
#endif
    this->profiler.clearTimer("wall_time");
    info->wall_time = wall_time;
    info->exec_type = plan.operation;
}

void Master_Executer::distributed_execution(Plan &plan){
    master->logger.writeToLog(" - Distributed execution", false);
    vector<string> cmd_params;
    Node q1, q2, result;
    long long result_size;

    q1 = plan.ordered_subqueries[0];

    for (unsigned i = 1; i < plan.ordered_subqueries.size(); i++) {
        q2 = plan.ordered_subqueries[i];
        if(i<plan.ordered_subqueries.size()-1){
            result_size = join_two_queries(plan, q1, q2, &plan.ordered_subqueries[i+1], result, (i==(plan.ordered_subqueries.size()-1)));
        }
        else{
            result_size = join_two_queries(plan, q1, q2, NULL, result, (i==(plan.ordered_subqueries.size()-1)));
        }
        if(result_size == 0){
            master->logger.writeToLog("Cutting short! Empty Results", false);
            cmd_params.clear();
            this->info->cardinality = 0;
            this->master->prepare_results_table(cmd_params, 0);
            break;
        }
        q1 = result;
    }
}

long long Master_Executer::join_two_queries(Plan &plan, Node &q1, Node &q2, Node * ahead, Node &res, int final) {
    this->profiler.startTimer("join");
    master->logger.writeToLog(part_string, false);
    vector<string> cmd_params;
    vector<pair<int, int> > join_cols, ahead_jc;
    vector<long long> res_sizes;
    vector<int> final_result;
    Node ahead_res;
    PREPARE_AHEAD prep_op;
    int prep_col;
    long long results_size;
    vector<int> pos;

    cmd_params.clear();
    res_sizes.resize(this->master->communicator->num_workers);
    master->logger.writeToLog("Join query: "+q1.encode_query() +"\t[total mass: "+toString(q1.cardinality)+"]", false);
    master->logger.writeToLog("With query: "+ q2.encode_query() +"\t[total mass: "+toString(q2.cardinality)+ "]", false);

    prep_op = NO_PREPARE;
    prep_col = -1;
    q1.get_join_node_and_cols(q2, join_cols, res);
    if(ahead != NULL){
        res.get_join_node_and_cols(*ahead, ahead_jc, ahead_res);
        if(ahead->row[ahead_jc[0].second] == plan.initial_vertex){
            if(ahead_jc[0].second == 0){
                master->logger.writeToLog("PWOC", false);
                prep_op = NO_PREPARE;
                prep_col = -1;
            }
            else{
                if(!isVariable(ahead->row[0])){
                    master->logger.writeToLog("PREPARE_LOCALITY_SUBJECT", false);
                    prep_op = PREPARE_LOCALITY_SUBJECT;
                    prep_col = ahead_jc[0].first;
                }
                else{
                    master->logger.writeToLog("RANDOM", false);
                    prep_op = PREPARE_RANDOM;
                    prep_col = ahead_jc[0].first;
                }
            }
        }
        else{
            if(ahead_jc[0].second == 0){
                master->logger.writeToLog("LOCALITY", false);
                prep_op = PREPARE_LOCALITY;
                prep_col = ahead_jc[0].first;
            }
            else{
                if(!isVariable(ahead->row[0])){
                    master->logger.writeToLog("PREPARE_LOCALITY_SUBJECT", false);
                    prep_op = PREPARE_LOCALITY_SUBJECT;
                    prep_col = ahead_jc[0].first;
                }
                else{
                    master->logger.writeToLog("RANDOM", false);
                    prep_op = PREPARE_RANDOM;
                    prep_col = ahead_jc[0].first;
                }
            }
        }
    }
    cmd_params.push_back(toString(prep_op));
    cmd_params.push_back(toString(prep_col));

    for(unsigned i = 0 ; i < join_cols.size();i++){
        master->logger.writeToLog(toString(join_cols[i].first)+" "+toString(join_cols[i].second), false);
    }

    if(q2.row[join_cols[0].second] == plan.initial_vertex && join_cols[0].second == 0){
        master->logger.writeToLog("NO COMMUNICATION", false);
        cmd_params.push_back(toString(final));
        cmd_params.push_back(q1.encode_query());
        cmd_params.push_back(q2.encode_query());
        for(unsigned i = 0 ; i < join_cols.size(); i++){
            cmd_params.push_back(toString(join_cols[i].first));
            cmd_params.push_back(toString(join_cols[i].second));
        }
        if(prep_op == PREPARE_LOCALITY_SUBJECT){
            cmd_params.push_back(toString(ahead->row[0]));
        }
        this->info->parallel_joins++;
        master->communicator->send_command(CMD_JOIN_WITHOUT_COMMUNICATION, cmd_params, SEND_TO_ALL);
    }
    else{
        master->logger.writeToLog("WITH COMMUNICATION", false);
        cmd_params.push_back(toString(final));
        cmd_params.push_back(q1.encode_query());
        cmd_params.push_back(q2.encode_query());
        for(unsigned i = 0 ; i < join_cols.size(); i++){
            cmd_params.push_back(toString(join_cols[i].first));
            cmd_params.push_back(toString(join_cols[i].second));
        }
        if(prep_op == PREPARE_LOCALITY_SUBJECT){
            cmd_params.push_back(toString(ahead->row[0]));
        }
        this->info->distributed_joins++;
        master->communicator->send_command(CMD_JOIN_WITH_COMMUNICATION, cmd_params, SEND_TO_ALL);
        this->master->communicator->sync();//For distributing the Join Column
        this->master->communicator->sync();//For distributing the candidate list
    }
    if(final){
        plan.get_projection_pos(res.row, pos);
        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            res_sizes[i] = this->master->communicator->mpi_receive_long(i);
            res.cardinality += res_sizes[i];
        }
        this->info->cardinality = res.cardinality;
        if(!silent){
            this->master->prepare_results_table(plan.query.projections, (int)res.cardinality);
        }
        this->master->communicator->sync();
        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            final_result.resize(res.row.size()*res_sizes[i]);
            this->master->communicator->mpi_receive_int_vector(final_result, i);
            if(!silent){
                this->master->update_results(pos, final_result, res.row.size());
            }
        }
        results_size = res.cardinality;
    }
    else{
        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            res.cardinality += this->master->communicator->mpi_receive_long(i);
        }
        results_size = res.cardinality;
    }
    this->profiler.pauseTimer("join");
    master->logger.writeToLog("join took: "+toString(this->profiler.readPeriod("join")), false);
    this->profiler.clearTimer("join");
    if(final){
        master->logger.writeToLog("Results: "+toString(res.cardinality), false);
    }
    return results_size;
}

void Master_Executer::star_query_execution(Plan &plan){
    master->logger.writeToLog("- Embarrassingly Parallel (Star)", false);
    vector<string> cmd_params;
    vector<int> final_result;
    Node res, q1;
    vector<pair<int, int> > join_cols;
    vector<vector<int> > pos;
    vector<int> res_sizes;

    res_sizes.resize(this->master->communicator->num_workers);
    pos.resize(this->master->communicator->num_workers);
    q1 = plan.query.nodes[0];
    cmd_params.push_back(toString(plan.query.nodes.size()));
    cmd_params.push_back(q1.encode_query());
    for(unsigned i = 1 ; i < plan.query.nodes.size() ; i++){
        cmd_params.push_back(plan.query.nodes[i].encode_query());
        q1.get_join_node_and_cols(plan.query.nodes[i], join_cols, res);
        q1 = res;
    }

    if(plan.query.nodes.size() == 1)
        q1.get_join_node_and_cols(q1, join_cols, res);

    for(unsigned i = 0 ; i < plan.query.projections.size() ; i++){
        cmd_params.push_back(plan.query.projections[i]);
    }
    this->info->parallel_joins = plan.query.nodes.size()-1;
    master->communicator->send_command(CMD_SOLVE_STAR_QUERY, cmd_params, SEND_TO_ALL);

    for(int i = 1 ; i < this->master->communicator->num_workers; i++){
        final_result.resize(plan.query.projections.size()+1);
        this->master->communicator->mpi_receive_int_vector(final_result, i);
        res_sizes[i] = final_result[0];
        pos[i].insert(pos[i].end(), final_result.begin()+1, final_result.end());
        res.cardinality += res_sizes[i];
    }
    this->info->cardinality = res.cardinality;
    if(!silent){
        this->master->prepare_results_table(plan.query.projections, (int)res.cardinality);
    }
    this->master->communicator->sync();
    for(int i = 1 ; i < this->master->communicator->num_workers; i++){
        final_result.resize(res.row.size()*res_sizes[i]);
        this->master->communicator->mpi_receive_int_vector(final_result, i);
        if(!silent){
            this->master->update_results(pos[i], final_result, res.row.size());
        }
    }
    master->logger.writeToLog("Results: "+toString(res.cardinality), false);

}

PLAN_OP Master_Executer::start_phd(Plan &plan){
    PLAN_OP op = SEMI_JOIN;
    if(this->master->manager->replication_ratio < max_replication_ratio){
        this->profiler.startTimer("PHD");
        this->start_distribution(plan);
        this->master->q_index->add_distributed_query(plan);
        if(plan.operation == PHD_PARALLEL){
            this->master->q_index->is_parallel_query(&plan);
            this->parallel_execution(plan);
            op = PARALLEL;
        }
        else{
            plan.plan_semi_join();
            if(plan.operation == EMPTY){
                master->logger.writeToLog("Empty Results", false);
            }
            else{
                this->distributed_execution(plan);
            }
            op = SEMI_JOIN;
        }
        this->profiler.pauseTimer("PHD");
        master->logger.writeToLog("Redistribution took: "+toString(this->profiler.readPeriod("PHD")), false);
        master->logger.writeToLog("Replication Ratio: "+toString(this->master->manager->replication_ratio)+", Gini: "+toString(this->master->manager->gini), false);
        this->profiler.clearTimer("PHD");

    }
    else{
        master->logger.writeToLog(part_string, false);
        master->logger.writeToLog("EXCEEDED REPLICATION RATIO! NEED TO EVICT", false);
        op = this->evict_replicated_data(plan);
    }
    return op;
}

void Master_Executer::start_distribution(Plan &plan){
    vector<string> cmd_params;
    net_cmd cmd;
    vector<string> paths;
    map<string, float>::iterator it;
    int src;
    vector<long long> level_replication;

    for(unsigned i = 0 ; i < plan.query.nodes.size(); i++){
        if(plan.ODE[i]->children.size() == 0){
            //master->logger.writeToLog(plan.ODE[i]->print2(), false);
            plan.ODE[i]->get_path(cmd_params);
            cmd_params.push_back("EOP");
        }
    }
    this->master->communicator->send_command(CMD_START_PHD, cmd_params, SEND_TO_ALL);
    for(int i = 1 ; i < this->master->communicator->num_workers; i++){
        src = this->master->communicator->receive_command(cmd, cmd_params);
        this->master->manager->replicated_triples_load[src] += atoll(cmd_params[0].c_str());
        this->master->manager->replicated_triples_load[0] += atoll(cmd_params[0].c_str());
        this->master->manager->total_triples_load[src] += atoll(cmd_params[0].c_str());
        this->master->manager->total_triples_load[0] += atoll(cmd_params[0].c_str());
    }
    this->master->manager->replication_ratio = ((float)this->master->manager->replicated_triples_load[0])/this->master->manager->original_triples_load[0];
    this->master->manager->gini = gini_coef(this->master->manager->total_triples_load);
    master->logger.writeToLog("Total Load: "+print_list(this->master->manager->total_triples_load, ","), false);
}

void Master_Executer::parallel_execution(Plan &plan){
    master->logger.writeToLog("- Embarrassingly Parallel", false);
    vector<string> cmd_params;
    map<string, float>::iterator it;
    vector<int> final_result;
    long long total =0;
    int dest = 0;
    vector<vector<int> > pos;
    vector<int> res_sizes;

    res_sizes.resize(this->master->communicator->num_workers);
    pos.resize(this->master->communicator->num_workers);
    plan.update_supqueries_cardinality();

    if(isVariable(plan.core)){
        cmd_params.push_back("0");
    }
    else{
        dest = (atoi(plan.core.c_str()) % (this->master->communicator->num_workers-1))+1;
        cmd_params.push_back(toString(dest));
    }

    cmd_params.push_back(toString(plan.ODE.size()));
    for(unsigned i = 0 ; i < plan.ODE.size(); i++){
        cmd_params.push_back(plan.query.nodes[i].encode_query());
    }
    plan.order_subqueries_for_semi_join();
    for(unsigned i = 0 ; i < plan.ordered_subqueries.size(); i++){
        cmd_params.push_back(plan.ordered_subqueries[i].encode_query());
    }
    cmd_params.push_back(toString(plan.walk_scores.size()));
    for (it = plan.walk_scores.begin(); it != plan.walk_scores.end(); it++) {
        cmd_params.push_back(it->first);
        cmd_params.push_back(toString(it->second));
    }
    for(unsigned i = 0 ; i < plan.query.projections.size(); i++){
        cmd_params.push_back(plan.query.projections[i]);
    }
    if(isVariable(plan.core)){
        this->master->communicator->send_command(CMD_EXECUTE_QUERY_IN_PARALLEL, cmd_params, SEND_TO_ALL);
    }
    else{
        this->master->communicator->send_command(CMD_EXECUTE_QUERY_IN_PARALLEL, cmd_params, dest);
    }
    this->info->parallel_joins = plan.query.nodes.size()-1;
    if(isVariable(plan.core)){

        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            final_result.resize(plan.query.projections.size()+1);
            this->master->communicator->mpi_receive_int_vector(final_result, i);
            res_sizes[i] = final_result[0];
            pos[i].insert(pos[i].end(), final_result.begin()+1, final_result.end());
            total += res_sizes[i];
        }
        this->info->cardinality = total;
        if(!silent){
            this->master->prepare_results_table(plan.query.projections, (int)total);
        }
        this->master->communicator->sync();
        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            final_result.resize(plan.query.variables.size()*res_sizes[i]);
            this->master->communicator->mpi_receive_int_vector(final_result, i);
            if(!silent){
                this->master->update_results(pos[i], final_result, plan.query.variables.size());
            }
        }
        master->logger.writeToLog("Results: "+toString(total), false);
    }
    else{

        final_result.resize(plan.query.projections.size()+1);
        this->master->communicator->mpi_receive_int_vector(final_result, dest);
        res_sizes[dest] = final_result[0];
        pos[dest].insert(pos[dest].end(), final_result.begin()+1, final_result.end());
        total = res_sizes[dest];
        this->info->cardinality = total;
        final_result.resize(plan.query.variables.size()*total);
        this->master->communicator->mpi_receive_int_vector(final_result, dest);
        if(!silent){
            this->master->prepare_results_table(plan.query.projections, total);
            this->master->update_results(pos[dest], final_result, plan.query.variables.size());
        }

        master->logger.writeToLog("Results: "+toString(total), false);
    }
}

PLAN_OP Master_Executer::evict_replicated_data(Plan &plan){
    unordered_set<query_index_node*>::iterator it;
    double min = numeric_limits<double>::max();
    query_index_node* node = NULL, *tmp_node;
    vector<string> params;
    net_cmd cmd;
    int src;
    for(it = this->master->q_index->qi_leaves.begin(); it != this->master->q_index->qi_leaves.end(); it++){
        if((*it)->ts < min){
            min = (*it)->ts;
            node = *it;
        }
    }
    //cout<<node->print()<<endl;
    if(node != NULL){
        tmp_node = node;
        while(tmp_node != NULL){
            params.push_back(toString(tmp_node->reversed));
            params.push_back(tmp_node->encode());
            tmp_node = tmp_node->parent;
        }
        this->master->communicator->send_command(CMD_EVICT, params, SEND_TO_ALL);
        for(int i = 1 ; i < this->master->communicator->num_workers; i++){
            src = this->master->communicator->receive_command(cmd, params);
            this->master->manager->replicated_triples_load[src] -= atoll(params[0].c_str());
            this->master->manager->replicated_triples_load[0] -= atoll(params[0].c_str());
            this->master->manager->total_triples_load[src] -= atoll(params[0].c_str());
            this->master->manager->total_triples_load[0] -= atoll(params[0].c_str());
        }
        this->master->manager->replication_ratio = ((float)this->master->manager->replicated_triples_load[0])/this->master->manager->original_triples_load[0];
        this->master->manager->gini = gini_coef(this->master->manager->total_triples_load);

        if(node->parent == NULL){//Core incident edge
            node->exact = true;
            this->master->q_index->distrib_queries->erase(node);
            delete node;
        }
        else{//internal leaf node
            node->exact = true;
            node->parent->children.erase(node);
            if(node->parent->children.size()==0){
                this->master->q_index->qi_leaves.insert(node->parent);
            }
            delete node;
        }
        this->master->q_index->qi_leaves.erase(node);

        return this->start_phd(plan);
    }
    else{
        throwException("EVICTION CANNOT PROCCEEDE");
        return FAIL;
    }
}

long long Master_Executer::get_aggregate_comm_volume(){
    vector<string> params;
    net_cmd cmd;
    long long total = this->master->communicator->comm_volume;

    this->master->communicator->send_command(CMD_COMM_VOL, params, SEND_TO_ALL);

    for(int i = 1 ; i < this->master->communicator->num_workers; i++){
        this->master->communicator->receive_command(cmd, params);
        total += atoll(params[0].c_str());
    }
    return total;
}
