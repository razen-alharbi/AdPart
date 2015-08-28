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
std::size_t walk_edge_hash::operator()(walk_edge const * x) const
{
	std::size_t seed = 0;
	boost::hash_combine(seed, x->label);
	return seed;
}
bool walk_edge_equal::operator()(walk_edge const* e1,	walk_edge const* e2) const
{
	if(e1->label != e2->label)
		return false;
	else{
		if(e1->nodes_reversed == e2->nodes_reversed){
			return ((e1->n1.label == e2->n1.label) && (e1->n2.label == e2->n2.label));
		}
		else{
			return ((e1->n1.label == e2->n2.label) && (e1->n2.label == e2->n1.label));
		}
	}
}
