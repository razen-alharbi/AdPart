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

#include "engine/local_node.h"
Local_Node::Local_Node() : Node(){
	num_subj = 0;
	num_obj = 0;
}
Local_Node::Local_Node(string s, string p, string o){
	row[0] = s;
	row[1] = p;
	row[2] = o;
}
Local_Node::Local_Node(walk_edge * edge){
	so_index_t::iterator so_it;
	predicate_index_t::iterator p_it;
	boost::unordered_map<int, vector<Data_record*> >::iterator s_it;
	we = edge;
	temp_query = false;
	row.clear();
	row.resize(3);
	num_variables = 0;
	cardinality = 0;
	num_obj = 0;
	num_subj = 0;
	if(edge->nodes_reversed){
		row[0] = edge->n2.label;
		row[1] = edge->label;
		row[2] = edge->n1.label;
	}
	else{
		row[0] = edge->n1.label;
		row[1] = edge->label;
		row[2] = edge->n2.label;
	}
	for(unsigned i = 0 ; i < row.size(); i++){
		if(isVariable(row[i]))
			num_variables++;
	}
	cardinality = 0;
	variables = V_BOTH;
	if(!temp_query)
		this->analyze_variables();

	int predicate = atoi(row[1].c_str());
	p_it = edge->rin->index->p_index.find(predicate);
	if(p_it == edge->rin->index->p_index.end()){
		cardinality = 0;
		num_subj = 0;
		num_obj = 0;
	}
	else{
		if(variables == V_BOTH){
			p_it = edge->rin->index->p_index.find(predicate);
			cardinality = p_it->second.size();
			num_subj = edge->rin->index->s_index[predicate].size();
			num_obj = edge->rin->index->o_index[predicate].size();
		}
		else if(variables == V_SUBJ){
			num_obj = 1;
			so_it = edge->rin->index->o_index.find(predicate);
			s_it = so_it->second.find(atoi(row[2].c_str()));
			if(s_it != so_it->second.end()){
				/*if(row[1] == "3" && row[2] == "2208" && MPI::COMM_WORLD.Get_rank() == 3){
					cout<<s_it->second.size()<<" "<<this->we->rin->index->p_index[predicate].size()<<" "<<this->we->rin->print()<<endl;
					for(boost::unordered_set<Data_record*>::iterator ttt = this->we->rin->index->p_index[predicate].begin() ; ttt !=  this->we->rin->index->p_index[predicate].end() ; ttt++){
						cout<<(*ttt)->triple[0]<<" "<<predicate<<" "<<(*ttt)->triple[1]<<endl;
					}
				}*/

				cardinality = s_it->second.size();
				num_subj =  cardinality;
			}
		}
		else{
			so_it = edge->rin->index->s_index.find(predicate);
			s_it = so_it->second.find(atoi(row[0].c_str()));
			if(s_it != so_it->second.end()){
				cardinality = s_it->second.size();
				num_obj = cardinality;
			}
		}
	}
}

string Local_Node::print(){
	return print_list(row, "|")+" "+toString(num_subj)+" "+toString(cardinality)+" "+toString(num_obj);
}

string Local_Node::print2(){
	return print_list(row, "|");
}
