#include "engine/query_count.h"
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
query_count::query_count() {
	predicate = "";
	reversed = false;
	count = 0;
	parent = NULL;
}

query_count::query_count(string &_predicate, bool _reversed) {
	predicate = _predicate;
	reversed = _reversed;
	count = 0;
	parent = NULL;
}

query_count::query_count(walk_edge * edge){
	predicate = edge->label;
	reversed = edge->nodes_reversed;
	count = 0;
	parent = NULL;
}
string query_count::print()const{
	return predicate + "("+toString(reversed)+")";
}

std::size_t query_count_hash::operator()(query_count const * x) const
{
	std::size_t seed = 0;
	boost::hash_combine(seed, x->predicate);
	return seed;
}
bool query_count_equal::operator()(query_count const* e1,	query_count const* e2) const
{
	if((e1->predicate == e2->predicate) && (e1->reversed == e2->reversed)){
		return true;
	}
	else{
		return false;
	}
}

void query_count::update_proactivity_info(walk_edge * edge, unsigned int& var_count,map<string, float> &walk_scores){
	//Potential Optimization use priority queue
	int max_const_count = numeric_limits<int>::min();
	string max_const;
	float corresponding_score;
	//cout<<count<<" "<<edge->print2()<<" will be "<<endl;
	if(!isVariable(edge->n1.label)){//constant core
		if(binding_count.find(edge->n1.label) == binding_count.end())
			binding_count[edge->n1.label] = 0;
		binding_count[edge->n1.label]++;
		boost::unordered_map<string, int>::iterator it;
		for(it = binding_count.begin() ; it != binding_count.end(); it++){
			if(it->second > max_const_count){
				max_const = it->first;
				max_const_count = it->second;
			}
		}
		if((((float)max_const_count)/count) > 0.6){
			corresponding_score = walk_scores[edge->n1.label];
			//walk_scores.erase(edge->n1.label);
			walk_scores[max_const] = corresponding_score;
			edge->n1.label = max_const;
		}
		else{
			walk_scores[VAR_STRING+toString(var_count)] = walk_scores[edge->n1.label];
			//walk_scores.erase(edge->n1.label);
			edge->n1.label = VAR_STRING+toString(var_count++);
		}
	}
	else if((edge->children.size() == 0) && !isVariable(edge->n2.label)){//connected to leaf
		if(propagating_count.find(edge->n2.label) == propagating_count.end())
			propagating_count[edge->n2.label] = 0;
		propagating_count[edge->n2.label]++;

		boost::unordered_map<string, int>::iterator it;
		for(it = propagating_count.begin() ; it != propagating_count.end(); it++){
			if(it->second > max_const_count){
				max_const = it->first;
				max_const_count = it->second;
			}
		}
		if((((float)max_const_count)/count) > 0.6){
			corresponding_score = walk_scores[edge->n2.label];
			//walk_scores.erase(edge->n2.label);
			walk_scores[max_const] = corresponding_score;
			edge->n2.label = max_const;
		}
		else{
			walk_scores[VAR_STRING+toString(var_count)] = walk_scores[edge->n2.label];
			//walk_scores.erase(edge->n2.label);
			edge->n2.label = VAR_STRING+toString(var_count++);
		}
	}
	//cout<<edge->print2()<<endl;
}
