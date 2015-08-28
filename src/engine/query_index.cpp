#include "engine/query_index.h"
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
Query_Index::Query_Index(MasterGUI * master) {
	this->master = master;
    this->distrib_queries = new query_index_t();
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	this->startup_time = timespec_to_double(ts);
}

Query_Index::~Query_Index() {
	delete this->distrib_queries;
}

bool Query_Index::is_parallel_query(Plan * plan){
	if(!isVariable(plan->core)){
		return is_parallel_const(plan);
	}
	else{
		return is_parallel_var(plan);
	}
}

void Query_Index::update_time_stamps(boost::unordered_set<query_index_node*> &leaves){
	set<query_index_node*> updated;
	boost::unordered_set<query_index_node*>::iterator it;
	query_index_node * tmp_node;
	timespec ts;
	double d_ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	d_ts = timespec_to_double(ts) - this->startup_time;

	for(it = leaves.begin() ; it != leaves.end() ; it++){
		tmp_node = *it;
		this->qi_leaves.insert(tmp_node);

		while(tmp_node != NULL){
			if(updated.find(tmp_node) == updated.end()){
				updated.insert(tmp_node);
				tmp_node->count++;
				tmp_node->ts = d_ts;
			}
			tmp_node = tmp_node->parent;
		}
	}
	//cout<<"Leaves: "<<this->qi_leaves.size()<<endl;
	/*for(unordered_set<query_index_node*>::iterator ii = qi_leaves.begin(); ii != qi_leaves.end(); ii++){
		cout<<(*ii)->encode()<<" "<<(*ii)->ts<<endl;
	}*/
}

bool Query_Index::is_parallel_const(Plan* plan){
	query_index_node *tmp_index_node, *parent_index_node;
	bool const_core_match = true, var_core_match = true, exact_match, super_set;
	query_index_t::iterator it;
	boost::unordered_map<string, query_index_node *> index_nodes_ptrs;
	vector<int> remaining, tmp;
	walk_edge * tmp_edge;
	boost::unordered_map<string, query_index_node *>::iterator ptrs_it;
	boost::unordered_set<query_index_node*> const_leaves, var_leaves;
	//Working on core incident edges
	for(unsigned i = 0 ; i < plan->ODE.size() ; i++){
		if(plan->ODE[i]->parent == NULL){
			/*found = false;
			tmp_index_node = new query_index_node(plan->ODE[i]);
			for(it = this->distrib_queries->begin() ; it != this->distrib_queries->end() ; it++){
				if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && ((*it)->binding == tmp_index_node->binding) && (isVariable((*it)->propagating) && isVariable(tmp_index_node->propagating))){
					found = true;
					break;
				}
			}
			if(found){
				index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
				delete tmp_index_node;
				tmp_index_node = *it;
				if(plan->ODE[i]->children.size() == 0)
					const_leaves.insert(tmp_index_node);
			}
			else{
				const_core_match = false;
				const_leaves.clear();
				break;
			}*/
			tmp_index_node = new query_index_node(plan->ODE[i], true);
			it = this->distrib_queries->find(tmp_index_node);
			if(it != this->distrib_queries->end()){
				index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
				delete tmp_index_node;
				tmp_index_node = *it;
				if(plan->ODE[i]->children.size() == 0)
					const_leaves.insert(tmp_index_node);
			}
			else{
				const_core_match = false;
				const_leaves.clear();
				break;
			}
		}
		else{
			remaining.push_back(i);
		}
	}
	if(const_core_match){
		while(!remaining.empty() && const_core_match){
			for(unsigned i = 0 ; i < remaining.size(); i++){
				tmp_edge = plan->ODE[remaining[i]];
				ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
				if(ptrs_it != index_nodes_ptrs.end()){
					parent_index_node = ptrs_it->second;
					tmp_index_node = new query_index_node(tmp_edge);
					if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
						/*exact_match = false;
						super_set = false;
						for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
							if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
								if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
									exact_match = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
								else if(isVariable((*it)->propagating)){
									super_set = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
							}
						}
						if(super_set){
							const_leaves.insert(tmp_index_node);
						}
						else if(exact_match){
							const_leaves.insert(tmp_index_node);
						}
						else{
							const_leaves.clear();
							remaining.clear();
							const_core_match= false;
							break;
						}*/
						exact_match = false;
						super_set = false;
						for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
							if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
								if(isVariable((*it)->propagating)){
									if(isVariable(tmp_index_node->propagating)){
										exact_match = true;
										delete tmp_index_node;
										tmp_index_node = *it;
									}
									else{
										super_set = true;
										delete tmp_index_node;
										tmp_index_node = *it;
										break;
									}
								}
								else if((*it)->propagating == tmp_index_node->propagating){
									exact_match = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
							}
						}
						if(super_set || exact_match){
							const_leaves.insert(tmp_index_node);
						}
						else{
							const_leaves.clear();
							remaining.clear();
							const_core_match= false;
							break;
						}
					}
					else{
						it = parent_index_node->children.find(tmp_index_node);
						if(it == parent_index_node->children.end()){
							const_leaves.clear();
							remaining.clear();
							const_core_match= false;
							break;
						}
						else{
							index_nodes_ptrs[tmp_edge->encode()] = *it;
							delete tmp_index_node;
							tmp_index_node = *it;
							if(tmp_edge->children.size() == 0)
								const_leaves.insert(tmp_index_node);
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
			/*found = false;
			tmp_index_node = new query_index_node(plan->ODE[i]);
			for(it = this->distrib_queries->begin() ; it != this->distrib_queries->end() ; it++){
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
				if(plan->ODE[i]->children.size() == 0)
					var_leaves.insert(tmp_index_node);
			}
			else{
				var_core_match = false;
				var_leaves.clear();
				break;
			}*/
			tmp_index_node = new query_index_node(plan->ODE[i], false);
			it = this->distrib_queries->find(tmp_index_node);

			if(it != this->distrib_queries->end()){
				index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
				delete tmp_index_node;
				tmp_index_node = *it;
				if(plan->ODE[i]->children.size() == 0)
					var_leaves.insert(tmp_index_node);
			}
			else{
				var_core_match = false;
				var_leaves.clear();
				break;
			}
		}
		else{
			remaining.push_back(i);
		}
	}
	if(var_core_match){
		while(!remaining.empty() && var_core_match){
			for(unsigned i = 0 ; i < remaining.size(); i++){
				tmp_edge = plan->ODE[remaining[i]];
				ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
				if(ptrs_it != index_nodes_ptrs.end()){
					parent_index_node = ptrs_it->second;
					tmp_index_node = new query_index_node(tmp_edge);
					if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
						exact_match = false;
						super_set = false;
						for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
							/*if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
								if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
									exact_match = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
								else if(isVariable((*it)->propagating)){
									super_set = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
							}*/

							if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
								if(isVariable((*it)->propagating)){
									if(isVariable(tmp_index_node->propagating)){
										exact_match = true;
										delete tmp_index_node;
										tmp_index_node = *it;
									}
									else{
										super_set = true;
										delete tmp_index_node;
										tmp_index_node = *it;
										break;
									}
								}
								else if((*it)->propagating == tmp_index_node->propagating){
									exact_match = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
							}
						}
						if(super_set || exact_match){
							var_leaves.insert(tmp_index_node);
						}
						else{
							var_leaves.clear();
							remaining.clear();
							var_core_match= false;
							break;
						}
					}
					else{
						it = parent_index_node->children.find(tmp_index_node);
						if(it == parent_index_node->children.end()){
							var_leaves.clear();
							remaining.clear();
							var_core_match = false;
							break;
						}
						else{
							index_nodes_ptrs[tmp_edge->encode()] = *it;
							delete tmp_index_node;
							tmp_index_node = *it;
							if(tmp_edge->children.size() == 0)
								var_leaves.insert(tmp_index_node);
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

	plan->leaves.clear();
	if(var_core_match){
		plan->leaves.insert(var_leaves.begin(), var_leaves.end());
		return true;
	}
	else if(const_core_match){
		plan->leaves.insert(const_leaves.begin(), const_leaves.end());
		return true;
	}
	else
		return false;
}

bool Query_Index::is_parallel_var(Plan* plan){
	query_index_node *tmp_index_node, *parent_index_node;
	bool exact_match, super_set;
	query_index_t::iterator it;
	boost::unordered_map<string, query_index_node *> index_nodes_ptrs;
	vector<int> remaining, tmp;
	walk_edge * tmp_edge;
	boost::unordered_map<string, query_index_node *>::iterator ptrs_it;

	//Working on core incident edges
	for(unsigned i = 0 ; i < plan->ODE.size() ; i++){
		if(plan->ODE[i]->parent == NULL){
			tmp_index_node = new query_index_node(plan->ODE[i], false);
			it = this->distrib_queries->find(tmp_index_node);
			if(it == this->distrib_queries->end()){
				plan->leaves.clear();
				return false;
			}
			else{
				index_nodes_ptrs[plan->ODE[i]->encode()] = *it;
				delete tmp_index_node;
				tmp_index_node = *it;
				if(plan->ODE[i]->children.size() == 0)
					plan->leaves.insert(tmp_index_node);
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
				tmp_index_node = new query_index_node(tmp_edge);
				if(!isVariable(tmp_index_node->propagating)){//i.e. constant leaf
					exact_match = false;
					super_set = false;
					for(it = parent_index_node->children.begin() ; it != parent_index_node->children.end(); it++){
						/*if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
							if(((*it)->propagating == tmp_index_node->propagating) && (!super_set)){
								exact_match = true;
								delete tmp_index_node;
								tmp_index_node = *it;
							}
							else if(isVariable((*it)->propagating)){
								super_set = true;
								delete tmp_index_node;
								tmp_index_node = *it;
							}
						}*/
						if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && (isVariable((*it)->binding) && isVariable(tmp_index_node->binding))){
							if(isVariable((*it)->propagating)){
								if(isVariable(tmp_index_node->propagating)){
									exact_match = true;
									delete tmp_index_node;
									tmp_index_node = *it;
								}
								else{
									super_set = true;
									delete tmp_index_node;
									tmp_index_node = *it;
									break;
								}
							}
							else if((*it)->propagating == tmp_index_node->propagating){
								exact_match = true;
								delete tmp_index_node;
								tmp_index_node = *it;
							}
						}
					}
					if(super_set || exact_match){
						plan->leaves.insert(tmp_index_node);
					}
					else{
						plan->leaves.clear();
						return false;
					}
				}
				else{
					it = parent_index_node->children.find(tmp_index_node);
					if(it == parent_index_node->children.end()){
						plan->leaves.clear();
						return false;
					}
					else{
						index_nodes_ptrs[tmp_edge->encode()] = *it;
						delete tmp_index_node;
						tmp_index_node = *it;
						if(tmp_edge->children.size() == 0)
							plan->leaves.insert(tmp_index_node);
					}
				}
			}
			else{
				tmp.push_back(remaining[i]);
			}
		}
		remaining.swap(tmp);
	}
	return true;
}

void Query_Index::check_for_frequency(Plan * plan){
	int min_count = numeric_limits<int>::max();
	query_count * parent = NULL;
	bool subset = true;
	unsigned int var_count = plan->query.variables.size();

	for(unsigned i = 0; i < plan->ODE.size() ; i++){
		if(plan->ODE[i]->parent == NULL){
			this->update_query_count(parent, plan->ODE[i], min_count, var_count, plan, subset);
		}
	}
    if(min_count >= qft){
		if(subset)
			plan->operation = PHD_PARALLEL;
		else
			plan->operation = PHD_SEMI_JOIN;
	}
}

void Query_Index::update_query_count(query_count * parent, walk_edge * edge, int &min_count, unsigned int& var_count, Plan * plan, bool &subset){
	query_count * tmp_edge = new query_count(edge);
	q_count_t *level_edges;
	walk_edge previous_edge(edge);
	if(parent == NULL)
		level_edges = &(this->query_counter);
	else
		level_edges = &(parent->children);
	q_count_t::iterator it = level_edges->find(tmp_edge);
	if(it == level_edges->end()){
		tmp_edge->parent = parent;
		level_edges->insert(tmp_edge);
		tmp_edge->count = 1;
	}
	else{
		delete tmp_edge;
		tmp_edge = *it;
		tmp_edge->count++;
	}
	tmp_edge->update_proactivity_info(edge, var_count, plan->walk_scores);
	if(!previous_edge.subOf(edge)){
		subset = false;
	}
	if(tmp_edge->count < min_count)
		min_count = tmp_edge->count;

	for(unsigned i = 0 ; i < edge->children.size() ; i++){
		this->update_query_count(tmp_edge, edge->children[i], min_count, var_count, plan, subset);
	}
}

void Query_Index::add_distributed_query(Plan & plan){
	query_index_node *tmp_index_node, *parent_index_node;
	//bool found;
	query_index_t::iterator it;
	boost::unordered_map<string, query_index_node *> index_nodes_ptrs;
	vector<int> remaining, tmp;
	walk_edge * tmp_edge;
	boost::unordered_map<string, query_index_node *>::iterator ptrs_it;
	timespec ts;
	double d_ts;

	clock_gettime(CLOCK_REALTIME, &ts);
	d_ts = timespec_to_double(ts) - this->startup_time;
	//Working on core incident edges
	for(unsigned i = 0 ; i < plan.ODE.size() ; i++){
		if(plan.ODE[i]->parent == NULL){
			//if the core is constant we do exact match
			if(!isVariable(plan.ODE[i]->n1.label)){
				/*found = false;
				tmp_index_node = new query_index_node(plan.ODE[i]);
				for(it = this->distrib_queries->begin() ; it != this->distrib_queries->end() ; it++){
					if(((*it)->predicate == tmp_index_node->predicate) && ((*it)->reversed == tmp_index_node->reversed) && ((*it)->binding == tmp_index_node->binding) && (isVariable((*it)->propagating) && isVariable(tmp_index_node->propagating))){
						found = true;
						break;
					}
				}
				if(found){
					index_nodes_ptrs[plan.ODE[i]->encode()] = *it;
					delete tmp_index_node;
					tmp_index_node = *it;
				}
				else{
					this->distrib_queries->insert(tmp_index_node);
					index_nodes_ptrs[plan.ODE[i]->encode()] = tmp_index_node;
				}*/
				tmp_index_node = new query_index_node(plan.ODE[i], true);
				it = this->distrib_queries->find(tmp_index_node);

				if(it != this->distrib_queries->end()){
					index_nodes_ptrs[plan.ODE[i]->encode()] = *it;
					delete tmp_index_node;
					tmp_index_node = *it;
				}
				else{
					this->distrib_queries->insert(tmp_index_node);
					tmp_index_node->exact = false;
					index_nodes_ptrs[plan.ODE[i]->encode()] = tmp_index_node;
				}
			}
			else{
				tmp_index_node = new query_index_node(plan.ODE[i]);
				it = this->distrib_queries->find(tmp_index_node);
				if(it == this->distrib_queries->end()){
					this->distrib_queries->insert(tmp_index_node);
					index_nodes_ptrs[plan.ODE[i]->encode()] = tmp_index_node;
				}
				else{
					index_nodes_ptrs[plan.ODE[i]->encode()] = *it;
					delete tmp_index_node;
					tmp_index_node = *it;
				}
			}
			tmp_index_node->count++;
			tmp_index_node->ts = d_ts;
			if(plan.ODE[i]->children.size() == 0)
				this->qi_leaves.insert(tmp_index_node);
		}
		else{
			remaining.push_back(i);
		}
	}

	while(!remaining.empty()){
		for(unsigned i = 0 ; i < remaining.size(); i++){
			tmp_edge = plan.ODE[remaining[i]];
			ptrs_it = index_nodes_ptrs.find(tmp_edge->parent->encode());
			if(ptrs_it != index_nodes_ptrs.end()){
				parent_index_node = ptrs_it->second;
				if(!isVariable(tmp_edge->n2.label)){//i.e. constant leaf
					tmp_index_node = new query_index_node(tmp_edge, true);
					it = parent_index_node->children.find(tmp_index_node);
					if(it == parent_index_node->children.end()){
						tmp_index_node->parent = parent_index_node;
						if(parent_index_node->children.empty()){
							this->qi_leaves.erase(parent_index_node);
						}
						parent_index_node->children.insert(tmp_index_node);
						index_nodes_ptrs[tmp_edge->encode()] = tmp_index_node;
						tmp_index_node->exact = false;
					}
					else{
						index_nodes_ptrs[tmp_edge->encode()] = *it;
						delete tmp_index_node;
						tmp_index_node = *it;
					}
					tmp_index_node->count++;
					tmp_index_node->ts = d_ts;
					if(plan.ODE[remaining[i]]->children.size() == 0)
						this->qi_leaves.insert(tmp_index_node);
				}
				else{
					tmp_index_node = new query_index_node(tmp_edge);
					it = parent_index_node->children.find(tmp_index_node);
					if(it == parent_index_node->children.end()){
						tmp_index_node->parent = parent_index_node;
						if(parent_index_node->children.empty()){
							this->qi_leaves.erase(parent_index_node);
						}
						parent_index_node->children.insert(tmp_index_node);
						index_nodes_ptrs[tmp_edge->encode()] = tmp_index_node;
					}
					else{
						index_nodes_ptrs[tmp_edge->encode()] = *it;
						delete tmp_index_node;
						tmp_index_node = *it;
					}
					tmp_index_node->count++;
					tmp_index_node->ts = d_ts;
					if(plan.ODE[remaining[i]]->children.size() == 0)
						this->qi_leaves.insert(tmp_index_node);
				}
				/*parent_index_node = ptrs_it->second;
				tmp_index_node = new query_index_node(tmp_edge);
				it = parent_index_node->children.find(tmp_index_node);
				if(it == parent_index_node->children.end()){
					tmp_index_node->parent = parent_index_node;
					parent_index_node->children.insert(tmp_index_node);
					index_nodes_ptrs[tmp_edge->encode()] = tmp_index_node;
				}
				else{
					index_nodes_ptrs[tmp_edge->encode()] = *it;
					delete tmp_index_node;
					tmp_index_node = *it;
				}
				tmp_index_node->count++;
				tmp_index_node->ts = d_ts;
				if(plan.ODE[remaining[i]]->children.size() == 0)
					this->qi_leaves.insert(tmp_index_node);*/
			}
			else{
				tmp.push_back(remaining[i]);
			}
		}
		remaining.swap(tmp);
	}
	//cout<<"Leaves: "<<this->qi_leaves.size()<<endl;
}
