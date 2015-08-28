#include "parallel_partitioner/parallelpartitioner.h"
#include "ui_parallelpartitioner.h"
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
ParallelPartitioner::ParallelPartitioner(string &db_directory, string &input_file_name) :
    QMainWindow(0),
    ui(new Ui::ParallelPartitioner)
{
    ui->setupUi(this);
    this->ui->progressBar->setRange(0,100);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    master = new PartitionerMaster(this, db_directory, input_file_name);
    master->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), master, SLOT(deleteLater()));
    connect(this, SIGNAL(operate()), master, SLOT(start()));
    connect(master, SIGNAL(updateProgress(int)), this, SLOT(updatePB(int)));
    workerThread.start();
    emit operate();
}

ParallelPartitioner::~ParallelPartitioner()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

void ParallelPartitioner::updatePB(int v){
    if(v == 1000){
        master->finish_go_home();
        QApplication::quit();
    }
    this->ui->progressBar->setValue(v);
}


PartitionerMaster::PartitionerMaster(ParallelPartitioner * caller, string &db_directory, string &input_file_name){
    this->caller = caller;
    this->communicator = new PartitionerCommunicator();
    this->db_directory = db_directory;
    this->input_file_name = input_file_name;
    this->data_file = this->db_directory+"data/"+this->input_file_name+"_encoded";
    this->num_rec = 0;
    this->processed_so_far = 0;
}

PartitionerMaster::~PartitionerMaster(){
    delete this->communicator;
}

void PartitionerMaster::start(){
    int src;
    net_cmd cmd;
    vector<string> params;
    ifstream file(this->data_file, ios::binary | ios::ate);
    bool failed = false;
    long long file_size = file.tellg();
    file.close();
    this->num_rec = file_size/RECORDSIZE;
    this->caller->ui->statusBar->showMessage("Starting Master!");
    for(int i = 1 ; i < this->communicator->num_workers ; i++){
        src = this->communicator->receive_command(cmd, params);
        if(cmd == CMD_NOT_READY)
            failed = true;
        this->caller->ui->statusBar->showMessage(QString(string("Worker "+toString(src)+" is ready!").c_str()));
    }
    params.clear();

    if(failed){
        cout<<"failed";
    }
    else{
        this->communicator->send_command(CMD_STARTING_PARTITIONING, params, SEND_TO_ALL);
        this->load_and_partition_data();
        cout<<"success";
    }
    emit updateProgress(1000);
}

void PartitionerMaster::load_and_partition_data() {
    this->caller->ui->statusBar->showMessage("Data partitioning started!");
    Profiler profiler;
    profiler.startTimer("partition_rdf_data");

    FILE * pFile;
    long int num_rec, records_per_iteration, num_iterations;
    char * buffer;
    size_t result;
    int num_parts = this->communicator->get_num_workers()-1;

    pFile = fopen(this->data_file.c_str(), "rb");
    if(pFile==NULL){
        cout<<("Could not open the file")<<endl;
        exit (2);
    }

    buffer = (char*) malloc(sizeof(char)*BUFFERSIZE);
    if(buffer == NULL){
        cout<<("Memory error")<<endl;
        exit (2);
    }

    fseek (pFile , 0 , SEEK_END);
    num_rec = ftell (pFile)/RECORDSIZE;
    records_per_iteration = BUFFERSIZE/RECORDSIZE;
    num_iterations = ceil(1.0*num_rec/records_per_iteration);
    rewind (pFile);
    data_block_t* blocks = (data_block_t*) malloc(num_parts*sizeof(data_block_t));
    for(int i = 0 ; i < num_parts; i++){
        blocks[i].ptr = 0;
    }
    int dest, subj;
    for(unsigned int i = 0 ; i < num_iterations; i++){
        result = fread(buffer, RECORDSIZE, records_per_iteration, pFile);
        for(unsigned int j = 0 ; j < result*RECORDSIZE ; j+=RECORDSIZE){
            memcpy(&subj, buffer+j, sizeof(int));
            dest = subj%num_parts;

            if((blocks[dest].ptr+RECORDSIZE) <= BUFFERSIZE){
                memcpy(blocks[dest].data+blocks[dest].ptr, buffer+j, RECORDSIZE);
                blocks[dest].ptr += RECORDSIZE;
            }
            else{
                send_data_to_worker(blocks, dest);
                blocks[dest].ptr = 0;
                memcpy(blocks[dest].data+blocks[dest].ptr, buffer+j, RECORDSIZE);
                blocks[dest].ptr += RECORDSIZE;
            }
        }
    }
    for(int i = 0 ; i < num_parts; i++){
        send_data_to_worker(blocks, i);
    }

    fclose(pFile);
    free(buffer);
    free(blocks);
    profiler.pauseTimer("partition_rdf_data");
    this->caller->ui->statusBar->showMessage(QString(string("Done with data partitioning in " + toString(profiler.readPeriod("partition_rdf_data")) + " sec").c_str()));
    profiler.clearTimer("partition_rdf_data");
}
void PartitionerMaster::finish_go_home() {
    this->caller->ui->statusBar->showMessage("Finalizing MPI, Please wait ...");
    vector<string> params;
    this->communicator->send_command(CMD_KILL, params, SEND_TO_ALL);
    MPI::COMM_WORLD.Barrier();
    MPI::Finalize();
    cout.flush();
}

void PartitionerMaster::send_data_to_worker(data_block_t * blocks, int dest){
     MPI::Request request;
    request = this->communicator->mpi_comm.Isend(blocks[dest].data, blocks[dest].ptr/sizeof(int), MPI::INT, dest+1, TAG_DISTRIB_DATA);
    request.Wait();
    processed_so_far += blocks[dest].ptr/RECORDSIZE;
    emit updateProgress(1.0*this->processed_so_far/num_rec*100);
}
