#ifndef PREDICATE_STAT_H
#define PREDICATE_STAT_H

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
using namespace std;

class predicate_stat {
public:
	predicate_stat();
	predicate_stat(int predicate_text);
	predicate_stat(string predicate_text);
	virtual ~predicate_stat();
	string print(bool all);
	string print2();
	long long subject_total_degree;
	long long subject_uniques;
	float subject_score;
	float pred_per_subj;
	long long object_total_degree;
	long long object_uniques;
	float object_score;
	float pred_per_obj;
	long long total_count;
	int predicate_text;
};

#endif
