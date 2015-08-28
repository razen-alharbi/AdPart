#ifndef PARTITIONER_CLIENT_H
#define PARTITIONER_CLIENT_H

#include "common/utils.h"
#include "parallel_partitioner/partitioner_communicator.h"
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
class PartitionerClient{
public:
    PartitionerClient(string &db_directory, string &input_file_name);
    ~PartitionerClient();
	void start();
	int get_rank();
	void start_receiving_data();
private:
	void wait_for_commands();
	int rank;
	string dest_file;
    string data_path;
    string dic_path;
    string input_file_name;
    PartitionerCommunicator * communicator;
};

#endif  /*PARTITIONER_CLIENT_H*/

