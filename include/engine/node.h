#ifndef NODE_H
#define	 NODE_H

#include "common/utils.h"
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
enum node_variables {
	/* 0 	*/V_SUBJ,
	/* 1 	*/V_BOTH,
	/* 2 	*/V_OBJ
};

class Node {
public:

	Node();
	Node(vector<string> &tokens);
	Node(string &subquery);

	bool operator<(const Node& n2) const {
		return row[0] < n2.row[0];
	}

	vector<string> row;

	// indicates whether it has a path predicate
	//bool hasPathPredicate;

	int num_variables;
	vector<string> getColumnNames();
	void buildFromArray(vector<string> tokens);
	void get_join_node_and_cols(Node &n2, vector<pair<int, int> > &join_cols, Node &res);
	vector<string> getVarNames();
	string encode_query();
	static Node parseNode(string str);
	void analyze_variables();
	long long cardinality;
	bool temp_query;
	string print();
	node_variables variables;
};

#endif	/* NODE_HPP */

