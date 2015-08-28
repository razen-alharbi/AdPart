#ifndef PARALLELPARTITIONER_H
#define PARALLELPARTITIONER_H

#include <common/utils.h>
#include <QThread>
#include "parallel_partitioner/partitioner_communicator.h"
#include "common/profiler.h"
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
class PartitionerMaster;
namespace Ui {
class ParallelPartitioner;
}

class ParallelPartitioner : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;
public:
    ParallelPartitioner(string &db_directory, string &input_file_name);
    ~ParallelPartitioner();
    Ui::ParallelPartitioner *ui;
    PartitionerMaster * master;

signals:
    void operate();

private slots:
    void updatePB(int v);
};


class PartitionerMaster: public QObject
{
    Q_OBJECT
public:

    PartitionerMaster(ParallelPartitioner * caller, string &db_directory, string &input_file_name);
    ~PartitionerMaster();
    PartitionerCommunicator* communicator;
    void finish_go_home();
private:
    void load_and_partition_data();
    void send_data_to_worker(data_block_t * blocks, int dest);
    Profiler profiler;
    string db_directory;
    string input_file_name;
    string data_file;
    ParallelPartitioner * caller;
    long long num_rec;
    long long processed_so_far;

private slots:
    void start();
signals:
    void updateProgress(int m);
};

#endif // PARALLELPARTITIONER_H
