#ifndef INDEX_H
#define INDEX_H

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
struct Data_record {
	int triple[2];

	Data_record(int subject, int object) {
		triple[0] = subject;
		triple[1] = object;
	}
};


struct Intermediate_record {
	vector<int> triple;

	Intermediate_record(vector<int> data) {
		triple.reserve(data.size());
		triple.assign(data.begin(), data.end());
	}
};

typedef boost::unordered_map<int, vector<int> > simple_index_t;
typedef boost::unordered_map<int, vector<Data_record*> > predicate_index_t;
typedef boost::unordered_map<int, boost::unordered_map<int, vector<Data_record*> > > so_index_t;
class Index {

public:
	Index();
	~Index();
	void insert(int pred, Data_record* record);
	predicate_index_t p_index;
	so_index_t s_index;
	so_index_t o_index;
	long long records_count;
private:

};
#endif /* INDEX_H */
