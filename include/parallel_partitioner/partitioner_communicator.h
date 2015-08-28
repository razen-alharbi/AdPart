#ifndef COMM_H
#define COMM_H
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
enum net_cmd {
	/* 0 */ CMD_LOAD_DATA,
	/* 1 */ CMD_PING,
	/* 2 */ CMD_READY,
	/* 3 */	CMD_KILL,
    /* 4 */	CMD_STARTING_PARTITIONING,
    /* 5 */ CMD_NOT_READY,
};

enum special_tags {
	/* 0 	*/GENERIC_TAG,
	/* 1 	*/TAG_DISTRIB_SIZES,
	/* 2 	*/TAG_DISTRIB_DATA,
	/* 3 	*/TAG_DISTRIB_CL_SIZES,
	/* 4 	*/TAG_DISTRIB_CL
};

#define SEND_TO_ALL -1
#define RECV_FROM_ANY -1

const size_t max_mpi_len = 512 * 1024;

class PartitionerCommunicator {
public:
	PartitionerCommunicator();
	PartitionerCommunicator(MPI::Intracomm mpi_com);
	~PartitionerCommunicator();
	void mpi_send_string(string& msg, int dest);

	void mpi_receive_string(int src, string& msg);

	void mpi_send_long(long long msg, int dest);
	void mpi_send_int_vector(vector<int> &data, int dest);
	long long mpi_receive_long(int src);
	void mpi_receive_int_vector(vector<int> &data, int from);
	void mpi_send_double(double msg, int dest);

	double mpi_receive_double(int src);

	void send_command(net_cmd cmd, vector<string> &params, int dest);

	int receive_command(net_cmd& cmd, vector<string>& params);

	string translate_command_to_string(net_cmd cmd);

	int get_worker_id();

	int get_num_workers();

	void wait_for_clients();
	void sync();
	int comm_tag;
	void mpi_all_to_all(vector<vector<int> >&src, vector<vector<int> > &recvd);
	MPI::Intracomm mpi_comm;
	int num_workers;
	int rank;
	long long comm_volume;
};

#endif
