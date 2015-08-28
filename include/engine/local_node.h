#ifndef LOCAL_NODE_H
#define	 LOCAL_NODE_H

#include "engine/node.h"
#include "engine/walk_edge.h"
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
class Local_Node : public Node {
public:
	Local_Node();
	Local_Node(string s, string p, string o);
	Local_Node(walk_edge * edge);

	string print();
	string print2();
	long long num_subj;
	long long num_obj;
	walk_edge * we;
};

#endif	/* NODE_HPP */

