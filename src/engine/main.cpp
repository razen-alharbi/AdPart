#include "common/profiler.h"
#include "engine/client.h"
#include <QApplication>
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

int main(int argc, char *argv[])
{
    int rank;
    string db_folder, file_name, query_folder, adhash_home, db_name;
    MPI::Init(argc, argv);
    rank = MPI::COMM_WORLD.Get_rank();

    if (argc < 5) {
        throwException("Usage: engine <AdHash_HOME> <db_name> <db_folder> <file_name> [<query_folder>]");
    }

    adhash_home = string(argv[1]);
    db_name = string(argv[2]);
    db_folder = string(argv[3]);
    file_name = string(argv[4]);
    if(rank == 0){
        if(argc == 6)
            query_folder = string(argv[5]);
        QApplication a(argc, argv);
        MasterGUI w(db_name, adhash_home, db_folder, file_name, query_folder);
        w.move(QApplication::desktop()->screen()->rect().center() - w.rect().center());
        w.show();

        return a.exec();
    }
    else{
        Client client(adhash_home, db_folder, file_name);
    }
    return 0;
}

