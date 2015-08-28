#include "engine/plan.h"
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
Plan::Plan(MasterGUI* master, Query q, bool adaptivity_enabled) {
    query = q;
    this->adaptivity_enabled = adaptivity_enabled;
    this->master = master;
    encode_subqueries();
    generatePlan();
}

Plan::Plan(vector<string> &params, bool redistribution) {
    if(redistribution){
        this->plan_for_redistribution(params);
    }
    else{
        this->plan_for_parallel_execution(params);
    }

    /*if(MPI::COMM_WORLD.Get_rank() == 1){
        cout<<"CORE3: "<<this->core<<endl;
        for(unsigned i = 0 ; i < ODE.size(); i++){
            cout<<ODE[i]->print2()<<" "<<ODE[i]->nodes_reversed<<endl;
        }
    }*/
}

Plan::Plan(vector<string> &params){
    int num_nodes;

    num_nodes = atoi(&params[0][0]);
    for(int i = 1 ; i <= num_nodes ;i++){
        this->query.nodes.push_back(Node(params[i]));
    }
    for(unsigned i = num_nodes+1 ; i < params.size() ;i++){
        this->query.projections.push_back(params[i]);
    }
}

Plan::~Plan() {
    for(unsigned i = 0 ; i < ODE.size() ; i++){
        delete ODE[i];
    }
}

void Plan::encode_subqueries(){
    int id;
    boost::unordered_map<string, int>::iterator it;
    for(unsigned int i = 0 ; i < query.string_nodes.size(); i++){
        query.nodes.push_back(query.string_nodes[i]);
        if(!isVariable(query.nodes[i].row[0])){
            id = b_search(master->verts_data, query.nodes[i].row[0]);
            if(id == -1)
                id = master->manager->max_vertex_id+1;
            query.nodes[i].row[0] = toString(id);
        }
        if(!isVariable(query.nodes[i].row[1])){
            it = master->preds_map.find(query.nodes[i].row[1]);
            if(it == master->preds_map.end())
                id = master->manager->max_predicate_id+1;
            else
                id = it->second;
            query.nodes[i].row[1] = toString(id);
        }
        if(!isVariable(query.nodes[i].row[2])){
            id = b_search(master->verts_data, query.nodes[i].row[2]);
            if(id == -1)
                id = master->manager->max_vertex_id+1;
            query.nodes[i].row[2] = toString(id);
        }
    }
}

void Plan::plan_for_redistribution(vector<string> &params){
    unsigned index = 0;
    boost::unordered_map<string, walk_edge *> edge_ptrs;
    boost::unordered_map<string, walk_edge *>::iterator it;
    walk_edge * tmp_edge, *parent = NULL;
    vector<string> splits;
    while(true){
        if(params[index] == "EOP"){
            if(index == params.size()-1){
                break;
            }
            else{
                parent = NULL;
            }
        }
        else{
            split_string(params[index], intermediate_delim, splits);
            tmp_edge = new walk_edge(splits[1], splits[2], splits[3], atoi(splits[0].c_str()), parent);
            it = edge_ptrs.find(tmp_edge->encode());
            if(it == edge_ptrs.end()){
                this->ODE.push_back(tmp_edge);
                edge_ptrs[tmp_edge->encode()] = tmp_edge;
                if(parent != NULL)
                    parent->children.push_back(tmp_edge);
                parent = tmp_edge;
            }
            else{
                parent = it->second;
                delete tmp_edge;
            }
        }
        index++;
    }

    /*if(MPI::COMM_WORLD.Get_rank() ==1){
        cout<<"ODE SIZE: "<<ODE.size()<<endl;
        for(unsigned i = 0 ; i < this->ODE.size(); i++){
            if(ODE[i]->children.size() == 0){
                cout<<ODE[i]->print2()<<endl;
            }
        }
    }*/
}

void Plan::plan_for_parallel_execution(vector<string> &params){
    int num_nodes, num_scores, limit;
    float max_score = -1;
    Node tmp_node;
    map<string, float>::iterator it;

    num_nodes = atoi(&params[1][0]);
    for(int i = 2 ; i < num_nodes+2 ; i++){
        tmp_node = Node(params[i]);
        query.nodes.push_back(tmp_node);
    }

    num_scores = atoi(&params[2*num_nodes + 2][0]);
    limit = 2*(num_nodes+num_scores) + 3;
    for (int i = 2*num_nodes + 3; i < limit; i += 2) {
        walk_scores[params[i]] = atof(&params[i + 1][0]);
    }
    for (unsigned i = limit; i < params.size(); i++) {
        this->query.projections.push_back(params[i]);
    }
    for(it = walk_scores.begin(); it != walk_scores.end(); it++){
        if(it->second > max_score){
            max_score = it->second;
            this->core = it->first;
        }
    }
    //cout<<this->core<<endl;
    this->master = NULL;
    this->walk_query();
}

void Plan::generatePlan(){
    if(this->adaptivity_enabled)
        this->find_plan_type();
    else
        this->operation = this->is_star() ? STAR:SEMI_JOIN;

    if(this->operation == SEMI_JOIN){
        plan_semi_join();
    }
}

void Plan::plan_semi_join(){
    this->update_supqueries_cardinality();
    if(this->operation == EMPTY){
        return;
    }
#ifdef DEBUG
    for(unsigned i = 0 ; i < query.nodes.size(); i++)
        this->ordered_subqueries.push_back(query.nodes[i]);
#else
    this->order_subqueries();
#endif
    this->initial_vertex = this->ordered_subqueries[0].row[0];
}

bool Plan::is_empty(){
    int tmp;
    for(unsigned i = 0 ; i < query.nodes.size() ; i++){
        if(!isVariable(query.nodes[i].row[1])){
            tmp = atoi(query.nodes[i].row[1].c_str());
            if(tmp > this->master->manager->max_predicate_id){
                return true;
            }
        }
        else{
            throwException("Currently we don't support unbounded predicate!");
            return true;
        }
        if(!isVariable(query.nodes[i].row[0])){
            tmp = atoi(query.nodes[i].row[0].c_str());
            if(tmp > this->master->manager->max_vertex_id){
                return true;
            }
        }
        if(!isVariable(query.nodes[i].row[2])){
            tmp = atoi(query.nodes[i].row[2].c_str());
            if(tmp > this->master->manager->max_vertex_id){
                return true;
            }
        }
    }
    return false;
}

bool Plan::is_star(){
    string first = this->query.nodes[0].row[0];
    for(unsigned i = 1 ; i < this->query.nodes.size(); i++){
        if(this->query.nodes[i].row[0] != first){
            return false;
        }
    }
    return true;
}

void Plan::find_plan_type(){
    if(is_empty()){
        this->operation = EMPTY;
        return;
    }
    bool parallel;
    string tmp_core= "";

    this->operation = this->is_star() ? STAR:SEMI_JOIN;
    if(this->operation == SEMI_JOIN){
        compute_walk_scores();
        walk_query();
        parallel = this->master->q_index->is_parallel_query(this);
        if(parallel){
            this->operation = PARALLEL;
            this->master->q_index->update_time_stamps(this->leaves);
        }
        else{
            this->master->q_index->check_for_frequency(this);

            for(unsigned i = 0 ; i < ODE.size(); i++){
                //cout<<ODE[i]->print2()<<endl;
                if(ODE[i]->parent == NULL){
                    if(tmp_core == "")
                        tmp_core = ODE[i]->n1.label;
                    else if(ODE[i]->n1.label != tmp_core){
                        //throwException("NONEMATCHING CORE!!");
                        master->logger.writeToLog("NONEMATCHING CORE!!", true);
                    }
                }
                this->core = tmp_core;
            }
        }
        /*cout<<"CORE: "<<this->core<<endl;
        for(unsigned i = 0 ; i < ODE.size(); i++){
            cout<<ODE[i]->print2()<<" "<<ODE[i]->nodes_reversed<<endl;
        }*/
    }
}

void Plan::order_subqueries(){
    this->order_subqueries_for_semi_join();
}

void Plan::update_supqueries_cardinality(){
    for(unsigned i = 0 ; i < this->query.nodes.size(); i++){
        if(this->query.nodes[i].variables != V_BOTH)
            update_subquery_cardinality(this->query.nodes[i]);
        else
            this->query.nodes[i].cardinality = master->manager->predicates_stats[toString(this->query.nodes[i].row[1])].total_count;

        if(this->query.nodes[i].cardinality == 0){
            this->operation = EMPTY;
            break;
        }
    }
}

void Plan::update_subquery_cardinality(Node &subquery){
    vector<string> cmd_params;
    long long cardinality;
    string encoded_subquery = subquery.encode_query();

    master->logger.writeToLog(part_string, false);
    master->logger.writeToLog("Adding parts for query: " + encoded_subquery, false);
    cmd_params.push_back(encoded_subquery);

    this->master->communicator->send_command(CMD_GET_SUBQUERY_CARDINALITY, cmd_params, SEND_TO_ALL);

    for (int i = 1; i < master->communicator->num_workers; ++i) {
        cardinality = this->master->communicator->mpi_receive_long(i);
        subquery.cardinality += cardinality;
    }
    master->logger.writeToLog("|"+subquery.encode_query()+"| ---> "+toString(subquery.cardinality)+" "+toString(subquery.variables), false);
}

void Plan::order_subqueries_for_parallel_execution(vector<Local_Node> &nodes, boost::unordered_map<string, predicate_stat>&stat_table){
    boost::unordered_map<string, long long> var_min_binding;
    boost::unordered_map<string, long long>::iterator it;
    sorted_pairs_desc_t sp;
    string current_var;
    sorted_int_pairs_t sp2;
    vector<Local_Node> newOrder;
    set<string> can_join_with;
    pair<string, long long> current_pair;
    vector<pair<string, long long> > delayed;
    set<int> finished;
    predicate_stat stat;
    dynamic_state d_state;
    set<string> started_from;
    set<int> remaining;

    for(unsigned i = 0 ; i < nodes.size() ; i++){
        if(isVariable(nodes[i].row[0])){
            it = var_min_binding.find(nodes[i].row[0]);
            if(it == var_min_binding.end()){
                var_min_binding[nodes[i].row[0]] = nodes[i].num_subj;
            }
            else{
                if(nodes[i].num_subj < it->second)
                    it->second = nodes[i].num_subj;
            }
        }
        if(isVariable(nodes[i].row[2])){
            it = var_min_binding.find(nodes[i].row[2]);
            if(it == var_min_binding.end()){
                var_min_binding[nodes[i].row[2]] = nodes[i].num_obj;
            }
            else{
                if(nodes[i].num_obj < it->second)
                    it->second = nodes[i].num_obj;
            }
        }
    }

    for(it = var_min_binding.begin(); it != var_min_binding.end() ; it++)
        sp.push(make_pair(it->first, it->second));

    bool first = true;
    while(!sp.empty()){
        if(!first){
            while(true){
                current_pair = sp.top();
                current_var = current_pair.first;
                sp.pop();
                if(can_join_with.find(current_var) != can_join_with.end())
                    break;
                else
                    delayed.push_back(current_pair);
            }
        }
        else{
            first = false;
            current_pair = sp.top();
            current_var = current_pair.first;
            sp.pop();
        }
        for(unsigned i = 0 ; i < nodes.size() ; i++){
            if(finished.find(i) == finished.end()){
                if(nodes[i].row[0] == current_var){
                    //sp2.push(make_pair(i, nodes[i].cardinality));
                    sp2.push(make_pair(i, nodes[i].num_subj));
                    if(isVariable(nodes[i].row[2]))
                        can_join_with.insert(nodes[i].row[2]);
                    finished.insert(i);
                }
                else if(nodes[i].row[2] == current_var){
                    sp2.push(make_pair(i, nodes[i].num_obj));
                    if(isVariable(nodes[i].row[0]))
                        can_join_with.insert(nodes[i].row[0]);
                    finished.insert(i);
                }
            }
        }
        while(!sp2.empty()){
            newOrder.push_back(nodes[sp2.top().first]);
            sp2.pop();
        }
        for(unsigned i = 0 ; i < delayed.size() ; i++){
            sp.push(delayed[i]);
        }
        delayed.clear();
    }

    /*for(unsigned i = 0 ; i < nodes.size(); i++){
        if(MPI::COMM_WORLD.Get_rank() == 1)
            cout<<nodes[i].print2()<<endl;
    }*/

    for(unsigned i = 0 ; i < nodes.size(); i++){
        if(started_from.find(nodes[i].row[0]) == started_from.end()){
            started_from.insert(nodes[i].row[0]);
            remaining.clear();
            for(unsigned j = 0 ; j < nodes.size() ;j++){
                if(i!=j)
                    remaining.insert(j);
            }
            d_state = dynamic_state(this->query.variables);
            d_state.communication = 0;
            d_state.cost = 0;
            d_state.res = nodes[i];
            d_state.addNode(i);
            stat = stat_table[nodes[i].row[1]];
            if(nodes[i].variables == V_BOTH){
                d_state.cardinality = nodes[i].cardinality;
                d_state.cum_cardinality = d_state.cardinality;
                d_state.variables_bindings[nodes[i].row[0]] = nodes[i].num_subj;
                d_state.variables_bindings[nodes[i].row[2]] = nodes[i].num_obj;
            }
            else if(nodes[i].variables == V_SUBJ){
                d_state.cardinality = nodes[i].cardinality;
                d_state.cum_cardinality = d_state.cardinality;
                d_state.variables_bindings[nodes[i].row[0]] = nodes[i].num_subj;
            }
            else{
                d_state.cardinality = nodes[i].cardinality;
                d_state.cum_cardinality = d_state.cardinality;
                d_state.variables_bindings[nodes[i].row[2]] = nodes[i].num_obj;
            }
            this->order_table[toString(i)] = d_state;
            for(set<int>::iterator it = remaining.begin() ; it != remaining.end() ;it++){
                this->branch_local(nodes, toString(i), *it, remaining, stat_table);
            }
        }
        //break;
    }


    string order_string;
    for(unsigned i = 0 ; i < this->query.nodes.size()-1 ; i++){
        order_string += toString(i)+"|";
    }
    order_string += toString(this->query.nodes.size()-1);
    for(unsigned i = 0 ; i < this->query.nodes.size() ; i++){
        newOrder[i] = nodes[this->order_table[order_string].ordered_nodes[i]];
    }




    bool debug = true;
    for(unsigned i = 0 ; i < nodes.size() ; i++){
        /*if(MPI::COMM_WORLD.Get_rank() ==1)
            cout<<newOrder[i].print()<<endl;*/
        nodes[i] = newOrder[i];
        if(!debug)
            nodes[i] = newOrder[i];
        else{
            if(newOrder[i].print2() == "?x|4|24")
                nodes[0] = newOrder[i];
            if(newOrder[i].print2() == "?x|0|?y")
                nodes[1] = newOrder[i];
            if(newOrder[i].print2() == "?x|8|?z")
                nodes[2] = newOrder[i];
            if(newOrder[i].print2() == "?z|4|22638")
                nodes[3] = newOrder[i];
            if(newOrder[i].print2() == "?y|4|8622222")
                nodes[4] = newOrder[i];
            if(newOrder[i].print2() == "?z|9|?y")
                nodes[5] = newOrder[i];
        }
    }
}

void Plan::order_subqueries_for_semi_join(){
    vector<int> remaining;
    boost::unordered_map<string, int> var_subj_count;
    boost::unordered_map<string, int> var_subj_obj_count;
    boost::unordered_map<string, int>::iterator it;
    vector<Node> newOrder;
    sorted_pairs_t sp;
    sorted_int_pairs_t sp2, sp3;
    string current_var;
    predicate_stat stat;
    dynamic_state d_state;
    set<string> started_from;
    bool subj_obj_star = false;

    for(unsigned i = 0 ; i < this->query.nodes.size() ; i++){
        it = var_subj_count.find(this->query.nodes[i].row[0]);
        if(it == var_subj_count.end()){
            var_subj_count[this->query.nodes[i].row[0]] = 0;
            it = var_subj_count.find(this->query.nodes[i].row[0]);
        }
        it->second = it->second+1;

        it = var_subj_obj_count.find(this->query.nodes[i].row[0]);
        if(it == var_subj_obj_count.end()){
            var_subj_obj_count[this->query.nodes[i].row[0]] = 0;
            it = var_subj_obj_count.find(this->query.nodes[i].row[0]);
        }
        it->second = it->second+1;

        it = var_subj_obj_count.find(this->query.nodes[i].row[2]);
        if(it == var_subj_obj_count.end()){
            var_subj_obj_count[this->query.nodes[i].row[2]] = 0;
            it = var_subj_obj_count.find(this->query.nodes[i].row[2]);
        }
        it->second = it->second+1;
    }
    for(it = var_subj_obj_count.begin() ; it != var_subj_obj_count.end() ; it++){
        if(it->second == (signed)query.nodes.size())
            subj_obj_star = true;
    }

    for(it = var_subj_count.begin(); it != var_subj_count.end() ; it++)
        sp.push(make_pair(it->first, it->second));

    while(!sp.empty()){
        current_var = sp.top().first;
        sp.pop();
        for(unsigned i = 0 ; i < this->query.nodes.size() ; i++){
            if(this->query.nodes[i].row[0] == current_var){
                //newOrder.push_back(this->query.nodes[i]);
                sp2.push(make_pair(i, this->query.nodes[i].cardinality));
            }
        }
        while(!sp2.empty()){
            newOrder.push_back(this->query.nodes[sp2.top().first]);
            sp2.pop();
        }
    }
    for(unsigned i = 0 ; i < newOrder.size() ; i++){
        this->query.nodes[i] = newOrder[i];
        //cout<<this->query.nodes[i].encode_query()<<"--->"<<this->query.nodes[i].cardinality<<endl;
    }


    if(subj_obj_star){
        //cout<<"SUBJECT OBJECT STAR"<<endl;
        //this->ordered_subqueries.push_back(this->query.nodes[0]);
        for(unsigned j = 0 ; j < this->query.nodes.size() ;j++){
            sp3.push(make_pair(j, this->query.nodes[j].cardinality));
        }
        while(!sp3.empty()){
            this->ordered_subqueries.push_back(this->query.nodes[sp3.top().first]);
            sp3.pop();
        }
    }
    else{
        /*for(unsigned i = 0 ; i < this->query.nodes.size(); i++){
            cout<<i<<"--->"<<this->query.nodes[i].encode_query()<<endl;
        }*/
        for(unsigned i = 0 ; i < this->query.nodes.size(); i++){
            if(started_from.find(this->query.nodes[i].row[0]) == started_from.end()){
                started_from.insert(this->query.nodes[i].row[0]);
                remaining.clear();
                for(unsigned j = 0 ; j < this->query.nodes.size() ;j++){
                    if(i!=j)
                        remaining.push_back(j);
                }
                d_state = dynamic_state(this->query.variables);
                d_state.res = this->query.nodes[i];
                d_state.addNode(i);
                stat = this->master->manager->predicates_stats[this->query.nodes[i].row[1]];
                if(this->query.nodes[i].variables == V_BOTH){
                    d_state.cardinality = stat.total_count;
                    d_state.cum_cardinality = d_state.cardinality;
                    //d_state.cost = d_state.cardinality;
                    d_state.communication = 0;
                    d_state.variables_bindings[this->query.nodes[i].row[0]] = stat.subject_uniques;
                    d_state.variables_bindings[this->query.nodes[i].row[2]] = stat.object_uniques;
                }
                else if(this->query.nodes[i].variables == V_SUBJ){
                    d_state.cardinality = this->query.nodes[i].cardinality;
                    d_state.cum_cardinality = d_state.cardinality;
                    //d_state.cost = d_state.cardinality;
                    d_state.communication = 0;
                    //d_state.variables_bindings[this->query.nodes[i].row[0]] = ((float)this->query.nodes[i].cardinality)/stat.pred_per_subj;
                    d_state.variables_bindings[this->query.nodes[i].row[0]] = this->query.nodes[i].cardinality;
                }
                else{
                    d_state.cardinality = this->query.nodes[i].cardinality;
                    d_state.cum_cardinality = d_state.cardinality;
                    //d_state.cost = d_state.cardinality;
                    d_state.communication = 0;
                    //d_state.variables_bindings[this->query.nodes[i].row[2]] = ((float)this->query.nodes[i].cardinality)/stat.pred_per_obj;
                    d_state.variables_bindings[this->query.nodes[i].row[2]] = this->query.nodes[i].cardinality;
                }
                this->order_table[toString(i)] = d_state;
                //cout<<part_string<<"\n"<<d_state.print()<<endl;
                //if(i == 2)

                for(unsigned it = 0 ; it != remaining.size() ;it++){
                    //cout<<part_string<<"\n"<<toString(i)<<"--->"<<this->order_table[toString(i)].res.encode_query()<<" "<<remaining.size()<<endl;
                    this->branch(toString(i), remaining[it], remaining);
                }
            }
            //break;
        }
        string order_string;
        //cout<<"BEFORE: "<<endl;
        for(unsigned i = 0 ; i < this->query.nodes.size()-1 ; i++){
            order_string += toString(i)+"|";
            //cout<<this->query.nodes[i].encode_query()<<endl;
        }
        //cout<<"AFTER: "<<endl;
        order_string += toString(this->query.nodes.size()-1);
        //cout<<print_list(this->order_table[order_string].ordered_nodes, "|")<<" "<<this->order_table[order_string].communication<<endl;
        //cout<<order_string<<" "<<order_table.size()<<endl;
        for(unsigned i = 0 ; i < this->query.nodes.size() ; i++){
            this->ordered_subqueries.push_back(this->query.nodes[this->order_table[order_string].ordered_nodes[i]]);
            //cout<<this->query.nodes[this->order_table[order_string].ordered_nodes[i]].encode_query()<<endl;
        }
    }
    /*for(unsigned i = 0 ; i < this->query.nodes.size(); i++){
        this->ordered_subqueries.push_back(this->query.nodes[i]);
    }*/
}

void Plan::branch_local(vector<Local_Node> &nodes,string state_string, int next, set<int> remaining, boost::unordered_map<string, predicate_stat> & stat_table){
    Node tmp_res;
    vector<pair<int,int> > join_cols;
    dynamic_state next_state = this->order_table[state_string];
    dynamic_state old_state = this->order_table[state_string];
    string new_state_string;
    vector<int> onv;
    predicate_stat stat;
    next_state.res.get_join_node_and_cols(nodes[next], join_cols, tmp_res);
    stat = stat_table[nodes[next].row[1]];

    remaining.erase(next);
    if(join_cols.size() != 0){
        onv.insert(onv.end(), next_state.ordered_nodes.begin(), next_state.ordered_nodes.end());
        onv.push_back(next);
        sort(onv.begin(), onv.end());
        new_state_string = print_list(onv, "|");
        next_state.addNode(next);
        next_state.res = tmp_res;
        if(old_state.ordered_nodes.size() == 1){
            if(nodes[old_state.ordered_nodes[0]].variables != V_BOTH && nodes[next].variables != V_BOTH){
                next_state.cardinality = min(old_state.cardinality, (float)nodes[next].cardinality);
                next_state.cum_cardinality += next_state.cardinality;

            }
            else if(nodes[old_state.ordered_nodes[0]].variables != V_BOTH){
                next_state.cardinality = max(old_state.cardinality, (float)nodes[next].cardinality);
                next_state.cum_cardinality += next_state.cardinality;
            }
            else if(nodes[next].variables != V_BOTH){
                next_state.cardinality = old_state.cardinality;
                next_state.cum_cardinality += next_state.cardinality;
            }
        }
        else{
            if(nodes[next].variables != V_BOTH){
                next_state.cardinality = old_state.cardinality;
                next_state.cum_cardinality += next_state.cardinality;
            }
            else{
                if(join_cols[0].second == 0){
                    next_state.cardinality = old_state.cardinality * stat.pred_per_subj;
                    next_state.cum_cardinality += next_state.cardinality;
                }
                else{
                    next_state.cardinality = old_state.cardinality * stat.pred_per_obj;
                    next_state.cum_cardinality += next_state.cardinality;
                }
            }
        }
        if(nodes[next].variables == V_BOTH){
            next_state.variables_bindings[nodes[next].row[0]] = min(old_state.variables_bindings[nodes[next].row[0]], (float)nodes[next].num_subj);
            next_state.variables_bindings[nodes[next].row[2]] = min(old_state.variables_bindings[nodes[next].row[2]], (float)nodes[next].num_obj);
        }
        else if(nodes[next].variables == V_SUBJ){
            next_state.variables_bindings[nodes[next].row[0]] = min(old_state.variables_bindings[nodes[next].row[0]], (float)nodes[next].num_subj);
        }
        else{
            next_state.variables_bindings[nodes[next].row[2]] = min(old_state.variables_bindings[nodes[next].row[2]], (float)nodes[next].num_obj);
        }
        if(this->order_table.find(new_state_string) == this->order_table.end()){
            //cout<<part_string<<"\nADDING FOR FIRST TIME\n"<<next_state.print()<<endl;
            this->order_table[new_state_string]= next_state;
        }
        else{
            if(this->order_table[new_state_string].cum_cardinality > next_state.cum_cardinality){
                //cout<<part_string<<"\nFOUND CHEAPER STATE\n"<<this->order_table[new_state_string].print()<<"\n"<<part_string<<"\n"<<next_state.print()<<endl;
                this->order_table[new_state_string] = next_state;
            }
            else if(this->order_table[new_state_string].cum_cardinality == next_state.cum_cardinality){
                //cout<<part_string<<"\nEQUAL\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
            }
            else{
                //cout<<part_string<<"\nGREATER\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
                return;
            }
        }

        for(set<int>::iterator it = remaining.begin() ; it != remaining.end() ;it++)
            this->branch_local(nodes, new_state_string, *it, remaining, stat_table);
    }
    else{//cannot join
        return;
    }

}

void Plan::branch(string state_string, int next, vector<int> remaining){
    Node tmp_res;
    vector<pair<int,int> > join_cols;
    dynamic_state next_state = this->order_table[state_string];
    dynamic_state old_state = this->order_table[state_string];
    string new_state_string;
    vector<int> onv, tmp_vec;
    predicate_stat stat;
    next_state.res.get_join_node_and_cols(this->query.nodes[next], join_cols, tmp_res);
    stat = this->master->manager->predicates_stats[this->query.nodes[next].row[1]];

    for(unsigned i = 0 ; i < remaining.size();i++){
        if(remaining[i] != next)
            tmp_vec.push_back(remaining[i]);
    }
    remaining.swap(tmp_vec);
    //remaining.erase(next);
    if(join_cols.size() != 0){
        //cout<<this->query.nodes[next].encode_query()<<" "<<old_state.print()<<endl<<part_string<<endl;
        /*if((old_state.ordered_nodes.size() == 1) && (join_cols[0].second == 0) && (this->query.nodes[old_state.ordered_nodes[0]].row[0] == this->query.nodes[next].row[join_cols[0].second])){
            if((this->query.nodes[next].cardinality <= old_state.res.cardinality)){
                //cout<<"Pruning "<<state_string<<" "<<this->order_table[state_string].res.encode_query()<<" && "<<this->query.nodes[next].encode_query()<<endl;
                return;
            }
            else if((this->query.nodes[next].cardinality == old_state.res.cardinality) && (next > old_state.ordered_nodes[0])){
                return;
            }
        }*/

        onv.insert(onv.end(), next_state.ordered_nodes.begin(), next_state.ordered_nodes.end());
        onv.push_back(next);
        sort(onv.begin(), onv.end());
        new_state_string = print_list(onv, "|");
        next_state.addNode(next);
        next_state.res = tmp_res;

        //Computing cost
        if(join_cols[0].second == 0){
            if((this->query.nodes[old_state.ordered_nodes[0]].row[0] == this->query.nodes[next].row[join_cols[0].second])){
                next_state.communication = old_state.communication;
            }
            else{
                if(this->query.nodes[next].variables == V_BOTH){
                    next_state.communication += old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]] + (2*old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]]*stat.pred_per_subj);
                }
                else{
                    next_state.communication += old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]] + (old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]]*stat.pred_per_subj);
                }
            }
            if(this->query.nodes[next].variables == V_BOTH){
                next_state.cardinality = old_state.cardinality * stat.pred_per_subj;
                next_state.cum_cardinality += next_state.cardinality;
            }
            else{
                next_state.cardinality = old_state.cardinality;
                next_state.cum_cardinality += next_state.cardinality;
            }
        }
        else{
            if(this->query.nodes[next].variables == V_BOTH){
                next_state.communication += (old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]] * (master->communicator->num_workers-1)) + (2* (master->communicator->num_workers-1)*old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]]*stat.pred_per_obj);
            }
            else{
                next_state.communication += (old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]] * (master->communicator->num_workers-1)) + ((master->communicator->num_workers-1)*old_state.variables_bindings[this->query.nodes[next].row[join_cols[0].second]]*stat.pred_per_obj);
            }
            if(this->query.nodes[next].variables == V_BOTH){
                next_state.cardinality = old_state.cardinality * stat.pred_per_obj;
                next_state.cum_cardinality += next_state.cardinality;
            }
            else{
                next_state.cardinality = old_state.cardinality;
                next_state.cum_cardinality += next_state.cardinality;
            }
        }

        if(this->query.nodes[next].variables == V_BOTH){
            if(join_cols[0].second == 0){
                next_state.variables_bindings[this->query.nodes[next].row[0]] = min(old_state.variables_bindings[this->query.nodes[next].row[0]], (float)stat.subject_uniques);
                next_state.variables_bindings[this->query.nodes[next].row[2]] = min((float)stat.object_uniques, min(old_state.variables_bindings[this->query.nodes[next].row[2]], old_state.variables_bindings[this->query.nodes[next].row[0]]*stat.pred_per_subj));
            }
            else{
                next_state.variables_bindings[this->query.nodes[next].row[0]] = min((float)stat.subject_uniques, min(old_state.variables_bindings[this->query.nodes[next].row[0]], old_state.variables_bindings[this->query.nodes[next].row[2]]*stat.pred_per_obj));
                next_state.variables_bindings[this->query.nodes[next].row[2]] = min(old_state.variables_bindings[this->query.nodes[next].row[2]], (float)stat.object_uniques);
            }
        }
        else if(this->query.nodes[next].variables == V_SUBJ){
            next_state.variables_bindings[this->query.nodes[next].row[0]] = min(old_state.variables_bindings[this->query.nodes[next].row[0]], (float)this->query.nodes[next].cardinality);
        }
        else{
            next_state.variables_bindings[this->query.nodes[next].row[2]] = min(old_state.variables_bindings[this->query.nodes[next].row[2]], (float)this->query.nodes[next].cardinality);
        }

        //next_state.cost = 0.8*next_state.communication+0.2*next_state.cardinality;
        next_state.cost = next_state.communication;
        /*if(next_state.ordered_nodes.size() == this->query.nodes.size())
            cout<<part_string<<"\n"<<next_state.print()<<endl;*/


        /*if(this->order_table.find(new_state_string) == this->order_table.end()){
            //cout<<part_string<<"\nADDING FOR FIRST TIME\n"<<next_state.print()<<endl;
            this->order_table[new_state_string]= next_state;
        }
        else{
            if(this->order_table[new_state_string].communication > next_state.communication){
                //cout<<part_string<<"\nFOUND CHEAPER STATE\n"<<this->order_table[new_state_string].print()<<"\n"<<part_string<<"\n"<<next_state.print()<<endl;
                this->order_table[new_state_string] = next_state;
            }
            else if(this->order_table[new_state_string].communication == next_state.communication){
                //cout<<part_string<<"\nEQUAL\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
                if(this->order_table[new_state_string].cum_cardinality <= next_state.cum_cardinality){
                    //cout<<this->order_table[new_state_string].cost<<" "<<next_state.cost<<endl;
                    //cout<<this->order_table[new_state_string].cum_cardinality<<" "<<next_state.cum_cardinality<<endl;
                    //cout<<"YA ALLAH"<<endl;
                    return;
                }
            }
            else{
                //cout<<part_string<<"\nGREATER\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
                return;
            }
        }*/

        if(this->order_table.find(new_state_string) == this->order_table.end()){
            //cout<<part_string<<"\nADDING FOR FIRST TIME\n"<<next_state.print()<<endl;
            this->order_table[new_state_string]= next_state;
        }
        else{
            if(this->order_table[new_state_string].cost > next_state.cost){
                //cout<<part_string<<"\nFOUND CHEAPER STATE\n"<<this->order_table[new_state_string].print()<<"\n"<<part_string<<"\n"<<next_state.print()<<endl;
                this->order_table[new_state_string] = next_state;
            }
            else if(this->order_table[new_state_string].cost == next_state.cost){
                //cout<<part_string<<"\nEQUAL\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
                if(this->order_table[new_state_string].cum_cardinality <= next_state.cum_cardinality){
                    //cout<<this->order_table[new_state_string].cost<<" "<<next_state.cost<<endl;
                    //cout<<this->order_table[new_state_string].cum_cardinality<<" "<<next_state.cum_cardinality<<endl;
                    //cout<<"YA ALLAH"<<endl;
                    return;
                }
            }
            else{
                //cout<<part_string<<"\nGREATER\n"<<this->order_table[new_state_string].print()<<"\n"<<next_state.print()<<endl;
                return;
            }
        }
    }
    else{//cannot join
        //cout<<old_state.res.print()<<" CANNOT JOIN "<<this->query.nodes[next].print()<<endl;
        remaining.push_back(next);
        return;
    }
    for(unsigned it = 0 ; it != remaining.size() ;it++){
        this->branch(new_state_string, remaining[it], remaining);
    }

}

void Plan::compute_walk_scores_max() {
    float current_score, stat_score;
    map<string, float>::iterator it;
    pred_stat_t::iterator p_it;
    Node n;

    for (unsigned i = 0; i < this->query.nodes.size(); i++) {
        n = this->query.nodes[i];
        p_it = this->master->manager->predicates_stats.find(n.row[1]);
        if(p_it != this->master->manager->predicates_stats.end()){
            it = walk_scores.find(n.row[0]);
            if(it == walk_scores.end()){
                walk_scores[n.row[0]] = 0;
                it = walk_scores.find(n.row[0]);
            }
            current_score = it->second;
            stat_score = p_it->second.subject_score;
            it->second = std::max(current_score, stat_score);

            it = walk_scores.find(n.row[2]);
            if(it == walk_scores.end()){
                walk_scores[n.row[2]] = 0;
                it = walk_scores.find(n.row[2]);
            }

            current_score = it->second;
            stat_score = p_it->second.object_score;
            it->second = std::max(current_score, stat_score);
        }
        else{
            it = walk_scores.find(n.row[0]);
            if(it == walk_scores.end()){
                walk_scores[n.row[0]] = 0;
            }

            it = walk_scores.find(n.row[2]);
            if(it == walk_scores.end()){
                walk_scores[n.row[2]] = 0;
            }
        }
    }

#ifdef REVERSE_WALK
    float min = numeric_limits<float>::max();
    for(it = walk_scores.begin(); it != walk_scores.end(); it++){
        if(it->second < min){
            min = it->second;
            this->core = it->first;
        }
    }
#else
    float max = -1;
    for(it = walk_scores.begin(); it != walk_scores.end(); it++){
        if(it->second > max){
            max = it->second;
            this->core = it->first;
        }
    }
#endif
}

void Plan::compute_walk_scores_sum() {
    float current_score, stat_score;
    map<string, float>::iterator it;
    pred_stat_t::iterator p_it;
    Node n;

    for (unsigned i = 0; i < this->query.nodes.size(); i++) {
        n = this->query.nodes[i];
        p_it = this->master->manager->predicates_stats.find(n.row[1]);
        if(p_it != this->master->manager->predicates_stats.end()){
            it = walk_scores.find(n.row[0]);
            if(it == walk_scores.end()){
                walk_scores[n.row[0]] = 0;
                it = walk_scores.find(n.row[0]);
            }
            current_score = it->second;
            stat_score = p_it->second.subject_score;
            it->second = current_score+stat_score;

            it = walk_scores.find(n.row[2]);
            if(it == walk_scores.end()){
                walk_scores[n.row[2]] = 0;
                it = walk_scores.find(n.row[2]);
            }

            current_score = it->second;
            stat_score = p_it->second.object_score;
            it->second = current_score+stat_score;
        }
    }

#ifdef REVERSE_WALK
    float min = numeric_limits<float>::max();
    for(it = walk_scores.begin(); it != walk_scores.end(); it++){
        if(it->second < min){
            min = it->second;
            this->core = it->first;
        }
    }
#else
    float max = -1;
    for(it = walk_scores.begin(); it != walk_scores.end(); it++){
        if(it->second > max){
            max = it->second;
            core = it->first;
        }
    }
#endif
}

void Plan::compute_walk_scores() {
#ifdef MAX_SUM
    compute_walk_scores_sum();
#else
    compute_walk_scores_max();
#endif
}

void Plan::walk_query() {

    priority_queue<walk_edge*,vector<walk_edge*>, mycomparison> EE;
    map<string, vector<pair<string, string> > > siblings;
    walk_node n1, n2;
    walk_edge * tmp_e;
    boost::unordered_set<walk_edge*, walk_edge_hash, walk_edge_equal> EE_marked_edges, ODE_marked_edges;
    // core data placement
    for (unsigned i = 0; i < this->query.nodes.size(); i++) {
        siblings[this->query.nodes[i].row[0]].push_back(make_pair(this->query.nodes[i].row[1],this->query.nodes[i].row[2]));
        siblings[this->query.nodes[i].row[2]].push_back(make_pair(this->query.nodes[i].row[1],this->query.nodes[i].row[0]));

        int hash_idx = -1;
        if (this->query.nodes[i].row[0] == core) {
            hash_idx = 0;
            n1.label = this->query.nodes[i].row[0];
            n1.score = walk_scores[n1.label];
            n2.label = this->query.nodes[i].row[2];
            n2.score = walk_scores[n2.label];
        } else if (this->query.nodes[i].row[2] == core) {
            hash_idx = 2;
            n1.label = this->query.nodes[i].row[2];
            n1.score = walk_scores[n1.label];
            n2.label = this->query.nodes[i].row[0];
            n2.score = walk_scores[n2.label];
        }

        if (hash_idx > -1) {
            tmp_e = new walk_edge(n1, n2, this->query.nodes[i].row[1]);
            tmp_e->nodes_reversed = false;
            tmp_e->parent = NULL;
            if (hash_idx == 2)
                tmp_e->nodes_reversed = true;
            EE.push(tmp_e);
            ODE.push_back(tmp_e);
            EE_marked_edges.insert(tmp_e);
            ODE_marked_edges.insert(tmp_e);
            //cout<<EE_marked_edges.size()<<" "<<ODE_marked_edges.size()<<" "<<ODE.size()<<endl;
        }
    }

    walk_edge * lookup_edge = new walk_edge();
    walk_edge * e;
    vector<walk_edge*> corresponding_triples;
    vector<pair<string, string> > sibs;

    while (!EE.empty()) {
        // first we remove the edge e that starts with the highest score
        e = EE.top();
        EE.pop();
        //cout<<"TOP: "<<e<<" "<<e->print()<<endl;

        //First we add the edge
        if(e->parent != NULL){
            ODE_marked_edges.insert(e);
            ODE.push_back(e);
        }
        //Then we get all edges between e's ends (doesn't have to be just e)
        get_triple_by_ends(corresponding_triples, e->n1.label, e->n2.label, ODE_marked_edges);
        //cout<<corresponding_triples.size()<<endl;
        for (unsigned i = 0; i < corresponding_triples.size(); i++) {
            if (corresponding_triples[i]->n1.label != core	&& corresponding_triples[i]->n2.label != core) {
                ODE_marked_edges.insert(corresponding_triples[i]);
                corresponding_triples[i]->parent = e->parent;
                ODE.push_back(corresponding_triples[i]);
            }
        }

        if (siblings.find(e->n2.label) == siblings.end())
            continue;

        sibs = siblings[e->n2.label];
        //cout<<sibs.size()<<endl;
        for (unsigned i = 0; i < sibs.size(); i++) {
            //cout<<sibs[i].first<<" "<<sibs[i].second<<endl;
            // we only consider unmarked edges, so we look it up in the set
            lookup_edge->n1.label = e->n2.label;
            lookup_edge->label = sibs[i].first;
            lookup_edge->n2.label = sibs[i].second;

            if (EE_marked_edges.find(lookup_edge) == EE_marked_edges.end()) {
                //cout<<"NOT FOUND "<<endl;
                get_triple_by_ends(corresponding_triples, e->n2.label,	sibs[i].second, EE_marked_edges);
                for (unsigned j = 0; j < corresponding_triples.size(); j++) {
                    EE_marked_edges.insert(corresponding_triples[j]);
                    corresponding_triples[j]->n1.score = walk_scores[corresponding_triples[j]->n1.label];
                    corresponding_triples[j]->n2.score = walk_scores[corresponding_triples[j]->n2.label];
                    //cout<<"SCORES:"<<corresponding_triples[j]->n1.score<<" "<<corresponding_triples[j]->n2.score<<endl;
                    corresponding_triples[j]->parent = e;
                    e->children.push_back(corresponding_triples[j]);
                    EE.push(corresponding_triples[j]);
                    //cout<<e->n2.label<<" "<<sibs[i].second<<" COR: "<<corresponding_triples[j]->print()<<" "<<corresponding_triples[j]->nodes_reversed<<endl;
                }
            }
        }
    }
    delete lookup_edge;
}

void Plan::get_triple_by_ends(vector<walk_edge*> &res, string &end1, string &end2, boost::unordered_set<walk_edge*, walk_edge_hash, walk_edge_equal> &filter) {
    walk_edge * tmp_edge = new walk_edge();

    res.clear();
    for (unsigned i = 0; i < this->query.nodes.size(); i++) {
        tmp_edge->n1.label = this->query.nodes[i].row[0];
        tmp_edge->n2.label = this->query.nodes[i].row[2];
        tmp_edge->label = this->query.nodes[i].row[1];
        if(filter.find(tmp_edge) == filter.end()){
            if (this->query.nodes[i].row[0] == end1 && this->query.nodes[i].row[2] == end2){
                res.push_back(new walk_edge(this->query.nodes[i].row[0], this->query.nodes[i].row[2], this->query.nodes[i].row[1]));
                res[res.size()-1]->nodes_reversed =false;
            }
            else if (this->query.nodes[i].row[0] == end2 && this->query.nodes[i].row[2] == end1){
                res.push_back(new walk_edge(this->query.nodes[i].row[2], this->query.nodes[i].row[0], this->query.nodes[i].row[1]));
                res[res.size()-1]->nodes_reversed = true;
            }
        }
    }
    delete tmp_edge;
}

void Plan::get_projection_pos(vector<string> &res, vector<int> &pos){
    for(unsigned i = 0 ; i < this->query.projections.size() ; i++){
        for(unsigned j = 0 ; j < res.size(); j++){
            if(this->query.projections[i] == res[j]){
                pos.push_back(j);
                break;
            }
        }
    }
}
