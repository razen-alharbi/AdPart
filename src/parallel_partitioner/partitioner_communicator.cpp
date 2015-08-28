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
PartitionerCommunicator::PartitionerCommunicator(){
    this->comm_volume = 0;
    this->comm_tag = GENERIC_TAG;
    this->mpi_comm = MPI::COMM_WORLD;
    this->num_workers = this->mpi_comm.Get_size();
    this->rank = this->mpi_comm.Get_rank();
}

PartitionerCommunicator::PartitionerCommunicator(MPI::Intracomm mpi_com){
    this->comm_volume = 0;
    this->mpi_comm = mpi_com;
    this->comm_tag = GENERIC_TAG;
    this->num_workers = this->mpi_comm.Get_size();
    this->rank = this->mpi_comm.Get_rank();
}

PartitionerCommunicator::~PartitionerCommunicator(){

}

void PartitionerCommunicator::mpi_send_string(string &msg, int dest) {

    int msgLength = msg.length();

    if (dest == SEND_TO_ALL) {
        this->comm_volume += msgLength*(this->mpi_comm.Get_size()-1);
        for (int i = 1; i < this->mpi_comm.Get_size(); i++) {
            this->mpi_comm.Send(&msgLength, 1, MPI::INT, i, this->comm_tag);
            this->mpi_comm.Send(&(msg[0]), msgLength, MPI::CHAR, i, this->comm_tag);
        }
    } else {
        this->comm_volume += msgLength;
        this->mpi_comm.Send(&msgLength, 1, MPI::INT, dest, this->comm_tag);
        this->mpi_comm.Send(&(msg[0]), msgLength, MPI::CHAR, dest, this->comm_tag);
    }
}

void PartitionerCommunicator::mpi_receive_string(int src, string& msg) {

    int msgLength;

    this->mpi_comm.Recv(&msgLength, 1, MPI::INT, src, this->comm_tag);

    char* recvBuf = new char[msgLength + 1];
    this->mpi_comm.Recv(recvBuf, msgLength, MPI::CHAR, src, this->comm_tag);
    recvBuf[msgLength] = '\0';

    msg = string(recvBuf);
    delete recvBuf;
}

long long PartitionerCommunicator::mpi_receive_long(int src) {
    long long msg;
    this->mpi_comm.Recv(&msg, 1, MPI::LONG_LONG, src, this->comm_tag);
    return msg;
}

double PartitionerCommunicator::mpi_receive_double(int src) {
    double msg;
    this->mpi_comm.Recv(&msg, 1, MPI::DOUBLE, src, this->comm_tag);
    return msg;
}

void PartitionerCommunicator::mpi_send_int_vector(vector<int> &data, int dest) {
    this->comm_volume += data.size()*sizeof(MPI::INT);
    this->mpi_comm.Send(&data[0], data.size(), MPI::INT, dest, this->comm_tag);
}

void PartitionerCommunicator::mpi_receive_int_vector(vector<int> &data, int from){
    this->mpi_comm.Recv(&data[0], data.size(), MPI::INT, from, this->comm_tag);
}
void PartitionerCommunicator::mpi_send_long(long long msg, int dest) {
    if (dest == SEND_TO_ALL) {
        this->comm_volume += (this->mpi_comm.Get_size()-1) * sizeof(MPI::LONG_LONG);
        for (int i = 1; i < this->mpi_comm.Get_size(); i++) {
            this->mpi_comm.Send(&msg, 1, MPI::LONG_LONG, i, this->comm_tag);
        }
    } else {
        this->comm_volume += sizeof(MPI::LONG_LONG);
        this->mpi_comm.Send(&msg, 1, MPI::LONG_LONG, dest, this->comm_tag);
    }
}

void PartitionerCommunicator::mpi_send_double(double msg, int dest) {
    if (dest == SEND_TO_ALL) {
        this->comm_volume += (this->mpi_comm.Get_size()-1) * sizeof(MPI::DOUBLE);
        for (int i = 1; i < this->mpi_comm.Get_size(); i++) {
            this->mpi_comm.Send(&msg, 1, MPI::DOUBLE, i, this->comm_tag);
        }
    } else {
        this->comm_volume += sizeof(MPI::DOUBLE);
        this->mpi_comm.Send(&msg, 1, MPI::DOUBLE, dest, this->comm_tag);
    }
}

void PartitionerCommunicator::send_command(net_cmd cmd, vector<string> &params, int dest) {
    string cmd_string = toString(cmd);
    cmd_string += cmd_delimiter;
    cmd_string += toString(params.size());
    cmd_string += cmd_delimiter;

    //cout<<this->mpi_comm.Get_rank()<<" "<<cmd_string<<endl;

    for (unsigned i = 0; i < params.size(); i++) {
        cmd_string += params[i];
        cmd_string += cmd_delimiter;
    }

    if (dest == SEND_TO_ALL) {
        //for (int i = 1; i < this->mpi_comm.Get_size(); i++)
        mpi_send_string(cmd_string, SEND_TO_ALL);
    } else {
        mpi_send_string(cmd_string, dest);
    }
}

int PartitionerCommunicator::receive_command(net_cmd& cmd, vector<string>& cmd_params) {
    string cmd_string, param;
    vector<string> splits;
    int src, msgLength;
    unsigned int num_params = 0;
    MPI::Status stat;
    char* recvBuf;

    this->mpi_comm.Recv(&msgLength, 1, MPI::INT, MPI::ANY_SOURCE, this->comm_tag, stat);
    src = stat.Get_source();
    recvBuf = new char[msgLength + 1];

    this->mpi_comm.Recv(recvBuf, msgLength, MPI::CHAR, src, this->comm_tag);
    recvBuf[msgLength] = '\0';
    cmd_string = string(recvBuf);

    delete recvBuf;


    // parse returns net_cmd itself
    split_string(cmd_string, cmd_delimiter, splits);
    if(splits.size() < 2){
        throwException("ASSERTION FAILED: recieved < 2 tokens");
    }
    else{
        cmd = net_cmd(atoi(splits[0].c_str()));
        num_params = atoi(splits[1].c_str());
    }
    cmd_params.clear();

    // parsing parameters
    if(splits.size()-3 != num_params){
        throwException("ASSERTION FAILED: cmd: " + toString(cmd)+ " params_size: " + toString(cmd_params.size()));
    }
    string tmp_param;
    for (unsigned int i = 0; i < num_params; i++) {
        param = splits[i+2];
        cmd_params.push_back(param);
    }
    return src;
}
string PartitionerCommunicator::translate_command_to_string(net_cmd cmd) {
    string command;
    switch (cmd) {
    case CMD_LOAD_DATA:
        command = "CMD_LOAD_DATA";
        break;
    case CMD_PING:
        command = "CMD_PING";
        break;
    case CMD_READY:
        command = "CMD_READY";
        break;
    case CMD_KILL:
        command = "CMD_KILL";
        break;
    case CMD_STARTING_PARTITIONING:
        command = "CMD_STARTING_PARTITIONING";
        break;
    case CMD_NOT_READY:
        command = "CMD_NOT_READY";
        break;
    default:
        command = toString(cmd);
        break;
    }
    return command;
}

int PartitionerCommunicator::get_worker_id(){
    return this->rank;
}

int PartitionerCommunicator::get_num_workers(){
    return num_workers;
}

void PartitionerCommunicator::wait_for_clients() {
    vector<string> params;
    net_cmd cmd;
    send_command(CMD_PING, params, SEND_TO_ALL);
    for (int m = 1; m < get_num_workers(); m++)
        receive_command(cmd, params);
}

void PartitionerCommunicator::mpi_all_to_all(vector<vector<int> >&src, vector<vector<int> > &recvd) {
    int n = get_num_workers();
    MPI::Request* all_requests = new MPI::Request[n];
    vector<int> received_sizes(n);
    vector<int> sent_sizes(n);

    recvd.clear();
    recvd.resize(n);
    if (src.size() == 1) {
        for(int i = 1 ; i < n ; i++){
            sent_sizes[i] = src[0].size();
        }
        recvd[get_worker_id()] = src[0];
    }
    else{
        recvd[get_worker_id()] = src[get_worker_id()];
        for(int i = 1 ; i < n ; i++){
            sent_sizes[i] = src[i].size();
        }
    }

    // first we do all-to-all for the sizes
    this->comm_volume += (n-1) * sizeof(MPI::INT);
    for (int i = 1; i < n; i++) {
        if (i == get_worker_id())
            continue;
        all_requests[i] = this->mpi_comm.Isend(&(sent_sizes[i]), 1, MPI::INT, i,TAG_DISTRIB_SIZES);
    }

    // then we receive the sizes from everyone
    for (int i = 1; i < n; i++) {
        received_sizes[i] = 0;
        if (i == get_worker_id())
            continue;
        this->mpi_comm.Recv(&(received_sizes[i]), 1, MPI::INT, i,TAG_DISTRIB_SIZES);
    }

    // good things come to those who wait
    for (int i = 1; i < n; i++) {
        if (i == get_worker_id())
            continue;
        all_requests[i].Wait();
    }

    // now we send the data itself
    for (int i = 1; i < n; i++) {
        if (i == get_worker_id())
            continue;

        if (src.size() == 1) {
            this->comm_volume += src[0].size() * sizeof(MPI::INT);
            all_requests[i] = this->mpi_comm.Isend(&src[0][0], src[0].size(),	MPI::INT, i, TAG_DISTRIB_DATA);
        } else {
            this->comm_volume += src[i].size() * sizeof(MPI::INT);
            all_requests[i] = this->mpi_comm.Isend(&src[i][0], src[i].size(), MPI::INT, i, TAG_DISTRIB_DATA);
        }
    }
    for (int i = 1; i < n; i++) {
        if (i == get_worker_id())
            continue;
        recvd[i].resize(received_sizes[i]);
    }
    // then we receive the candidate lists from everyone
    for (int i = 1; i < n; i++) {

        if (i == get_worker_id())
            continue;
        this->mpi_comm.Recv(&recvd[i][0], received_sizes[i],	MPI::INT, i, TAG_DISTRIB_DATA);
    }

    // good things come to those who wait
    for (int i = 1; i < n; i++) {
        if (i == get_worker_id())
            continue;
        all_requests[i].Wait();
    }

}


void PartitionerCommunicator::sync(){
    this->mpi_comm.Barrier();
}
