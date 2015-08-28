#ifndef COMM_H
#define COMM_H
#include "engine/logger.h"
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
	/* 4 */	CMD_COMPUTE_PREDICATE_STATS,
	/* 5 */	CMD_CLEAN_VERTS_DATA,
	/* 6 */ CMD_GET_TRIPLE_LOAD,
	/* 7 */ CMD_EXCHANGE_VERTS_STATS,
	/* 8 	*/CMD_GET_SUBQUERY_CARDINALITY,
	/* 9 	*/CMD_JOIN_WITHOUT_COMMUNICATION,
	/* 10 	*/CMD_JOIN_WITH_COMMUNICATION,
	/* 11 	*/CMD_SOLVE_STAR_QUERY,
	/* 12 	*/CMD_START_PHD,
	/* 13 	*/CMD_EXECUTE_QUERY_IN_PARALLEL,
	/* 14 	*/CMD_STORE_STATS,
	/* 15 	*/CMD_EVICT,
	/* 16 	*/CMD_COMM_VOL,
	/* 2 	CMD_PING,
	 3 	CMD_GET_TRIPLE_LOAD,
	 4 	CMD_GET_QUERY_MASSES,
	 5	CMD_PREPARE_JOIN_COL,
	 6	CMD_DISTRIBUTE_JOIN_COLUMN,
	 7	CMD_BUILD_CANDIDATE_LISTS,
	 8	CMD_DISTRIBUTE_CANDIDATE_LISTS,
	 9	CMD_PARALLEL_EXEC_JOIN,
	 10	CMD_CLEANUP,
	 11	CMD_GET_QUERY_RESULT,
	 12	CMD_START_PHD,
	 13	CMD_REMOVE_REPLICATED_DATA,
	 14	CMD_EXECUTE_QUERY_IN_PARALLEL,
	 15	CMD_COMPUTE_PREDICATE_STATS,
	 16	CMD_CLEAN_VERTS_DATA,
	 17	CMD_GET_DATA_COMM,
	 18	CMD_EXPLORE,
	 19	CMD_JOIN,
	 20	CMD_SOLVE_FINALIZE_STAR,
	 21	CMD_FINALIZE*/
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

class Communicator {
public:
	Communicator();
	Communicator(MPI::Intracomm mpi_com);
	~Communicator();
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
