#include "engine/statisticsManager.h"

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

Statistics_Manager::Statistics_Manager(MasterGUI * master, unsigned nodes) {
	this->master = master;
	workers = nodes;
	this->original_triples_load.resize(this->workers);
	this->replicated_triples_load = vector<long long>(this->workers, 0);
	this->total_triples_load = vector<long long>(this->workers, 0);
	this->replication_ratio = 0;
	this->gini = 0;
	this->max_predicate_id = -1;
	this->max_vertex_id = -1;
}

Statistics_Manager::~Statistics_Manager(){
	//delete this->qi;
}

void Statistics_Manager::update_triple_load() {
    master->logger.writeToLog(part_string, false);
    master->logger.writeToLog("Triples Load!", false);
	vector<string> cmd_params;
	this->master->communicator->send_command(CMD_GET_TRIPLE_LOAD, cmd_params, SEND_TO_ALL);

	this->original_triples_load[0] = 0;
	long long triples;
	for (int i = 1; i < this->workers; i++) {
		triples = this->master->communicator->mpi_receive_long(i);
		this->original_triples_load[i] = triples;
		this->original_triples_load[0] += this->original_triples_load[i];
		this->total_triples_load[i] = triples;
		this->total_triples_load[0] += this->total_triples_load[i];
	}
    master->logger.writeToLog(print_list(this->original_triples_load, " ,"), false);
	this->gini = gini_coef(this->total_triples_load);
}

void Statistics_Manager::add_predicates(vector<string>& preds){
	pred_stat_t::iterator it;

	for(unsigned i = 0; i < preds.size(); i++){
		it = this->predicates_stats.find(preds[i]);
		if(it == this->predicates_stats.end()){
			this->predicates_stats[preds[i]] = predicate_stat(atoi(preds[i].c_str()));
		}
	}
}

void Statistics_Manager::analyze_predicates(){
#ifdef PERCENTILE
	analyze_predicates_elimination();
#else
	analyze_predicates_avg();
#endif
}

void Statistics_Manager::analyze_predicates_elimination(){
	float object_sum = 0, object_avg = 0, object_stdev = 0, max_deviation = 0, current_dev = 0, subject_sum = 0, subject_avg = 0, subject_stdev = 0;
	pred_stat_t::iterator it;
	string suspect;
	bool first = true, found_suspect = false;
	pred_stat_t to_be_removed;

	//Analyzing Object scores first as they are the source of outliers.
    master->logger.writeToLog("Removing outliers using object scores", true);
	while(first || found_suspect){
		first = false;
		found_suspect = false;
		max_deviation = 0;
		object_sum = 0;
		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			object_sum += it->second.object_score;
		}
		object_avg = object_sum/predicates_stats.size();
		object_sum = 0;
		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			object_sum += pow(it->second.object_score-object_avg,2);
		}
		object_stdev = sqrt(object_sum/(predicates_stats.size()-1));

		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			current_dev = abs(it->second.object_score-object_avg)/object_stdev;
			if((current_dev > 3) && (current_dev > max_deviation)){
				suspect = it->first;
				max_deviation = current_dev;
				found_suspect = true;
			}
		}

		if(found_suspect && ((0.002 * predicates_stats.size()) < 0.5)){
            master->logger.writeToLog("Removing outliar predicate "+toString(suspect), true);
			to_be_removed[suspect] = predicates_stats[suspect];
			to_be_removed[suspect].object_score = 0;
			predicates_stats.erase(suspect);
		}
	}

	//Analyzing subject scores now.
    master->logger.writeToLog("Removing outliers using subject scores", true);
	first = true;
	while(first || found_suspect){
		first = false;
		found_suspect = false;
		max_deviation = 0;
		subject_sum = 0;
		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			subject_sum += it->second.subject_score;
		}
		subject_avg = subject_sum/predicates_stats.size();
		subject_sum = 0;
		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			subject_sum += pow(it->second.subject_score-subject_avg,2);
		}
		subject_stdev = sqrt(subject_sum/(predicates_stats.size()-1));

		for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
			current_dev = abs(it->second.subject_score-subject_avg)/subject_stdev;
			if((current_dev > 3) && (current_dev > max_deviation)){
				suspect = it->first;
				max_deviation = current_dev;
				found_suspect = true;
			}
		}

		if(found_suspect && ((0.002 * predicates_stats.size()) < 0.5)){
            master->logger.writeToLog("Removing outliar predicate "+toString(suspect), true);
			to_be_removed[suspect] = predicates_stats[suspect];
			to_be_removed[suspect].subject_score = 0;
			predicates_stats.erase(suspect);
		}
	}
	for(it = to_be_removed.begin(); it != to_be_removed.end(); it++){
		predicates_stats[it->first] = it->second;
	}

	/*for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
		cout<<it->first<<"--->"<<it->second.print(false)<<endl;
	}*/
}

void Statistics_Manager::analyze_predicates_avg(){
	float left_average = 0, right_average = 0, left_sum = 0, right_sum = 0, left_stdev, right_stdev;
	pred_stat_t::iterator it;
	//vector<string> to_be_removed;

	for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
        master->logger.writeToLog(toString(it->second.subject_score)+" "+it->first+" "+toString(it->second.object_score), false);
		left_sum += it->second.subject_score;
		right_sum += it->second.object_score;
	}
	left_average = left_sum/predicates_stats.size();
	right_average = right_sum/predicates_stats.size();
	left_sum = 0;
	right_sum = 0;
	for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
		left_sum += pow(it->second.subject_score-left_average,2);
		right_sum += pow(it->second.object_score-right_average,2);
	}
	left_stdev = sqrt(left_sum/(predicates_stats.size()-1));
	right_stdev = sqrt(right_sum/(predicates_stats.size()-1));
    master->logger.writeToLog("Left ("+toString(left_average)+", "+toString(left_stdev)+"), Right ("+toString(right_average)+", "+toString(right_stdev)+")", false);

	for(it = predicates_stats.begin(); it != predicates_stats.end(); it++){
        master->logger.writeToLog(it->first+"("+toString(it->second.subject_score)+", "+toString(it->second.object_score)+")", false);
		if((it->second.subject_score > left_average+3*left_stdev) || (it->second.object_score > right_average+3*right_stdev)){// || it->first == "3")
			//to_be_removed.push_back(it->first);
			it->second.subject_score = 0;
			it->second.object_score = 0;
		}
	}

	/*for(unsigned i = 0 ; i < to_be_removed.size(); i++){
        master->logger.writeToLog("Removing "+to_be_removed[i]+" from predicate stats!", true);
		predicates_stats.erase(to_be_removed[i]);
	}*/
}

