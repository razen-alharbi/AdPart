#include "engine/datastore.h"
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
Datastore::Datastore() {
	this->main_index = new Index();
	//this->rep_index = new replica_index_t();
}

Datastore::~Datastore() {
	delete main_index;
	//delete rep_index;
}

Data_record * Datastore::insertTriple(int s, int p, int o) {
	Data_record * record = new Data_record(s, o);
	if (this->verts_counter.find(s) == this->verts_counter.end())
		this->verts_counter[s] = 0;
	if (this->verts_counter.find(o) == this->verts_counter.end())
		this->verts_counter[o] = 0;

	this->verts_counter[s]++;
	this->verts_counter[o]++;

	this->index_triple(p, record);
	return record;
}

void Datastore::index_triple(int predicate, Data_record * rec) {
	this->main_index->insert(predicate, rec);
}

long long Datastore::getSize() {
	return this->main_index->records_count;
}

void Datastore::get_all_predicates(vector<string>& preds) {

	preds.clear();

	for (unsigned i = 0; i < this->main_index->p_index.size(); i++) {
		if (this->main_index->p_index[i].size() > 0)
			preds.push_back(toString(i));
	}
}

void Datastore::compute_stats(int pred, predicate_stat & stat,
		Communicator *comm) {
	this->accurate_stat(pred, stat, comm);
}

void Datastore::accurate_stat(int pred, predicate_stat & stat,
		Communicator *comm) {
	so_index_t::iterator it;
	boost::unordered_map<int, vector<Data_record*> >::iterator it2;
	vector<vector<int> > to_send, to_recv;
	boost::unordered_set<int> uniques;
	int num_workers, dest;
	predicate_index_t::iterator p_it;

	num_workers = comm->get_num_workers() - 1;

	stat.predicate_text = pred;
	stat.subject_total_degree = 0;
	stat.subject_uniques = 0;
	stat.subject_score = 0;

	p_it = this->main_index->p_index.find(pred);
	if (p_it != this->main_index->p_index.end()) {
		stat.total_count = p_it->second.size();
	}
	to_send.push_back(vector<int>());
	it = this->main_index->s_index.find(pred);
	if (it != this->main_index->s_index.end()) {
		to_send[0].push_back(it->second.size());
		for (it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			if (this->verts_counter.find(it2->first)
					!= this->verts_counter.end())
				stat.subject_total_degree += this->verts_counter[it2->first];
		}
	} else {
		to_send[0].push_back(0);
	}
	comm->mpi_all_to_all(to_send, to_recv);
	for (unsigned i = 1; i < to_recv.size(); i++) {
		stat.subject_uniques += to_recv[i][0];
	}

	if (stat.subject_uniques > 0) {
		stat.subject_score = ((float) stat.subject_total_degree)
																																																																																																																																																				/ stat.subject_uniques;
	}

	to_send[0].clear();
	to_send.resize(num_workers + 1);
	stat.object_total_degree = 0;
	stat.object_uniques = 0;
	stat.object_score = 0;

	it = this->main_index->o_index.find(pred);
	if (it != this->main_index->o_index.end()) {
		for (it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			if (uniques.find(it2->first) == uniques.end()) {
				dest = (it2->first % num_workers) + 1;
				to_send[dest].push_back(it2->first);
				uniques.insert(it2->first);
			}
		}

	}
	uniques.clear();
	comm->mpi_all_to_all(to_send, to_recv);
	for (unsigned i = 1; i < to_recv.size(); i++) {
		for (unsigned j = 0; j < to_recv[i].size(); j++) {
			if (uniques.find(to_recv[i][j]) == uniques.end()) {
				uniques.insert(to_recv[i][j]);
				if (this->verts_counter.find(to_recv[i][j])
						!= this->verts_counter.end())
					stat.object_total_degree +=
							this->verts_counter[to_recv[i][j]];
			}
		}
	}
	to_send.clear();
	to_send.push_back(vector<int>());
	to_send[0].push_back(uniques.size());
	comm->mpi_all_to_all(to_send, to_recv);

	for (unsigned i = 1; i < to_recv.size(); i++) {
		stat.object_uniques += to_recv[i][0];
	}
	if (stat.object_uniques > 0) {
		stat.object_score = ((float) stat.object_total_degree)
																																																																																																																																																				/ stat.object_uniques;
	}
}

void Datastore::exchange_verts_stats(Communicator *comm) {
	vector<vector<int> > to_send, to_recv;
	boost::unordered_map<int, int>::iterator vc_it;
	int dest, num_workers;

	num_workers = comm->get_num_workers() - 1;
	to_send.resize(num_workers + 1);

	for (vc_it = this->verts_counter.begin();
			vc_it != this->verts_counter.end(); vc_it++) {
		dest = (vc_it->first % num_workers) + 1;
		to_send[dest].push_back(vc_it->first);
		to_send[dest].push_back(vc_it->second);
	}
	comm->mpi_all_to_all(to_send, to_recv);

	for (unsigned i = 1; i < to_recv.size(); i++) {
		if ((int) i == comm->get_worker_id())
			continue;
		for (unsigned j = 0; j < to_recv[i].size(); j += 2) {
			if (this->verts_counter.find(to_recv[i][j])
					== this->verts_counter.end())
				this->verts_counter[to_recv[i][j]] = 0;

			this->verts_counter[to_recv[i][j]] += to_recv[i][j + 1];
		}
	}
}

long long Datastore::calc_sub_mass(Node &subquery) {
	int variables, predicate, q_const;
	predicate_index_t::iterator p_it;
	so_index_t::iterator so_it1;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	long long cardinality = 0;
	so_index_t * so_index;

	predicate = atoi(subquery.row[1].c_str());

	if (isVariable(subquery.row[0])) {
		if (isVariable(subquery.row[2])) {
			variables = V_BOTH;
		} else {
			variables = V_SUBJ;
			q_const = atoi(subquery.row[V_OBJ].c_str());
			so_index = &this->main_index->o_index;

		}
	} else {
		variables = V_OBJ;
		q_const = atoi(subquery.row[V_SUBJ].c_str());
		so_index = &this->main_index->s_index;
	}

	if (variables == V_BOTH) {
		p_it = this->main_index->p_index.find(predicate);
		if (p_it != this->main_index->p_index.end()) {
			cardinality += p_it->second.size();
		}
	} else {
		so_it1 = so_index->find(predicate);
		if (so_it1 != so_index->end()) {
			so_it2 = so_it1->second.find(q_const);
			if (so_it2 != so_it1->second.end()) {
				cardinality += so_it2->second.size();
			}
		}
	}
	return cardinality;
}

long long Datastore::join_two_queries_star(Node &q1, Node &q2,	vector<pair<int, int> > &join_cols, vector<int> &q1_data, bool join_on_const, string &join_const_val){
	int skip, predicate, q_const, q_const_val, other_col;
	long long total = 0;
	vector<int> results;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	so_index_t::iterator so_it1;
	vector<Data_record*>::iterator set_it;
	so_index_t * index;
	boost::unordered_set<int> uniques;
	vector<pair<int, int> > tmp_cols;
	Node res;

	if (!q1.temp_query) {
		skip = 1;
		if (q1.variables == V_BOTH)
			skip = 2;
	} else {
		skip = q1.row.size();
	}

	for (unsigned i = 0; i < join_cols.size(); i++) {
		if(!q1.temp_query){
			if (join_cols[i].first == V_OBJ)
				join_cols[i].first = 1;
		}
		if (join_cols[i].second == V_OBJ)
			join_cols[i].second = 1;
	}

	predicate = atoi(q2.row[1].c_str());

	if(join_on_const){
		index = &(main_index->s_index);
		results.clear();
		so_it1 = index->find(predicate);
		if (so_it1 != index->end()) {
			so_it2 = so_it1->second.find(atoi(join_const_val.c_str()));
			if (so_it2 != so_it1->second.end()) {
				if(join_cols.size() != 0){
					for (set_it = so_it2->second.begin();set_it != so_it2->second.end(); set_it++) {
						for (unsigned i = 0; i < q1_data.size(); i += skip) {
							if(q1_data[i + join_cols[1].first] == (*set_it)->triple[join_cols[1].second]){
								results.insert(results.end(), q1_data.begin() + i,q1_data.begin() + i + skip);
								total++;
							}
						}
					}
				}
				else{
					for (set_it = so_it2->second.begin();set_it != so_it2->second.end(); set_it++) {
						for (unsigned i = 0; i < q1_data.size(); i += skip) {
							results.insert(results.end(), q1_data.begin() + i,q1_data.begin() + i + skip);
							results.push_back((*set_it)->triple[1]);
							total++;
						}
					}
				}
			}
		}
	}
	else{
		if (join_cols[0].second == V_SUBJ) {
			index = &(main_index->s_index);
			q_const = 2;
			other_col = 1;
		} else {
			index = &(main_index->o_index);
			q_const = 0;
			other_col = 0;
		}

		q_const_val = -1;
		if (!isVariable(q2.row[q_const])) {
			q_const_val = atoi(q2.row[q_const].c_str());
		}

		results.clear();
		so_it1 = index->find(predicate);
		if (so_it1 != index->end()) {
			for (unsigned i = 0; i < q1_data.size(); i += skip) {
				so_it2 = so_it1->second.find(q1_data[i + join_cols[0].first]);
				if (so_it2 != so_it1->second.end()) {
					for (set_it = so_it2->second.begin();
							set_it != so_it2->second.end(); set_it++) {
						if (q_const_val == -1) {
							if (join_cols.size() == 1) {
								results.insert(results.end(), q1_data.begin() + i,
										q1_data.begin() + i + skip);
								results.push_back((*set_it)->triple[other_col]);
								total++;

							} else {
								if (q1_data[i + join_cols[1].first]
								            == (*set_it)->triple[join_cols[1].second]) {
									results.insert(results.end(),
											q1_data.begin() + i,
											q1_data.begin() + i + skip);
									total++;

								}
							}
						} else {
							if ((*set_it)->triple[other_col] == q_const_val) {
								results.insert(results.end(), q1_data.begin() + i,
										q1_data.begin() + i + skip);
								total++;

							}
						}
					}
				}
			}
		}
	}

	q1_data.swap(results);
	return total;
}

long long Datastore::join_two_queries(Node &q1, Node &q2,
		vector<pair<int, int> > &join_cols, vector<int> &q1_data,
		Index * q2_index, int locality, int prep_col,
		vector<vector<int> > &all_jc, int num_workers, int rank,
		simple_index_t *tmp_index, int current_tmp_index, int q2_const_subject) {
	int skip, predicate, q_const, q_const_val, other_col;
	long long total = 0;
	vector<int> results;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	so_index_t::iterator so_it1;
	vector<Data_record*>::iterator set_it;
	so_index_t * index;
	boost::unordered_set<int> uniques;
	vector<pair<int, int> > tmp_cols;
	Node res;
	int ahead_skip = 0, dest, tmp_value, res_skip = 0;

	if (locality != NO_PREPARE) {
		q1.get_join_node_and_cols(q2, tmp_cols, res);
		res_skip = res.row.size();
		ahead_skip = res_skip - prep_col;
		for (unsigned i = 0; i < all_jc.size(); i++) {
			all_jc[i].clear();
		}
	}

	if (!q1.temp_query) {
		skip = 1;
		if (q1.variables == V_BOTH)
			skip = 2;
	} else {
		skip = q1.row.size();
	}

	for (unsigned i = 0; i < join_cols.size(); i++) {
		if(!q1.temp_query){
			if (join_cols[i].first == V_OBJ)
				join_cols[i].first = 1;
		}
		if (join_cols[i].second == V_OBJ)
			join_cols[i].second = 1;
	}

	predicate = atoi(q2.row[1].c_str());
	if (join_cols[0].second == V_SUBJ) {
		index = &(q2_index->s_index);
		q_const = 2;
		other_col = 1;
	} else {
		index = &(q2_index->o_index);
		q_const = 0;
		other_col = 0;
	}

	q_const_val = -1;
	if (!isVariable(q2.row[q_const])) {
		q_const_val = atoi(q2.row[q_const].c_str());
	}

	results.clear();
	for (unsigned i = 0; i < q1_data.size(); i += skip) {
		so_it1 = index->find(predicate);
		if (so_it1 != index->end()) {
			so_it2 = so_it1->second.find(q1_data[i + join_cols[0].first]);
			if (so_it2 != so_it1->second.end()) {
				for (set_it = so_it2->second.begin();
						set_it != so_it2->second.end(); set_it++) {
					if (q_const_val == -1) {
						if (join_cols.size() == 1) {
							results.insert(results.end(), q1_data.begin() + i,
									q1_data.begin() + i + skip);
							results.push_back((*set_it)->triple[other_col]);
							total++;
							if (locality != NO_PREPARE) {
								tmp_value = results[results.size()-ahead_skip];
								if (uniques.find(tmp_value) == uniques.end()) {
									uniques.insert(tmp_value);
									tmp_index[current_tmp_index][tmp_value] = vector<int>();
									if (locality == PREPARE_LOCALITY) {
										dest = ((tmp_value) % (num_workers - 1)) + 1;
										all_jc[dest].push_back(tmp_value);
									}
									else if(locality == PREPARE_LOCALITY_SUBJECT){
										dest = ((q2_const_subject) % (num_workers - 1)) + 1;
										all_jc[dest].push_back(tmp_value);
									}
									else {
										all_jc[rank].push_back(tmp_value);
									}
								}
								tmp_index[current_tmp_index][tmp_value].push_back(
										results.size() - res_skip);
							}
						} else {
							if (q1_data[i + join_cols[1].first]
							            == (*set_it)->triple[join_cols[1].second]) {
								results.insert(results.end(),
										q1_data.begin() + i,
										q1_data.begin() + i + skip);
								total++;
								if (locality != NO_PREPARE) {
									tmp_value = results[results.size()-ahead_skip];
									if (uniques.find(tmp_value) == uniques.end()) {
										uniques.insert(tmp_value);
										tmp_index[current_tmp_index][tmp_value] = vector<int>();
										if (locality == PREPARE_LOCALITY) {
											dest = ((tmp_value) % (num_workers - 1)) + 1;
											all_jc[dest].push_back(tmp_value);
										}
										else if(locality == PREPARE_LOCALITY_SUBJECT){
											dest = ((q2_const_subject) % (num_workers - 1)) + 1;
											all_jc[dest].push_back(tmp_value);
										}
										else {
											all_jc[rank].push_back(tmp_value);
										}
									}
									tmp_index[current_tmp_index][tmp_value].push_back(
											results.size() - res_skip);
								}
							}
						}
					} else {
						if ((*set_it)->triple[other_col] == q_const_val) {
							results.insert(results.end(), q1_data.begin() + i,
									q1_data.begin() + i + skip);
							total++;
							if (locality != NO_PREPARE) {
								//tmp_value = *(results.end() - ahead_skip);
								tmp_value = results[results.size()-ahead_skip];
								if (uniques.find(tmp_value) == uniques.end()) {
									uniques.insert(tmp_value);
									tmp_index[current_tmp_index][tmp_value] = vector<int>();
									if (locality == PREPARE_LOCALITY) {
										dest = ((tmp_value) % (num_workers - 1)) + 1;
										all_jc[dest].push_back(tmp_value);
									}
									else if(locality == PREPARE_LOCALITY_SUBJECT){
										dest = ((q2_const_subject) % (num_workers - 1)) + 1;
										all_jc[dest].push_back(tmp_value);
									}
									else {
										all_jc[rank].push_back(tmp_value);
									}
								}
								tmp_index[current_tmp_index][tmp_value].push_back(
										results.size() - res_skip);
							}
						}
					}
				}
			}
		}
	}
	q1_data = results;
	return total;
}
void Datastore::answer_sub_local(Local_Node &sub_query, Index *index, vector<int> &result){
	int predicate, q_const, var_index = 0;;
	predicate_index_t::iterator p_it;
	so_index_t::iterator so_it1;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	vector<Data_record*>::iterator set_it;
	so_index_t * so_index;
	boost::unordered_set<int> uniques;

	result.clear();
	predicate = atoi(sub_query.row[1].c_str());

	q_const = 0;
	so_index = &index->o_index;
	if (sub_query.variables == V_SUBJ) {
		q_const = atoi(sub_query.row[V_OBJ].c_str());
		so_index = &index->o_index;
		var_index = 0;

	} else if (sub_query.variables == V_OBJ) {
		q_const = atoi(sub_query.row[V_SUBJ].c_str());
		so_index = &index->s_index;
		var_index = 1;
	}

	if (sub_query.variables == V_BOTH) {
		p_it = index->p_index.find(predicate);
		if (p_it != index->p_index.end()) {
			for (set_it = p_it->second.begin(); set_it != p_it->second.end();
					set_it++) {
				result.push_back((*set_it)->triple[0]);
				result.push_back((*set_it)->triple[1]);
			}
		}
	} else {
		so_it1 = so_index->find(predicate);
		if (so_it1 != so_index->end()) {
			so_it2 = so_it1->second.find(q_const);
			if (so_it2 != so_it1->second.end()) {
				for (set_it = so_it2->second.begin();
						set_it != so_it2->second.end(); set_it++) {
					result.push_back((*set_it)->triple[var_index]);
				}
			}
		}
	}
}
void Datastore::answer_sub(Node &sub_query, Index *index, vector<int> &result,
		vector<vector<int> > &all_jc, bool prep_jc, int prep_locality,
		int jc_index, int rank, int num_workers, simple_index_t *tmp_index, int current_tmp_index, int q2_const_subject) {
	int predicate, q_const, dest, var_index = 0;;
	predicate_index_t::iterator p_it;
	so_index_t::iterator so_it1;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	vector<Data_record*>::iterator set_it;
	so_index_t * so_index;
	boost::unordered_set<int> uniques;
	int tmp_value;

	for (unsigned i = 0; i < all_jc.size(); i++) {
		all_jc[i].clear();
	}
	result.clear();
	predicate = atoi(sub_query.row[1].c_str());

	q_const = 0;
	so_index = &index->o_index;
	if (sub_query.variables == V_SUBJ) {
		q_const = atoi(sub_query.row[V_OBJ].c_str());
		so_index = &index->o_index;
		var_index = 0;

	} else if (sub_query.variables == V_OBJ) {
		q_const = atoi(sub_query.row[V_SUBJ].c_str());
		so_index = &index->s_index;
		var_index = 1;
	}

	if (sub_query.variables == V_BOTH) {
		p_it = index->p_index.find(predicate);
		if (p_it != index->p_index.end()) {
			for (set_it = p_it->second.begin(); set_it != p_it->second.end();
					set_it++) {
				result.push_back((*set_it)->triple[0]);
				result.push_back((*set_it)->triple[1]);
				if (prep_jc) {
					tmp_value = (*set_it)->triple[jc_index];
					if (uniques.find(tmp_value) == uniques.end()) {
						uniques.insert(tmp_value);
						if (prep_locality == PREPARE_LOCALITY) {
							dest = ((tmp_value) % (num_workers - 1)) + 1;
							all_jc[dest].push_back(tmp_value);
						}
						else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
							dest = ((q2_const_subject) % (num_workers - 1)) + 1;
							all_jc[dest].push_back(tmp_value);
						}
						else {
							all_jc[rank].push_back(tmp_value);
						}
						tmp_index[current_tmp_index][tmp_value] = vector<int>();
					}
					tmp_index[current_tmp_index][tmp_value].push_back(result.size() - 2);
				}
			}
		}
	} else {
		so_it1 = so_index->find(predicate);
		if (so_it1 != so_index->end()) {
			so_it2 = so_it1->second.find(q_const);
			if (so_it2 != so_it1->second.end()) {
				for (set_it = so_it2->second.begin();
						set_it != so_it2->second.end(); set_it++) {
					result.push_back((*set_it)->triple[var_index]);
					if (prep_jc) {
						tmp_value = (*set_it)->triple[jc_index];
						if (uniques.find(tmp_value) == uniques.end()) {
							uniques.insert(tmp_value);
							if (prep_locality == PREPARE_LOCALITY) {
								dest = ((tmp_value) % (num_workers - 1)) + 1;
								all_jc[dest].push_back(tmp_value);
							}
							else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
								dest = ((q2_const_subject) % (num_workers - 1)) + 1;
								all_jc[dest].push_back(tmp_value);
							}
							else {
								all_jc[rank].push_back(tmp_value);
							}
							tmp_index[current_tmp_index][tmp_value] = vector<int>();
						}
						tmp_index[current_tmp_index][tmp_value].push_back(result.size() - 1);
					}
				}
			}
		}
	}
}

void Datastore::build_candidate_list(Node &query, int join_col,
		vector<int> &jc_data, vector<int> &result) {
	so_index_t * index;
	so_index_t::iterator it;
	boost::unordered_map<int, vector<Data_record*> >::iterator it2;
	vector<Data_record*>::iterator set_it;
	int query_constant, predicate, query_constant_val;

	result.clear();
	predicate = atoi(query.row[1].c_str());

	if (join_col == 0) {
		index = &(main_index->s_index);
		query_constant = 2;
	} else {
		index = &(main_index->o_index);
		query_constant = 0;
	}

	if (query.variables == V_BOTH) {
		query_constant_val = -1;
	} else {
		query_constant_val = atoi(query.row[query_constant].c_str());
	}

	for (unsigned i = 0; i < jc_data.size(); i++) {
		it = index->find(predicate);
		if (it != index->end()) {
			it2 = it->second.find(jc_data[i]);
			if (it2 != it->second.end()) {
				for (set_it = it2->second.begin(); set_it != it2->second.end();
						set_it++) {
					if (query.variables == V_BOTH) {
						result.push_back((*set_it)->triple[0]);
						result.push_back((*set_it)->triple[1]);
					} else if ((*set_it)->triple[!join_col]
					                             == query_constant_val) {
						result.push_back((*set_it)->triple[join_col]);
					}
				}
			}
		}
	}
}

long long Datastore::finalize_join(Node &q1, Node &q2,
		vector<pair<int, int> > &join_cols, int prep_locality, int prep_col,
		vector<vector<int> > &all_candidate_lists, simple_index_t *tmp_index, int current_tmp_index,
		vector<vector<int> > &all_join_cols, vector<int> &result, int num_workers, int rank, int q2_const_subject) {



	Node res;
	vector<pair<int, int> > tmp_join_cols;
	int candid_record_size, num_records, index_key;
	vector<int> *index_value;
	vector<int> tmp_result;
	boost::unordered_set<int> uniques;
	int tmp_value = 0, res_skip = 0, ahead_skip = 0, dest = 0;

	q1.get_join_node_and_cols(q2, tmp_join_cols, res);
	//candid_record_size = res.row.size()-q1.num_variables+1;
	candid_record_size = q2.num_variables;

	if (prep_locality != NO_PREPARE) {
		res_skip = res.row.size();
		ahead_skip = res_skip - prep_col;
		for (unsigned i = 0; i < all_join_cols.size(); i++) {
			all_join_cols[i].clear();
		}
	}
	for (unsigned j = 0; j < join_cols.size(); j++) {
		if(join_cols[j].second == 2){
			if(q2.variables == V_BOTH)
				join_cols[j].second = 1;
			else{
				join_cols[j].second = 0;
			}
		}
		if(!q1.temp_query && join_cols[j].first == 2){
			if(q1.num_variables == 1)
				join_cols[j].first = 0;
			else
				join_cols[j].first = 1;
		}
	}
	tmp_index[!current_tmp_index].clear();
	for (unsigned i = 1; i < all_candidate_lists.size(); i++) {
		num_records = all_candidate_lists[i].size()/candid_record_size;
		if (all_candidate_lists[i].empty()){
			continue;
		}
		for (unsigned pos = 0; pos < all_candidate_lists[i].size(); pos+=candid_record_size) {
			index_key = all_candidate_lists[i][pos+join_cols[0].second];
			if (tmp_index[current_tmp_index].find(index_key) == tmp_index[current_tmp_index].end()) {
                throwException(toString(MPI::COMM_WORLD.Get_rank())+" ERROR. Temporary index failed on lookup: " + toString(index_key));
				exit(1);
			}

			index_value = &(tmp_index[current_tmp_index][index_key]);
			if(join_cols.size() > 1){
				for (unsigned j = 0; j < index_value->size(); j++) {
					if(result[(*index_value)[j]+join_cols[1].first] == all_candidate_lists[i][pos+join_cols[1].second]){
						tmp_result.insert(tmp_result.end(), result.begin()+(*index_value)[j], result.begin()+(*index_value)[j]+q1.num_variables);
						if (prep_locality != NO_PREPARE) {
							tmp_value = *(tmp_result.end() - ahead_skip);
							if (uniques.find(tmp_value) == uniques.end()) {
								uniques.insert(tmp_value);
								tmp_index[!current_tmp_index][tmp_value] = vector<int>();
								if (prep_locality == PREPARE_LOCALITY) {
									dest = ((tmp_value) % (num_workers - 1)) + 1;
									all_join_cols[dest].push_back(tmp_value);
								}
								else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
									dest = ((q2_const_subject) % (num_workers - 1)) + 1;
									all_join_cols[dest].push_back(tmp_value);
								}
								else {
									all_join_cols[rank].push_back(tmp_value);
								}
							}
							tmp_index[!current_tmp_index][tmp_value].push_back(tmp_result.size() - res_skip);
						}
					}
				}
			}
			else{
				if(candid_record_size == 1){
					for (unsigned j = 0; j < index_value->size(); j++) {

						tmp_result.insert(tmp_result.end(), result.begin()+(*index_value)[j], result.begin()+(*index_value)[j]+q1.num_variables);
						if (prep_locality != NO_PREPARE) {
							tmp_value = *(tmp_result.end() - ahead_skip);
							if (uniques.find(tmp_value) == uniques.end()) {
								uniques.insert(tmp_value);
								tmp_index[!current_tmp_index][tmp_value] = vector<int>();
								if (prep_locality == PREPARE_LOCALITY) {
									dest = ((tmp_value) % (num_workers - 1)) + 1;
									all_join_cols[dest].push_back(tmp_value);
								}
								else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
									dest = ((q2_const_subject) % (num_workers - 1)) + 1;
									all_join_cols[dest].push_back(tmp_value);
								}
								else {
									all_join_cols[rank].push_back(tmp_value);
								}
							}
							tmp_index[!current_tmp_index][tmp_value].push_back(tmp_result.size() - res_skip);
						}
					}
				}
				else{
					if(join_cols[0].second == 0){
						for (unsigned j = 0; j < index_value->size(); j++) {
							tmp_result.insert(tmp_result.end(), result.begin()+(*index_value)[j], result.begin()+(*index_value)[j]+q1.num_variables);
							tmp_result.push_back(all_candidate_lists[i][pos+1]);
							if (prep_locality != NO_PREPARE) {
								tmp_value = *(tmp_result.end() - ahead_skip);
								if (uniques.find(tmp_value) == uniques.end()) {
									uniques.insert(tmp_value);
									tmp_index[!current_tmp_index][tmp_value] = vector<int>();
									if (prep_locality == PREPARE_LOCALITY) {
										dest = ((tmp_value) % (num_workers - 1)) + 1;
										all_join_cols[dest].push_back(tmp_value);
									}
									else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
										dest = ((q2_const_subject) % (num_workers - 1)) + 1;
										all_join_cols[dest].push_back(tmp_value);
									}
									else {
										all_join_cols[rank].push_back(tmp_value);
									}
								}
								tmp_index[!current_tmp_index][tmp_value].push_back(tmp_result.size() - res_skip);
							}
						}

					}
					else{
						for (unsigned j = 0; j < index_value->size(); j++) {
							tmp_result.insert(tmp_result.end(), result.begin()+(*index_value)[j], result.begin()+(*index_value)[j]+q1.num_variables);
							tmp_result.push_back(all_candidate_lists[i][pos]);
							if (prep_locality != NO_PREPARE) {
								tmp_value = *(tmp_result.end() - ahead_skip);
								if (uniques.find(tmp_value) == uniques.end()) {
									uniques.insert(tmp_value);
									tmp_index[!current_tmp_index][tmp_value] = vector<int>();
									if (prep_locality == PREPARE_LOCALITY) {
										dest = ((tmp_value) % (num_workers - 1)) + 1;
										all_join_cols[dest].push_back(tmp_value);
									}
									else if(prep_locality == PREPARE_LOCALITY_SUBJECT){
										dest = ((q2_const_subject) % (num_workers - 1)) + 1;
										all_join_cols[dest].push_back(tmp_value);
									}
									else {
										all_join_cols[rank].push_back(tmp_value);
									}
								}
								tmp_index[!current_tmp_index][tmp_value].push_back(tmp_result.size() - res_skip);
							}
						}
					}
				}
			}
		}
	}
	result = tmp_result;
	return result.size()/res.row.size();
}

void Datastore::hash_distribute_core_edge(replica_index_node * edge, Communicator * comm, long long &replicated_records_count){
	int predicate, variables, q_const_val, dest_machine;
	so_index_t * index = NULL;
	predicate_index_t::iterator p_it;
	vector<Data_record*>::iterator set_it;
	vector<vector<int> > to_send, to_recv;
	so_index_t::iterator so_it1;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	int counter = 0;
	Data_record * tmp_record;

	to_send.resize(comm->get_num_workers());
	predicate = atoi(edge->predicate.c_str());

	//Note: We know that the edge is reversed
	if(isVariable(edge->binding)){
		if(isVariable(edge->propagating)){
			variables = V_BOTH;
			q_const_val = -1;
		}
		else{
			variables = V_OBJ;
			q_const_val = atoi(edge->propagating.c_str());
			index = &(this->main_index->s_index);
		}
	}
	else{
		variables = V_SUBJ;
		q_const_val = atoi(edge->binding.c_str());
		index = &(this->main_index->o_index);
	}

	if (variables == V_BOTH) {
		p_it = this->main_index->p_index.find(predicate);
		if(p_it != this->main_index->p_index.end()){
			for(set_it = p_it->second.begin(); set_it != p_it->second.end(); set_it++){
				dest_machine = ((*set_it)->triple[1]) % (comm->num_workers - 1) + 1;
				to_send[dest_machine].push_back((*set_it)->triple[0]);
				to_send[dest_machine].push_back((*set_it)->triple[1]);
				counter++;
			}
		}
	}
	else {
		so_it1 = index->find(predicate);
		if(so_it1 != index->end()){
			so_it2 = so_it1->second.find(q_const_val);
			if(so_it2 != so_it1->second.end()){
				for(set_it = so_it2->second.begin(); set_it != so_it2->second.end(); set_it++){
					dest_machine = ((*set_it)->triple[1]) % (comm->num_workers - 1) + 1;
					to_send[dest_machine].push_back((*set_it)->triple[0]);
					to_send[dest_machine].push_back((*set_it)->triple[1]);
					counter++;
				}
			}
		}
	}

	comm->mpi_all_to_all(to_send, to_recv);
	for (unsigned j = 1; j < to_recv.size(); j++) {
		for (unsigned l = 0; l < to_recv[j].size(); l += 2) {
			tmp_record = new Data_record(to_recv[j][l], to_recv[j][l+1]);
			edge->index->insert(predicate, tmp_record);
			replicated_records_count++;
		}
	}
}

void Datastore::redistribute_internal_edge(replica_index_node * parent, replica_index_node * edge, Communicator * comm, long long &replicated_records_count){
	int variables, predicate, q_const_val, column_idx, dest_machine;
	predicate_index_t::iterator p_it;
	so_index_t::iterator so_it1;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	vector<string> q_splits;
	vector<Data_record*>::iterator set_it;
	boost::unordered_set<int> unique;
	so_index_t * so_index = NULL;
	vector<vector<int> > to_send, to_recv;
	Data_record * tmp_record;


	if(edge->reversed && isVariable(edge->propagating))
		to_send.resize(1);
	else
		to_send.resize(comm->num_workers);

	predicate = atoi(parent->predicate.c_str());

	if(parent->reversed){
		column_idx = 0;
		if(isVariable(parent->binding)){
			if(isVariable(parent->propagating)){
				variables = V_BOTH;
				q_const_val = -1;
			}
			else{
				variables = V_OBJ;
				q_const_val = atoi(parent->propagating.c_str());
				so_index = &parent->index->s_index;
			}
		}
		else{
			variables = V_SUBJ;
			q_const_val = atoi(parent->binding.c_str());
			so_index = &parent->index->o_index;
		}
	}
	else{
		column_idx = 1;
		if(isVariable(parent->binding)){
			if(isVariable(parent->propagating)){
				variables = V_BOTH;
				q_const_val = -1;
			}
			else{
				variables = V_SUBJ;
				q_const_val = atoi(parent->propagating.c_str());
				so_index = &parent->index->o_index;
			}
		}
		else{
			variables = V_OBJ;
			q_const_val = atoi(parent->binding.c_str());
			so_index = &parent->index->s_index;
		}
	}

	if(edge->reversed && isVariable(edge->propagating)){
		if (variables == V_BOTH) {
			p_it = parent->index->p_index.find(predicate);
			if(p_it != parent->index->p_index.end()){
				for(set_it = p_it->second.begin() ; set_it != p_it->second.end(); set_it++){
					if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
						to_send[0].push_back((*set_it)->triple[column_idx]);
						unique.insert((*set_it)->triple[column_idx]);
					}
				}
			}
		} else {
			so_it1 = so_index->find(predicate);
			if(so_it1 != so_index->end()){
				so_it2 = so_it1->second.find(q_const_val);
				if(so_it2 != so_it1->second.end()){
					for(set_it = so_it2->second.begin() ; set_it != so_it2->second.end(); set_it++){
						if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
							to_send[0].push_back((*set_it)->triple[column_idx]);
							unique.insert((*set_it)->triple[column_idx]);
						}
					}
				}
			}
		}
	}
	else{
		if(!edge->reversed){
			if (variables == V_BOTH) {
				p_it = parent->index->p_index.find(predicate);
				if(p_it != parent->index->p_index.end()){
					for(set_it = p_it->second.begin() ; set_it != p_it->second.end(); set_it++){
						if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
							dest_machine = ((*set_it)->triple[column_idx]) % (comm->num_workers - 1) + 1;
							to_send[dest_machine].push_back((*set_it)->triple[column_idx]);
							unique.insert((*set_it)->triple[column_idx]);
						}
					}
				}
			} else {
				so_it1 = so_index->find(predicate);
				if(so_it1 != so_index->end()){
					so_it2 = so_it1->second.find(q_const_val);
					if(so_it2 != so_it1->second.end()){
						for(set_it = so_it2->second.begin() ; set_it != so_it2->second.end(); set_it++){
							if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
								dest_machine = ((*set_it)->triple[column_idx]) % (comm->num_workers - 1) + 1;
								to_send[dest_machine].push_back((*set_it)->triple[column_idx]);
								unique.insert((*set_it)->triple[column_idx]);
							}
						}
					}
				}
			}

		}
		else{//edge reversed but constant propagation of the edge LOCALITY_SUBJECT
			if (variables == V_BOTH) {
				p_it = parent->index->p_index.find(predicate);
				if(p_it != parent->index->p_index.end()){
					for(set_it = p_it->second.begin() ; set_it != p_it->second.end(); set_it++){
						if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
							dest_machine = (atoi(edge->propagating.c_str())) % (comm->num_workers - 1) + 1;
							to_send[dest_machine].push_back((*set_it)->triple[column_idx]);
							unique.insert((*set_it)->triple[column_idx]);
						}
					}
				}
			} else {
				so_it1 = so_index->find(predicate);
				if(so_it1 != so_index->end()){
					so_it2 = so_it1->second.find(q_const_val);
					if(so_it2 != so_it1->second.end()){
						for(set_it = so_it2->second.begin() ; set_it != so_it2->second.end(); set_it++){
							if(unique.find((*set_it)->triple[column_idx]) == unique.end()){
								dest_machine = (atoi(edge->propagating.c_str())) % (comm->num_workers - 1) + 1;
								to_send[dest_machine].push_back((*set_it)->triple[column_idx]);
								unique.insert((*set_it)->triple[column_idx]);
							}
						}
					}
				}
			}
		}
	}

	comm->mpi_all_to_all(to_send, to_recv);


	predicate = atoi(edge->predicate.c_str());
	if(isVariable(edge->binding) && isVariable(edge->propagating)){
		variables = 2;
	}
	else{
		variables = 1;
	}

	if(edge->reversed){
		so_index = &(this->main_index->o_index);
		column_idx = 0;
	}
	else{
		so_index = &(this->main_index->s_index);
		column_idx = 1;
	}

	if(!isVariable(edge->propagating))
		q_const_val = atoi(edge->propagating.c_str());


	to_send.clear();
	to_send.resize(comm->num_workers);
	for(unsigned j = 1 ; j < to_recv.size(); j++){
		for(unsigned i = 0 ; i < to_recv[j].size(); i++){
			so_it1 = so_index->find(predicate);
			if(so_it1 != so_index->end()){
				so_it2 = so_it1->second.find(to_recv[j][i]);
				if(so_it2 != so_it1->second.end()){
					for(set_it = so_it2->second.begin() ; set_it != so_it2->second.end(); set_it++){
						if(variables == 2 || ((*set_it)->triple[column_idx] == q_const_val)){
							to_send[j].push_back((*set_it)->triple[0]);
							to_send[j].push_back((*set_it)->triple[1]);
						}
					}
				}
			}
		}
	}
	to_recv.clear();
	to_recv.reserve(to_send.size());
	comm->mpi_all_to_all(to_send, to_recv);
	for (unsigned j = 1; j < to_recv.size(); j++) {
		for (unsigned l = 0; l < to_recv[j].size(); l += 2) {
			tmp_record = new Data_record(to_recv[j][l], to_recv[j][l+1]);
			edge->index->insert(predicate, tmp_record);
			replicated_records_count++;
		}
	}
}

void Datastore::join_two_queries_local(Local_Node & q1, Local_Node & q2, Local_Node &res, vector<int> &q1_data){
	int skip, predicate, q_const, q_const_val, other_col;
	vector<int> results;
	boost::unordered_map<int, vector<Data_record*> >::iterator so_it2;
	so_index_t::iterator so_it1;
	vector<Data_record*>::iterator set_it;
	so_index_t * index;
	boost::unordered_set<int> uniques;
	vector<pair<int, int> > join_cols;

	q1.get_join_node_and_cols(q2, join_cols, res);

	if (!q1.temp_query) {
		skip = 1;
		if (q1.variables == V_BOTH)
			skip = 2;
	} else {
		skip = q1.row.size();
	}

	for (unsigned i = 0; i < join_cols.size(); i++) {
		if(!q1.temp_query){
			if (join_cols[i].first == V_OBJ){
				if(q1.variables == V_BOTH){
					join_cols[i].first = 1;
				}
				else{
					join_cols[i].first = 0;
				}
			}
		}
		if (join_cols[i].second == V_OBJ){
			join_cols[i].second = 1;
		}
	}

	predicate = atoi(q2.row[1].c_str());
	if (join_cols[0].second == V_SUBJ) {
		index = &(q2.we->rin->index->s_index);
		q_const = 2;
		other_col = 1;
	} else {
		index = &(q2.we->rin->index->o_index);
		q_const = 0;
		other_col = 0;
	}

	q_const_val = -1;
	if (!isVariable(q2.row[q_const])) {
		q_const_val = atoi(q2.row[q_const].c_str());
	}
	results.clear();
	so_it1 = index->find(predicate);
	if (so_it1 != index->end()) {
		for (unsigned i = 0; i < q1_data.size(); i += skip) {
			so_it2 = so_it1->second.find(q1_data[i + join_cols[0].first]);
			if (so_it2 != so_it1->second.end()) {
				for (set_it = so_it2->second.begin();
						set_it != so_it2->second.end(); set_it++) {
					if (q_const_val == -1) {
						if (join_cols.size() == 1) {
							results.insert(results.end(), q1_data.begin() + i,
									q1_data.begin() + i + skip);
							results.push_back((*set_it)->triple[other_col]);
						} else {
							if (q1_data[i + join_cols[1].first]
							            == (*set_it)->triple[join_cols[1].second]) {
								results.insert(results.end(),
										q1_data.begin() + i,
										q1_data.begin() + i + skip);
							}
						}
					} else {
						if ((*set_it)->triple[other_col] == q_const_val) {
							results.insert(results.end(), q1_data.begin() + i,
									q1_data.begin() + i + skip);
						}
					}
				}
			}
		}
	}
	q1_data.swap(results);
}

long long Datastore::evaluate_without_communication(vector<Local_Node> &nodes, vector<int> &results, Local_Node &res_node){
    Local_Node q1, q2;

	for(unsigned i = 0 ; i < nodes.size(); i++){
		if(nodes[i].cardinality == 0){
			return 0;
		}
	}
	q1 = nodes[0];
	this->answer_sub_local(q1, q1.we->rin->index, results);
	for (unsigned i = 1; i <nodes.size(); i++) {
		q2 = nodes[i];
        join_two_queries_local(q1, q2, res_node, results);
		if(results.size() == 0){
			break;
		}
        q1 = res_node;
		q1.cardinality = results.size()/q1.row.size();
	}
    return results.size()/res_node.row.size();
}
