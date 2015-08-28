#include "engine/predicate_stat.h"
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
predicate_stat::predicate_stat() {
	this->predicate_text = -1;
	this->object_total_degree = 0;
	this->subject_total_degree = 0;
	this->object_uniques = 0;
	this->subject_uniques = 0;
	this->object_score = 0;
	this->subject_score = 0;
	this->total_count = 0;
	this->pred_per_subj = 0;
	this->pred_per_obj = 0;
}
predicate_stat::predicate_stat(int predicate_text) {
	this->predicate_text = predicate_text;
	this->object_total_degree = 0;
	this->subject_total_degree = 0;
	this->object_uniques = 0;
	this->subject_uniques = 0;
	this->object_score = 0;
	this->subject_score = 0;
	this->total_count = 0;
	this->pred_per_subj = 0;
	this->pred_per_obj = 0;
}

predicate_stat::predicate_stat(string predicate_text) {
	vector<string> splits;
	split_string(predicate_text, " ", splits);
	this->predicate_text = atoi(splits[0].c_str());
	total_count = atoll(splits[1].c_str());
	subject_total_degree = atoll(splits[2].c_str());
	subject_uniques = atoll(splits[3].c_str());
	subject_score = atof(splits[4].c_str());
	pred_per_subj = atof(splits[5].c_str());
	object_total_degree = atoll(splits[6].c_str());
	object_uniques = atoll(splits[7].c_str());
	object_score = atof(splits[8].c_str());
	pred_per_obj = atof(splits[9].c_str());
}

predicate_stat::~predicate_stat() {

}

string predicate_stat::print(bool all) {
	string result;
	if (all)
		result = "[" + toString(subject_total_degree) + ", "
				+ toString(subject_uniques) + ", "
				+ toString(this->subject_score) + ", " + toString(pred_per_subj)
				+ "][" + toString(predicate_text) + ", " + toString(total_count)
				+ "][" + toString(object_total_degree) + ", "
				+ toString(object_uniques) + ", " + toString(this->object_score)
				+ ", " + toString(pred_per_obj) + "]";
	else
		result = "[" + toString(this->subject_score) + ", "
				+ toString(pred_per_subj) + "][" + toString(predicate_text)
				+ "][" + toString(this->object_score) + ", "
				+ toString(pred_per_subj) + "]";
	return result;
}

string predicate_stat::print2() {
	string result;
	result = toString(predicate_text) + " " + toString(total_count)+ " " +toString(subject_total_degree) + " "+ toString(subject_uniques) + " "
			+ toString(this->subject_score) + " " + toString(pred_per_subj)	+ " " + toString(object_total_degree) + " "+ toString(object_uniques) + " "
				+ toString(this->object_score)	+ " " + toString(pred_per_obj);
	return result;
}
