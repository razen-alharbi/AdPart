#include "parallel_partitioner/parallelpartitioner.h"
#include "common/utils.h"
#include <QApplication>
#include "common/profiler.h"
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
using namespace std;

int main(int argc, char** argv) {
    // variables declarations
    int rank;
    string db_directory, input_file_name;

    MPI::Init(argc, argv);
    rank = MPI::COMM_WORLD.Get_rank();
    if (argc < 3) {
        throwException("Usage: ParallelEncoder <db_directory> <input_file_name>");
    }
    db_directory = string(argv[1]);
    input_file_name = string(argv[2]);

    if(rank == 0){
        QApplication a(argc, argv);
        ParallelPartitioner w(db_directory, input_file_name);
        w.move(QApplication::desktop()->screen()->rect().center() - w.rect().center());
        w.show();

        return a.exec();
    }
    else{
        PartitionerClient client(db_directory, input_file_name);
        client.start();
    }
    return 0;
}
