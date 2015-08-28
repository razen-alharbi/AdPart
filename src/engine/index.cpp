#include "engine/index.h"
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
Index::Index(){
	records_count = 0;
}
Index::~Index(){
	predicate_index_t::iterator it;
	vector<Data_record*>::iterator s_it;

	for(it = p_index.begin(); it != p_index.end() ;it++){
		for(s_it = it->second.begin(); s_it != it->second.end(); s_it++){
			delete *s_it;
		}
		it->second.clear();
	}
	p_index.clear();
	s_index.clear();
	o_index.clear();
}
void Index::insert(int pred, Data_record* record){
	p_index[pred].push_back(record);
	s_index[pred][record->triple[0]].push_back(record);
	o_index[pred][record->triple[1]].push_back(record);
	records_count++;
}
