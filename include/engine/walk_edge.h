#ifndef WALK_EDGE_H_
#define WALK_EDGE_H_
#include "common/utils.h"
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
class walk_edge;
struct walk_edge_hash:std::unary_function<walk_edge*, std::size_t>
{
	std::size_t operator()(walk_edge const * x) const;
};
struct walk_edge_equal:std::binary_function<walk_edge*, walk_edge*, bool>
{
	bool operator()(walk_edge const* e1,	walk_edge const* e2) const;
};

struct walk_node {
	string label;
	float score;
	walk_node(string& lbl, float sc) {
		label = lbl;
		score = sc;
	}
	walk_node() {
		label = "";
		score = 0;
	}
};
class replica_index_node;
class walk_edge {
public:
	walk_node n1;
	walk_node n2;
	string label;
	walk_edge* parent;
	vector<walk_edge*> children;
	bool nodes_reversed;
	replica_index_node * rin;

	walk_edge(walk_node node1, walk_node node2, string lbl) {
		n1 = node1;
		n2 = node2;
		label = lbl;
		nodes_reversed = false;
		parent = NULL;
		rin = NULL;
	}
	walk_edge(string node1, string node2, string lbl) {
		n1.label = node1;
		n2.label = node2;
		label = lbl;
		nodes_reversed = false;
		parent = NULL;
		rin = NULL;
	}
	walk_edge(string node1, string lbl, string node2, bool _revered, walk_edge * _parent) {
		n1.label = node1;
		n2.label = node2;
		label = lbl;
		nodes_reversed = _revered;
		parent = _parent;
		rin = NULL;
	}
	walk_edge() {
		nodes_reversed = false;
		parent = NULL;
		rin = NULL;
	}
	walk_edge(walk_edge* edge) {
		nodes_reversed = edge->nodes_reversed;
		n1.label = edge->n1.label;
		n2.label = edge->n2.label;
		label = edge->label;
		parent = NULL;
		rin = NULL;
	}
	bool subOf(walk_edge* edge){
		if((label == edge->label) && (nodes_reversed == edge->nodes_reversed)){
			if(isVariable(n1.label) && isVariable(edge->n1.label)){
				if(isVariable(n2.label) && isVariable(edge->n2.label)){
					return true;
				}
				else if(!isVariable(n2.label) && isVariable(edge->n2.label)){
					return true;
				}
				else{
					return n2.label == edge->n2.label;
				}
			}
			else if(isVariable(n2.label) && isVariable(edge->n2.label)){
				if(!isVariable(n1.label) && isVariable(edge->n1.label)){
					return true;
				}
				else{
					return n1.label == edge->n1.label;
				}
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	string print() const{
		string result;
		if (nodes_reversed)
			result = "("+n2.label + ", " + label + ", " + n1.label+"), Children: "+toString(children.size());
		else
			result = "("+n1.label + ", " + label + ", " + n2.label+"), Children: "+toString(children.size());
		if(parent != NULL)
			result += ", Parent: "+parent->print();
		return result;
	}
	string print2() const{
		string result;
		if(parent != NULL)
			result = parent->print2()+"--->";
		result += "("+n1.label + ", " + label + ", " + n2.label+")["+toString(children.size())+"]";
		return result;
	}
	void get_path(vector<string> &path) const{
		if(parent != NULL)
			parent->get_path(path);
		path.push_back(toString(nodes_reversed)+intermediate_delim+n1.label+intermediate_delim+label +intermediate_delim+n2.label);
	}
	string encode() const{
		if(nodes_reversed)
			return n2.label+"|"+label+"|"+n1.label;
		else
			return n1.label+"|"+label+"|"+n2.label;
	}
};


struct mycomparison
{
	mycomparison(){
	}
	bool operator() (const walk_edge* e1, const walk_edge* e2) const
	{
#ifdef REVERSE_WALK
		if (e1->n1.score > e2->n1.score)
			return true;
		if (e1->n1.score == e2->n1.score && e1->n2.score > e2->n2.score)
			return true;
		return false;
#else
		if (e1->n1.score < e2->n1.score)
			return true;
		if (e1->n1.score == e2->n1.score && e1->n2.score < e2->n2.score)
			return true;
		return false;
#endif
	}
};

struct order_comparison
{
	order_comparison(){
	}
	bool operator() (const walk_edge* e1, const walk_edge* e2) const
	{
		if(e1->label != e2->label){
			return e1->label < e2->label;
		}
		else{
			if((e1->n1.label == e2->n1.label)){
				return e1->n2.label < e2->n2.label;
			}
			if((e1->n1.label == e2->n2.label)){
				return e1->n2.label < e2->n1.label;
			}
			return e1->n1.label < e2->n1.label;
		}
	}
};
#endif /* WALK_EDGE_H_ */
