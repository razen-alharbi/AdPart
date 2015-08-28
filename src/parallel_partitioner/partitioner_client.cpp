#include "parallel_partitioner/partitioner_client.h"
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

PartitionerClient::PartitionerClient(string &db_directory, string &input_file_name){
    this->rank = MPI::COMM_WORLD.Get_rank();
    this->communicator = new PartitionerCommunicator();
    this->input_file_name = input_file_name;
    this->data_path = db_directory+"parts/"+toString(this->communicator->get_num_workers()-1)+"/";
    this->dic_path = db_directory+"dic/";
    this->dest_file = this->data_path+"/"+this->input_file_name+toString(this->communicator->get_worker_id());
}

PartitionerClient::~PartitionerClient(){

}

void PartitionerClient::start(){
    vector<string> params;
    char buffer[1024];
    char *line_p;
    string command;
    FILE * fp;
    command = "mkdir -p "+this->data_path+" 2>/dev/null ; mkdir -p "+this->dic_path+" 2>/dev/null ; cd "+this->data_path+" 2>/dev/null ; pwd";
    fp = popen(command.c_str(), "r");

    if (!fp)
    {
        this->communicator->send_command(CMD_NOT_READY, params, 0);
    }

    line_p = fgets(buffer, sizeof(buffer), fp);
    string tmp = string(line_p);
    if(memcmp(this->data_path.c_str(), tmp.c_str(), this->data_path.size()-1) != 0){
        this->communicator->send_command(CMD_NOT_READY, params, 0);
    }
    else{
        this->communicator->send_command(CMD_READY, params, 0);
    }
    pclose(fp);
    this->wait_for_commands();
}
int PartitionerClient::get_rank(){
    return rank;
}

void PartitionerClient::wait_for_commands(){
    net_cmd cmd;
    vector<string> params;
    while (1) {
        this->communicator->receive_command(cmd, params);
        if(cmd == CMD_KILL){
            MPI::COMM_WORLD.Barrier();
            MPI::Finalize();
            break;
        }
        else{
            this->start_receiving_data();
        }
    }
}

void PartitionerClient::start_receiving_data(){
    char * buffer = (char*) malloc(sizeof(char)*BUFFERSIZE);
    MPI::Status status;
    int received, maxSize;

    maxSize = BUFFERSIZE/sizeof(int);
    FILE* pFile = fopen(this->dest_file.c_str(), "wb");
    while(true){
        this->communicator->mpi_comm.Recv(buffer, BUFFERSIZE/sizeof(int), MPI::INT, 0, TAG_DISTRIB_DATA, status);
        received = status.Get_count(MPI::INT);
        fwrite(buffer, sizeof(int), received, pFile);
        fflush(pFile);
        if(received != maxSize){
            break;
        }
    }
    fclose(pFile);
}
