#include "engine/client.h"

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
Client::Client(string &adhash_home, string &db_folder, string &file_name){
    this->adhash_home = adhash_home;
    this->logger = Logger(this->adhash_home);
    this->db_folder = db_folder;
    this->rank = MPI::COMM_WORLD.Get_rank();
    this->communicator = new Communicator();
    this->file_name = file_name;
    this->all_join_cols.resize(this->communicator->num_workers);
    this->all_candidate_lists.resize(this->communicator->get_num_workers());
    current_tmp_index = 0;
    srand(time(NULL));
    this->start();
}

Client::~Client(){

}

void Client::start(){
    vector<string> params;
    logger.writeToLog("Starting Client "+toString(rank)+"!", false);
    this->wait_for_commands();
}
int Client::get_rank(){
    return rank;
}

void Client::wait_for_commands(){
    logger.writeToLog(part_string, false);
    logger.writeToLog("ready to receive commands..", false);

    int src;
    net_cmd cmd;
    vector<string> params;
    while (1) {
        src = this->communicator->receive_command(cmd, params);
        logger.writeToLog(part_string, false);
        logger.writeToLog("-CMD: "+ this->communicator->translate_command_to_string(cmd)+" from "+toString(src), false);
        if(cmd == CMD_KILL){
            logger.writeToLog("Client "+toString(rank)+" terminated gracefully!", false);
            logger.writeToLog("-CMD: "+ this->communicator->translate_command_to_string(cmd)+" from "+toString(src)+" processed successfully!", false);
            this->communicator->sync();
            MPI::Finalize();
            break;
        }
        else{
            this->resolve_command(cmd, params, src);
        }
        logger.writeToLog("-CMD: "+ this->communicator->translate_command_to_string(cmd)+" from "+toString(src)+" processed successfully!", false);
    }
}

void Client::load_data(){
    logger.writeToLog(part_string, false);
    this->profiler.startTimer("loading_data");
    FILE * inFile;
    long int num_rec, records_per_iteration, num_iterations;
    char * buffer;
    size_t result;
    string time;
    vector<string> params;
    int max_predicate = -1;
    int max_vert = -1;
    string fn = this->db_folder+"parts/"+toString(this->communicator->get_num_workers()-1)+"/"+this->file_name+toString(this->communicator->get_worker_id());
    inFile = fopen(fn.c_str(), "rb");

    if(inFile==NULL){
        cerr<<("Could not open the file")<<endl;
        exit (2);
    }

    buffer = (char*) malloc(sizeof(char)*BUFFERSIZE);
    if(buffer == NULL){
        cout<<("Memory error")<<endl;
        exit (2);
    }

    fseek (inFile , 0 , SEEK_END);
    num_rec = ftell (inFile)/RECORDSIZE;
    records_per_iteration = BUFFERSIZE/RECORDSIZE;
    num_iterations = ceil(1.0*num_rec/records_per_iteration);
    rewind (inFile);
    int subj, obj, pred, type, triple_counter = 0;
    for(unsigned int i = 0 ; i < num_iterations; i++){
        result = fread(buffer, RECORDSIZE, records_per_iteration, inFile);
        for(unsigned int j = 0 ; j < result*RECORDSIZE ; j+=RECORDSIZE){
            memcpy(&subj, buffer+j, sizeof(int));
            memcpy(&pred, buffer+j+4, sizeof(int));
            memcpy(&obj, buffer+j+8, sizeof(int));
            memcpy(&type, buffer+j+12, sizeof(int));
            if(pred > max_predicate)
                max_predicate = pred;
            if(subj > max_vert)
                max_vert = subj;
            if(obj > max_vert)
                max_vert = obj;
            store.insertTriple(subj, pred, obj);
            triple_counter++;
            if (triple_counter % 1000000 == 0)
                logger.writeToLog("Read " + toString(triple_counter) + " triples..", false);
        }
    }
    fclose(inFile);
    free(buffer);
    this->profiler.pauseTimer("loading_data");
    time = toString(this->profiler.readPeriod("loading_data"));
    this->profiler.clearTimer("loading_data");
    this->store.get_all_predicates(params);
    params.push_back(time);
    params.push_back(toString(this->store.getSize()));
    params.push_back(toString(max_predicate));
    params.push_back(toString(max_vert));
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::resolve_command(net_cmd cmd, vector<string> &params, int src){
    switch(cmd){
    case CMD_LOAD_DATA:
        this->load_data();
        break;
    case CMD_COMPUTE_PREDICATE_STATS:
        this->compute_predicate_stats(params);
        break;
    case CMD_CLEAN_VERTS_DATA:
        this->clean_verts_data();
        break;
    case CMD_GET_TRIPLE_LOAD:
        this->get_triple_load(src);
        break;
    case CMD_EXCHANGE_VERTS_STATS:
        this->exchange_verts_stats(params);
        break;
    case CMD_GET_SUBQUERY_CARDINALITY:
        this->get_subquery_cardinality(params);
        break;
    case CMD_JOIN_WITHOUT_COMMUNICATION:
        this->join_without_communication(params);
        break;
    case CMD_JOIN_WITH_COMMUNICATION:
        this->join_with_communication(params);
        break;
    case CMD_SOLVE_STAR_QUERY:
        this->solve_star_query(params);
        break;
    case CMD_START_PHD:
        this->start_phd(params);
        break;
    case CMD_EXECUTE_QUERY_IN_PARALLEL:
        this->execute_query_in_parallel(params);
        break;
    case CMD_STORE_STATS:
        this->store_stats(params);
        break;
    case CMD_EVICT:
        this->evict(params);
        break;
    case CMD_COMM_VOL:
        this->get_communication_volume(params);
        break;
    default:
        logger.writeToLog("ERROR: UNRECOGNIZED COMMAND RECIEVED "+toString(cmd), true);
        break;
    }
}

void Client::compute_predicate_stats(vector<string> &params){
    int predicate = atoi(params[0].c_str());
    predicate_stat stat(predicate);
    this->profiler.startTimer("TOTAL");
    this->store.compute_stats(predicate, stat, this->communicator);
    this->profiler.pauseTimer("TOTAL");
    logger.writeToLog("TOTAL: "+toString(this->profiler.readPeriod("TOTAL")), false);
    this->profiler.clearTimer("TOTAL");
    logger.writeToLog(stat.print(true), false);
    params.clear();
    params.push_back(toString(stat.subject_total_degree));
    params.push_back(toString(stat.subject_uniques));
    params.push_back(toString(stat.subject_score));
    params.push_back(toString(stat.object_total_degree));
    params.push_back(toString(stat.object_uniques));
    params.push_back(toString(stat.object_score));
    params.push_back(toString(stat.total_count));
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::clean_verts_data(){
    vector<string> params;
    this->store.verts_counter.clear();
    this->communicator->comm_volume = 0;
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::store_stats(vector<string> &params){
    vector<string> split;
    predicate_stat stat;
    for(unsigned i = 0 ; i < params.size(); i++){
        split_string(params[i], "*", split);
        stat.predicate_text = atoi(split[0].c_str());
        stat.pred_per_subj = atof(split[1].c_str());
        stat.pred_per_obj = atof(split[2].c_str());
        predicates_stats[split[0]] = stat;
    }
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::get_triple_load(int src){
    long long load = this->store.getSize();
    this->communicator->mpi_send_long(load, src);
}

void Client::exchange_verts_stats(vector<string> &params){
    this->store.exchange_verts_stats(this->communicator);
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::get_subquery_cardinality(vector<string> &params){
    Node subquery = Node(params[0]);
    long long cardinality = 0;
    if(!subquery.temp_query){
        cardinality = store.calc_sub_mass(subquery);
    }
    //else
    //cardinality = this->queries_data[encoded_querystring].size();
    logger.writeToLog("Cardinality of "+params[0]+": "+toString(cardinality), false);
    this-> communicator->mpi_send_long(cardinality, 0);
}

void Client::join_without_communication(vector<string> &params){
    int prep_locality = atoi(params[0].c_str());
    int prep_col = atoi(params[1].c_str());
    int final = atoi(params[2].c_str());
    long long total = 0;
    Node q1(params[3]), q2(params[4]);
    vector<pair<int, int> > join_cols;
    int q2_const_subject = 0;

    for(unsigned i = 5 ; i < params.size()-1 ; i+=2){
        join_cols.push_back(make_pair(atoi(params[i].c_str()), atoi(params[i+1].c_str())));
    }
    if(prep_locality == PREPARE_LOCALITY_SUBJECT){
        q2_const_subject = atoi(params[params.size()-1].c_str());
    }

    if(!q1.temp_query){
        this->store.answer_sub(q1, this->store.main_index, tmp_data, this->all_join_cols, false, 0, 0, this->rank, this->communicator->get_num_workers(), this->tmp_index, current_tmp_index, q2_const_subject);
    }
    total = this->store.join_two_queries(q1, q2, join_cols, tmp_data, this->store.main_index, prep_locality, prep_col, this->all_join_cols, this->communicator->num_workers, this->rank, this->tmp_index, current_tmp_index, q2_const_subject);

    if(final){
        this->communicator->mpi_send_long(total, 0);
        this->communicator->sync();
        this->communicator->mpi_send_int_vector(tmp_data, 0);
    }
    else{
        this->communicator->mpi_send_long(total, 0);
    }
}
void Client::join_with_communication(vector<string> &params){
    int prep_locality = atoi(params[0].c_str());
    int prep_col = atoi(params[1].c_str());
    int final = atoi(params[2].c_str()), current_jc;
    long long total = 0;
    Node q1(params[3]), q2(params[4]);
    vector<pair<int, int> > join_cols;
    int jc_locality;
    int q2_const_subject = 0;


    for(unsigned i = 5 ; i < params.size()-1 ; i+=2){
        join_cols.push_back(make_pair(atoi(params[i].c_str()), atoi(params[i+1].c_str())));
    }
    if(!q1.temp_query){
        if(join_cols[0].second == 2){
            if(!isVariable(q2.row[0])){
                jc_locality = PREPARE_LOCALITY_SUBJECT;
                q2_const_subject = atoi(q2.row[0].c_str());
            }
            else
                jc_locality = PREPARE_RANDOM;
        }
        else{
            jc_locality = PREPARE_LOCALITY;
        }
        if(join_cols[0].first == 2)
            current_jc = 1;
        else
            current_jc = 0;

        this->store.answer_sub(q1, this->store.main_index, tmp_data, this->all_join_cols, true, jc_locality, current_jc, this->rank, this->communicator->get_num_workers(), this->tmp_index, current_tmp_index, q2_const_subject);
    }

    if(prep_locality == PREPARE_LOCALITY_SUBJECT){
        q2_const_subject = atoi(params[params.size()-1].c_str());
    }

    if(join_cols[0].second == 0){
        this->distribute_join_column(true);
    }
    else{
        if(!isVariable(q2.row[0])){
            this->distribute_join_column(true);
        }
        else{
            this->distribute_join_column(false);
        }
    }
    this->communicator->sync();

    this->build_candidate_lists(q2, join_cols[0].second);
    this->distribute_candidate_lists();

    this->communicator->sync();

    //cout<<"RR"<<prep_locality<<prep_col<<endl;
    total = this->finalize_join(q1, q2, join_cols, prep_locality, prep_col, q2_const_subject);

    if(final){
        this->communicator->mpi_send_long(total, 0);
        this->communicator->sync();
        this->communicator->mpi_send_int_vector(tmp_data, 0);
    }
    else{
        this->communicator->mpi_send_long(total, 0);
    }

}

void Client::distribute_join_column(bool locality_aware){
    vector<vector<int> > src_jc;
    int total = 0;
    if(locality_aware){
        this->communicator->mpi_all_to_all(this->all_join_cols, src_jc);

        for(unsigned i = 1 ; i < src_jc.size(); i++){
            this->all_join_cols[i].clear();
            this->all_join_cols[i].insert(this->all_join_cols[i].end(), src_jc[i].begin(), src_jc[i].end());
            logger.writeToLog("recieved jc from "+toString(i)+" of size "+toString(this->all_join_cols[i].size()), false);
        }

    }
    else{
        src_jc.push_back(this->all_join_cols[this->rank]);

        this->communicator->mpi_all_to_all(src_jc, this->all_join_cols);
        for(unsigned i = 1 ; i < this->all_join_cols.size(); i++){
            logger.writeToLog("recieved jc from "+toString(i)+" of size "+toString(this->all_join_cols[i].size()), false);
        }
    }
    for(unsigned i = 1 ; i < this->all_join_cols.size(); i++){
        total += this->all_join_cols[i].size();
    }
}

void Client::build_candidate_lists(Node &q2, int join_col){
    for(unsigned i = 0 ; i < this->all_candidate_lists.size(); i++){
        this->all_candidate_lists[i].clear();
    }


    if(join_col == 2){
        join_col = 1;
    }

    // for each join column
    for (unsigned jc = 1; jc < this->all_join_cols.size(); jc++) {
        this->store.build_candidate_list(q2, join_col, this->all_join_cols[jc], this->all_candidate_lists[jc]);
        logger.writeToLog(toString(jc)+"--->"+toString(this->all_candidate_lists[jc].size()), false);
    }
}

void Client::distribute_candidate_lists(){
    vector<vector<int> > razen_candidate_lists;
    this->communicator->mpi_all_to_all(this->all_candidate_lists, razen_candidate_lists);

    for(unsigned i = 1 ; i < razen_candidate_lists.size(); i++){
        this->all_candidate_lists[i].clear();
        this->all_candidate_lists[i].insert(this->all_candidate_lists[i].end(), razen_candidate_lists[i].begin(), razen_candidate_lists[i].end());
        logger.writeToLog("Triple size: "+toString(this->all_candidate_lists[i].size()), false);
    }
}

long long Client::finalize_join(Node &q1, Node &q2, vector<pair<int, int> > &join_cols, int prep_locality, int prep_col, int q2_const_subject){
    long long res_cardinality = this->store.finalize_join(q1, q2, join_cols, prep_locality, prep_col, all_candidate_lists, tmp_index, current_tmp_index, all_join_cols, tmp_data, this->communicator->get_num_workers(), this->rank, q2_const_subject);
    this->current_tmp_index = ! this->current_tmp_index;
    return res_cardinality;
}

void Client::solve_star_query(vector<string> &params){
    vector<Node> new_order;
    soreted_cardinalities sp;
    long long total = 0;
    vector<pair<int, int> > join_cols;
    Node q1, res;
    bool join_on_const = true, first = true;
    string join_const_val;
    Plan plan(params);

    for(unsigned i = 0 ; i < plan.query.nodes.size() ;i++){
        plan.query.nodes[i].cardinality = this->store.calc_sub_mass(plan.query.nodes[i]);
        sp.push(make_pair(i, plan.query.nodes[i].cardinality));
    }
    while(!sp.empty()){
        new_order.push_back(plan.query.nodes[sp.top().first]);
        sp.pop();
    }
    ////////////////////////////////////////////////////////////////
    q1 = new_order[0];
    this->store.answer_sub(q1, this->store.main_index, tmp_data, this->all_join_cols, false, 0, 0, this->rank, this->communicator->get_num_workers(), this->tmp_index, current_tmp_index, 0);
    total = tmp_data.size();
    if(q1.variables == V_BOTH)
        total = tmp_data.size()/2;

    for(unsigned i = 1 ; i < new_order.size() ;i++){
        if(!isVariable(new_order[i].row[0])){
            if(first){
                first = false;
                join_const_val = new_order[i].row[0];
            }
            if(new_order[i].row[0] != join_const_val){
                join_on_const = false;
                break;
            }
        }
        else{
            join_on_const = false;
            break;
        }
    }
    for(unsigned i = 1 ; i < new_order.size() ;i++){
        q1.get_join_node_and_cols(new_order[i], join_cols, res);
        total = this->store.join_two_queries_star(q1, new_order[i], join_cols, tmp_data, join_on_const, join_const_val);
        q1 = res;
    }


    vector<int> pos;
    pos.push_back(total);
    if(new_order.size() == 1)
        plan.get_projection_pos(plan.query.projections, pos);
    else
        plan.get_projection_pos(res.row, pos);
    this->communicator->mpi_send_int_vector(pos, 0);
    this->communicator->sync();
    this->communicator->mpi_send_int_vector(tmp_data, 0);

}

void Client::start_phd(vector<string> &params){
    Plan plan(params, true);
    replica_index_node *tmp_rep_node, *parent_rep_node;
    replica_index_t::iterator it;
    boost::unordered_map<string, replica_index_node *> replica_ptrs;
    boost::unordered_map<string, replica_index_node *>::iterator ptrs_it;
    long long replicated_records_count = 0;
    vector<int> remaining, tmp;
    walk_edge * tmp_edge;

    //Working on core incident edges
    for(unsigned i = 0 ; i < plan.ODE.size() ; i++){
        if(plan.ODE[i]->parent == NULL){
            //if the core is constant we do exact match
            if(!isVariable(plan.ODE[i]->n1.label)){
                tmp_rep_node = new replica_index_node(plan.ODE[i], true);
                it = this->store.rep_index.find(tmp_rep_node);

                if(it != this->store.rep_index.end()){
                    replica_ptrs[plan.ODE[i]->encode()] = *it;
                    delete tmp_rep_node;
                    tmp_rep_node = *it;
                }
                else{
                    this->store.rep_index.insert(tmp_rep_node);
                    tmp_rep_node->exact = false;
                    replica_ptrs[plan.ODE[i]->encode()] = tmp_rep_node;
                }
            }
            else{
                tmp_rep_node = new replica_index_node(plan.ODE[i]);
                //cout<<tmp_rep_node->print2()<<endl;
                it = this->store.rep_index.find(tmp_rep_node);
                if(it == this->store.rep_index.end()){
                    this->store.rep_index.insert(tmp_rep_node);
                    replica_ptrs[plan.ODE[i]->encode()] = tmp_rep_node;
                }
                else{
                    replica_ptrs[plan.ODE[i]->encode()] = *it;
                    delete tmp_rep_node;
                    tmp_rep_node = *it;
                }
            }
            if(tmp_rep_node->reversed){
                if(tmp_rep_node->index == NULL){
                    tmp_rep_node->index = new Index();
                    this->store.hash_distribute_core_edge(tmp_rep_node, this->communicator, replicated_records_count);
                }
            }
            else{
                if(tmp_rep_node->index == NULL)
                    tmp_rep_node->index = this->store.main_index;
            }
        }
        else{
            remaining.push_back(i);
        }
    }

    while(!remaining.empty()){
        for(unsigned i = 0 ; i < remaining.size(); i++){
            tmp_edge = plan.ODE[remaining[i]];
            ptrs_it = replica_ptrs.find(tmp_edge->parent->encode());
            if(ptrs_it != replica_ptrs.end()){
                parent_rep_node = ptrs_it->second;
                if(!isVariable(tmp_edge->n2.label)){//i.e. constant leaf
                    tmp_rep_node = new replica_index_node(tmp_edge, true);
                    it = parent_rep_node->children.find(tmp_rep_node);
                    if(it == parent_rep_node->children.end()){
                        tmp_rep_node->parent = parent_rep_node;
                        parent_rep_node->children.insert(tmp_rep_node);
                        replica_ptrs[tmp_edge->encode()] = tmp_rep_node;
                        tmp_rep_node->exact = false;
                    }
                    else{
                        replica_ptrs[tmp_edge->encode()] = *it;
                        delete tmp_rep_node;
                        tmp_rep_node = *it;
                    }

                    //Now the rdistribution logic
                    if(tmp_rep_node->index == NULL){
                        tmp_rep_node->index = new Index();
                        this->store.redistribute_internal_edge(parent_rep_node, tmp_rep_node, this->communicator, replicated_records_count);
                    }
                }
                else{
                    tmp_rep_node = new replica_index_node(tmp_edge);
                    it = parent_rep_node->children.find(tmp_rep_node);
                    if(it == parent_rep_node->children.end()){
                        tmp_rep_node->parent = parent_rep_node;
                        parent_rep_node->children.insert(tmp_rep_node);
                        replica_ptrs[tmp_edge->encode()] = tmp_rep_node;
                    }
                    else{
                        replica_ptrs[tmp_edge->encode()] = *it;
                        delete tmp_rep_node;
                        tmp_rep_node = *it;
                    }
                    //Now the rdistribution logic
                    if(tmp_rep_node->index == NULL){
                        tmp_rep_node->index = new Index();
                        this->store.redistribute_internal_edge(parent_rep_node, tmp_rep_node, this->communicator, replicated_records_count);
                    }
                }
            }
            else{
                tmp.push_back(remaining[i]);
            }
        }
        remaining.swap(tmp);
    }
    params.clear();
    params.push_back(toString(replicated_records_count));
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::execute_query_in_parallel(vector<string> &params){
    long long total = 0;
    vector<Local_Node> nodes;
    Local_Node res_node;
    vector<int> results, pos;
    int num_nodes;
    int dest_worker = atoi(params[0].c_str());

    if((dest_worker == 0) || (dest_worker == this->rank)){
        Plan plan(params, false);
        /*if(rank==1)
            cout<<"HERE"<<plan.ODE.size()<<" "<<plan.core<<endl;
        for(unsigned i = 0 ; i < plan.ODE.size() ; i++){
            if(rank==1)
                cout<<"PHD "<<plan.ODE[i]->print2()<<endl;
        }*/
        this->associate_edges_with_replica_index_nodes(&plan);



        //Just get it to work
        num_nodes = atoi(&params[1][0]);
        for(int i = 0 ; i < num_nodes; i++){
            for(unsigned j = 0 ; j < plan.ODE.size(); j++){
                if(params[2+num_nodes+i] == plan.ODE[j]->encode()){
                    nodes.push_back(Local_Node(plan.ODE[j]));
                }
            }
        }
        /*for(unsigned i = 0 ; i < plan.ODE.size() ;i++){
                nodes.push_back(Local_Node(plan.ODE[i]));
            }*/

        //plan.order_subqueries_for_parallel_execution(nodes, this->predicates_stats);

        total = this->store.evaluate_without_communication(nodes, results, res_node);



        pos.push_back(total);
        plan.get_projection_pos(res_node.row, pos);
        this->communicator->mpi_send_int_vector(pos, 0);
        if(dest_worker == 0)
            this->communicator->sync();
        this->communicator->mpi_send_int_vector(results, 0);

    }
}

void Client::associate_edges_with_replica_index_nodes(Plan *plan){
    if(!isVariable(plan->core)){
        associate_edges_with_replica_index_nodes_const(plan);
    }
    else{
        associate_edges_with_replica_index_nodes_var(plan);
    }
}

void Client::associate_edges_with_replica_index_nodes_const(Plan *plan){
    replica_index_node *tmp_index_node, *parent_index_node;
    bool found, const_core_match = true, var_core_match = true, exact_match, super_set;
    replica_index_t::iterator it;
    boost::unordered_map<string, replica_index_node *> index_nodes_ptrs;
    vector<int> remaining, tmp;
    walk_edge * tmp_edge;
    boost::unordered_map<string, replica_index_node *>::iterator ptrs_it;
    vector<replica_index_node*> const_ptrs, var_ptrs;

    const_ptrs.resize(plan->ODE.size());
    var_ptrs.resize(plan->ODE.size());
    //Working on core incident edges
    for(unsigned i = 0 ; i < plan->ODE.size() ; i++){
        if(plan->ODE[i]->parent == NULL){
            found = false;
            tmp_index_node = new replica_index_node(plan->ODE[i]);
            for(it = this->store.rep_index.begin() ; it != this->store.rep_index.end() ; it++){
                if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && ((*it)->binding == tmp_index_node->binding) && (isVariable((*it)->propagating) && isVariable(tmp_index_node->propagating))){
                    found = true;
                    break;
                }
            }
            if(found){
                index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
                delete tmp_index_node;
                tmp_index_node = *it;
                const_ptrs[i] = tmp_index_node;
            }
            else{
                const_core_match = false;
                break;
            }
        }
        else{
            remaining.push_back(i);
        }
    }
    if(const_core_match){
        while(!remaining.empty()){
            for(unsigned i = 0 ; i < remaining.size(); i++){
                tmp_edge = plan->ODE[remaining[i]];
                ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
                if(ptrs_it != index_nodes_ptrs.end()){
                    parent_index_node = ptrs_it->second;
                    tmp_index_node = new replica_index_node(tmp_edge);
                    if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
                        exact_match = false;
                        super_set = false;
                        for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
                            if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
                                if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
                                    exact_match = true;
                                    delete tmp_index_node;
                                    tmp_index_node = *it;
                                    const_ptrs[remaining[i]] = tmp_index_node;
                                }
                                else if(isVariable((*it)->propagating)){
                                    super_set = true;
                                    delete tmp_index_node;
                                    tmp_index_node = *it;
                                    const_ptrs[remaining[i]] = tmp_index_node;
                                }
                            }
                        }
                        if(!(super_set || exact_match)){
                            remaining.clear();
                            const_core_match= false;
                            break;
                        }
                    }
                    else{
                        it = parent_index_node->children.find(tmp_index_node);
                        if(it == parent_index_node->children.end()){
                            remaining.clear();
                            const_core_match= false;
                            break;
                        }
                        else{
                            index_nodes_ptrs[tmp_edge->encode()] = *it;
                            delete tmp_index_node;
                            tmp_index_node = *it;
                            const_ptrs[remaining[i]] = tmp_index_node;
                        }
                    }
                }
                else{
                    tmp.push_back(remaining[i]);
                }
            }
            remaining.swap(tmp);
        }
    }


    index_nodes_ptrs.clear();
    remaining.clear();
    tmp.clear();

    //VAR CHECK
    for(unsigned i = 0 ; i < plan->ODE.size() ; i++){
        if(plan->ODE[i]->parent == NULL){
            found = false;
            tmp_index_node = new replica_index_node(plan->ODE[i]);
            for(it = this->store.rep_index.begin() ; it != this->store.rep_index.end() ; it++){
                if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding))){
                    if((isVariable((*it)->propagating)) || (tmp_index_node->propagating == (*it)->propagating)){
                        found = true;
                        break;
                    }
                }
            }
            if(found){
                index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
                delete tmp_index_node;
                tmp_index_node = *it;
                var_ptrs[i] = tmp_index_node;
            }
            else{
                var_core_match = false;
                break;
            }
        }
        else{
            remaining.push_back(i);
        }
    }
    if(var_core_match){
        while(!remaining.empty()){
            for(unsigned i = 0 ; i < remaining.size(); i++){
                tmp_edge = plan->ODE[remaining[i]];
                ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
                if(ptrs_it != index_nodes_ptrs.end()){
                    parent_index_node = ptrs_it->second;
                    tmp_index_node = new replica_index_node(tmp_edge);
                    if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
                        exact_match = false;
                        super_set = false;
                        for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
                            if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
                                if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
                                    exact_match = true;
                                    delete tmp_index_node;
                                    tmp_index_node = *it;
                                    var_ptrs[remaining[i]] = tmp_index_node;
                                }
                                else if(isVariable((*it)->propagating)){
                                    super_set = true;
                                    delete tmp_index_node;
                                    tmp_index_node = *it;
                                    var_ptrs[remaining[i]] = tmp_index_node;
                                }
                            }
                        }
                        if(!(super_set||exact_match)){
                            remaining.clear();
                            var_core_match= false;
                            break;
                        }
                    }
                    else{
                        it = parent_index_node->children.find(tmp_index_node);
                        if(it == parent_index_node->children.end()){
                            remaining.clear();
                            var_core_match = false;
                            break;
                        }
                        else{
                            index_nodes_ptrs[tmp_edge->encode()] = *it;
                            delete tmp_index_node;
                            tmp_index_node = *it;
                            var_ptrs[remaining[i]] = tmp_index_node;
                        }
                    }
                }
                else{
                    tmp.push_back(remaining[i]);
                }
            }
            remaining.swap(tmp);
        }
    }

    if(var_core_match){
        for(unsigned i = 0 ; i < plan->ODE.size() ;i++){
            plan->ODE[i]->rin = var_ptrs[i];
        }
    }
    else if(const_core_match){
        for(unsigned i = 0 ; i < plan->ODE.size() ;i++){
            plan->ODE[i]->rin = const_ptrs[i];
        }
    }
    else
        throwException("COULD NOT MATCH QUERY IN REPLICA INDEX 1");
}

void Client::associate_edges_with_replica_index_nodes_var(Plan *plan){
    replica_index_node *tmp_index_node, *parent_index_node;
    bool exact_match, super_set;
    replica_index_t::iterator it;
    boost::unordered_map<string, replica_index_node *> index_nodes_ptrs;
    vector<int> remaining, tmp;
    walk_edge * tmp_edge;
    boost::unordered_map<string, replica_index_node *>::iterator ptrs_it;

    //Working on core incident edges
    for(unsigned i = 0 ; i < plan->ODE.size() ; i++){
        if(plan->ODE[i]->parent == NULL){
            tmp_index_node = new replica_index_node(plan->ODE[i]);
            it = this->store.rep_index.find(tmp_index_node);
            if(it == this->store.rep_index.end()){
                throwException("COULD NOT MATCH QUERY IN REPLICA INDEX 2");
            }
            else{
                index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
                delete tmp_index_node;
                tmp_index_node = *it;
                plan->ODE[i]->rin = tmp_index_node;
            }
        }
        else{
            remaining.push_back(i);
        }
    }

    while(!remaining.empty()){
        for(unsigned i = 0 ; i < remaining.size() ; i++){
            tmp_edge = plan->ODE[remaining[i]];
            ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
            if(ptrs_it != index_nodes_ptrs.end()){
                parent_index_node = ptrs_it->second;
                tmp_index_node = new replica_index_node(tmp_edge);
                if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
                    exact_match = false;
                    super_set = false;
                    for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
                        if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
                            if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
                                exact_match = true;
                                delete tmp_index_node;
                                tmp_index_node = *it;
                                plan->ODE[remaining[i]]->rin = tmp_index_node;
                            }
                            else if(isVariable((*it)->propagating)){
                                super_set = true;
                                delete tmp_index_node;
                                tmp_index_node = *it;
                                plan->ODE[remaining[i]]->rin = tmp_index_node;
                            }
                        }
                    }
                    if(!(super_set||exact_match)){
                        throwException("COULD NOT MATCH QUERY IN REPLICA INDEX 3");
                    }
                }
                else{
                    it = parent_index_node->children.find(tmp_index_node);
                    if(it == parent_index_node->children.end()){
                        throwException("COULD NOT MATCH QUERY IN REPLICA INDEX 4");
                    }
                    else{
                        index_nodes_ptrs[tmp_edge->encode()] = *it;
                        delete tmp_index_node;
                        tmp_index_node = *it;
                        plan->ODE[remaining[i]]->rin = tmp_index_node;
                    }
                }
            }
            else{
                tmp.push_back(remaining[i]);
            }
        }
        remaining.swap(tmp);
    }
}

void Client::evict(vector<string> &params){
    walk_edge* edge, *parent = NULL, *root = NULL;
    vector<string> splits;
    string core;
    replica_index_node * tmp_rep_node, *parent_rep_node;
    replica_index_t::iterator it;

    if(params.size() >= 2){
        split_string(params[params.size()-1], "|", splits);
        parent = new walk_edge(splits[0], splits[2], splits[1]);
        parent->nodes_reversed = atoi(params[params.size()-2].c_str());
        root = parent;
        core = root->n1.label;
    }
    for(unsigned i = params.size()-2 ; i > 0 ; i-=2){
        split_string(params[i-1], "|", splits);
        edge = new walk_edge(splits[0], splits[2], splits[1]);
        edge->nodes_reversed = atoi(params[i-2].c_str());
        edge->parent = parent;
        parent->children.push_back(edge);
        parent = edge;
    }

    //if the core is constant we do exact match
    if(!isVariable(core)){
        /*found = false;
        tmp_rep_node = new replica_index_node(root);
        for(it = this->store.rep_index.begin() ; it != this->store.rep_index.end() ; it++){
            if(rank==1)
                cout<<(*it)->print()<<endl;
            if(((*it)->predicate == tmp_rep_node->predicate) && ((*it)->reversed == tmp_rep_node->reversed) && ((*it)->binding == tmp_rep_node->binding) && (isVariable((*it)->propagating) && isVariable(tmp_rep_node->propagating))){
                found = true;
                break;
            }
        }
        if(found){
            delete tmp_rep_node;
            tmp_rep_node = *it;
        }
        else{
            delete tmp_rep_node;
            throwException("EVICTION: CANNOT FIND REPLICA INDEX NODE 1");
        }*/
        tmp_rep_node = new replica_index_node(root, true);
        it = this->store.rep_index.find(tmp_rep_node);
        if(it == this->store.rep_index.end()){
            delete tmp_rep_node;
            throwException("EVICTION: CANNOT FIND REPLICA INDEX NODE 1");
        }
        else{
            delete tmp_rep_node;
            tmp_rep_node = *it;
        }

    }
    else{
        tmp_rep_node = new replica_index_node(root, true);
        it = this->store.rep_index.find(tmp_rep_node);
        if(it == this->store.rep_index.end()){
            delete tmp_rep_node;
            throwException("EVICTION: CANNOT FIND REPLICA INDEX NODE 2");
        }
        else{
            delete tmp_rep_node;
            tmp_rep_node = *it;
        }
    }

    parent = root;
    parent_rep_node = tmp_rep_node;

    while(!parent->children.empty()){
        edge = parent->children[0];
        tmp_rep_node = new replica_index_node(edge, true);
        /*for(it = parent_rep_node->children.begin() ; it != parent_rep_node->children.end() ; it++){
            if(((*it)->predicate == tmp_rep_node->predicate) && ((*it)->reversed == tmp_rep_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_rep_node->binding))){
                if(isVariable(tmp_rep_node->propagating) && isVariable((*it)->propagating)){
                    found = true;
                    break;
                }
                else if(tmp_rep_node->propagating == (*it)->propagating){
                    found = true;
                    break;
                }
            }
        }
        if(found){
            delete tmp_rep_node;
            parent_rep_node = *it;
            parent = edge;
        }
        else{
            delete tmp_rep_node;
            throwException("EVICTION: CANNOT FIND REPLICA INDEX NODE 3");
        }*/
        it = parent_rep_node->children.find(tmp_rep_node);
        if(it == parent_rep_node->children.end()){
            delete tmp_rep_node;
            throwException("EVICTION: CANNOT FIND REPLICA INDEX NODE 3");
        }
        else{
            delete tmp_rep_node;
            parent_rep_node = *it;
            parent = edge;
        }
    }
    tmp_rep_node = parent_rep_node;

    params.clear();

    if(tmp_rep_node->parent == NULL){//Core incident edge
        if(tmp_rep_node->reversed){//delete index data
            params.push_back(toString(tmp_rep_node->index->records_count));
            delete tmp_rep_node->index;
            tmp_rep_node->exact = true;
            this->store.rep_index.erase(tmp_rep_node);
            /*unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal> tmp_set;
            for(unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal>::iterator it = this->store.rep_index.begin(); it != this->store.rep_index.end(); it++){
                if((*it) != tmp_rep_node){
                    tmp_set.insert(*it);
                }
            }
            this->store.rep_index = tmp_set;*/
            delete tmp_rep_node;
        }
        else{//only delete the node (no data deletion
            tmp_rep_node->exact = true;
            this->store.rep_index.erase(tmp_rep_node);
            /*unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal> tmp_set;
            for(unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal>::iterator it = this->store.rep_index.begin(); it != this->store.rep_index.end(); it++){
                if((*it) != tmp_rep_node){
                    tmp_set.insert(*it);
                }
            }
            this->store.rep_index = tmp_set;*/
            delete tmp_rep_node;
        }
    }
    else{//internal leaf node
        params.push_back(toString(tmp_rep_node->index->records_count));
        delete tmp_rep_node->index;
        tmp_rep_node->exact = true;
        tmp_rep_node->parent->children.erase(tmp_rep_node);
        /*unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal> tmp_set;
        for(unordered_set<replica_index_node*, replica_index_node_hash, replica_index_node_equal>::iterator it = tmp_rep_node->parent->children.begin(); it != tmp_rep_node->parent->children.end(); it++){
            if((*it) != tmp_rep_node){
                tmp_set.insert(*it);
            }
        }
        tmp_rep_node->parent->children = tmp_set;*/
        delete tmp_rep_node;
    }
    this->communicator->send_command(CMD_READY, params, 0);
}

void Client::get_communication_volume(vector<string> &params){
    params.clear();
    params.push_back(toString(this->communicator->comm_volume));
    this->communicator->send_command(CMD_READY, params, 0);
}
