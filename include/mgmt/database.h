#ifndef DATABASE
#define DATABASE

#include "common/utils.h"
#include <QStringList>
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
struct part_conf{
    QString cluster_conf;
    int num_parts;

    part_conf(QString _cluster_conf, int _num_parts){
        cluster_conf = _cluster_conf;
        num_parts = _num_parts;
    }
};

class Database {
public:

    Database();
    Database(string input);
    Database(string name, string inputFile, string directory, string query_folder);
    ~Database();
public:
    string name;
    string inputFile;
    string directory;
    string dic_folder;
    string data_folder;
    string parts_folder;
    string query_folder;
    int encoded;
    int num_triples;
    int num_preds;
    int num_verts;
    vector<part_conf> parts;

    string print();
    string printFormatted();
    bool conf_exists(QString _cluster_conf, int _num_parts);
    void removeConf(QString confText);
    bool usesClusterConfiguration(string &toBeRemoved);
};

#endif // DATABASE

