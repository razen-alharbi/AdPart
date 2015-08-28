#ifndef QUERY_H
#define QUERY_H
#include "engine/node.h"
#include "engine/SPARQLParser.hpp"
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
enum TYPE{SELECT, DELETE, INSERT};

class Query {

public:
    Query();
    Query(SPARQLParser parser);
	string prefix;

	string querystring;

	// list of projections
	vector<string> projections;

	// list of nodes
    vector<Node> string_nodes;
    vector<Node> nodes;

	// list of variables
	vector<string> variables;

	TYPE type;
	int qid;
	string print();
    bool supported;
};

#endif
