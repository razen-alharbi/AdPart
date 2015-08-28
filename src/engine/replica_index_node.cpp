#include "engine/replica_index_node.h"
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
replica_index_node::replica_index_node() {
	binding = "";
	propagating = "";
	predicate = "";
	reversed = false;
	parent = NULL;
	index = NULL;
	exact = false;
}

replica_index_node::replica_index_node(string &_binding, string &_predicate, string &_propagating, bool _reversed) {
	binding = _binding;
	propagating = _propagating;
	predicate = _predicate;
	reversed = _reversed;
	parent = NULL;
	index = NULL;
	exact = false;
}

replica_index_node::replica_index_node(walk_edge * edge){
	binding = edge->n1.label;
	propagating = edge->n2.label;
	predicate = edge->label;
	reversed = edge->nodes_reversed;
	parent = NULL;
	index = NULL;
	exact = false;
}

replica_index_node::replica_index_node(walk_edge * edge, bool exact){
	binding = edge->n1.label;
	propagating = edge->n2.label;
	predicate = edge->label;
	reversed = edge->nodes_reversed;
	parent = NULL;
	index = NULL;
	this->exact = exact;
}

string replica_index_node::print()const{
	return binding +" "+ predicate +" "+ propagating+ "("+toString(reversed)+")";
}

string replica_index_node::print2() const{
	string result;
	if(parent != NULL)
		result = parent->print2()+"--->";
	result += "("+binding + ", " + predicate + ", " + propagating+")["+toString(children.size())+"]";
	return result;
}

std::size_t replica_index_node_hash::operator()(replica_index_node const * x) const
{
	std::size_t seed = 0;
	boost::hash_combine(seed, x->predicate);
	return seed;
}
bool replica_index_node_equal::operator()(replica_index_node const* e1,replica_index_node const* e2) const{
	if(e1->exact){
		if((e1->predicate == e2->predicate) && (e1->reversed == e2->reversed)){
			if(isVariable(e1->binding) && isVariable(e2->binding)){
				if(isVariable(e1->propagating) && isVariable(e2->propagating)){
					return true;
				}
				else{
					return e1->propagating == e2->propagating;
				}
			}
			else if(isVariable(e1->propagating) && isVariable(e2->propagating)){
				return e1->binding == e2->binding;
			}
			else
				return false;
		}
		else
			return false;
	}
	else{
		if((e1->predicate == e2->predicate) && (e1->reversed == e2->reversed)){
			if(isVariable(e1->binding) && isVariable(e2->binding)){
				if(isVariable(e1->propagating) && isVariable(e2->propagating)){
					return true;
				}
				else if(!isVariable(e1->propagating) && isVariable(e2->propagating)){
					return true;
				}
				else{
					return e1->propagating == e2->propagating;
				}
			}
			else if(isVariable(e1->propagating) && isVariable(e2->propagating)){
				if(!isVariable(e1->binding) && isVariable(e2->binding)){
					return true;
				}
				else{
					return e1->binding == e2->binding;
				}
			}
			else{
				return (e1->binding == e2->binding) && (e1->propagating == e2->propagating);
			}
		}
		else{
			return false;
		}
	}
}
