#include "mgmt/database.h"
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
Database::Database(){

}

Database::Database(string name, string inputFile, string directory, string query_folder){
    this->name = name;
    this->inputFile = inputFile;
    this->directory = directory+"/";
    this->dic_folder = this->directory+"dic/";
    this->data_folder = this->directory+"data/";
    this->parts_folder = this->directory+"parts/";
    if(!query_folder.empty())
        this->query_folder = query_folder+"/";
    else
        this->query_folder = "";
    this->encoded = 0;
    this->num_preds = 0;
    this->num_triples = 0;
    this->num_verts = 0;
}

Database::Database(string input){
    QStringList splits = QString(input.c_str()).split('#');
    this->name = splits[0].toStdString();
    this->num_triples = atoi(splits[1].toStdString().c_str());
    this->num_preds = atoi(splits[2].toStdString().c_str());
    this->num_verts = atoi(splits[3].toStdString().c_str());
    this->encoded = atoi(splits[4].toStdString().c_str());
    this->inputFile = splits[5].toStdString();
    this->directory = splits[6].toStdString();
    this->dic_folder = splits[7].toStdString();
    this->data_folder = splits[8].toStdString();
    this->parts_folder = splits[9].toStdString();
    this->query_folder = splits[10].toStdString();
    for(int i = 11 ; i < splits.size() ; i+=2){
        parts.push_back(part_conf(splits[i], atoi(splits[i+1].toStdString().c_str())));
    }
}

Database::~Database(){

}

string Database::print(){
    string result;
    result+=this->name+"#"+toString(this->num_triples)+"#"+toString(this->num_preds)+"#"+toString(this->num_verts)+"#"+toString(this->encoded)+"#"+this->inputFile+"#"+this->directory+"#"+this->dic_folder+"#"+this->data_folder+"#"+this->parts_folder+"#"+this->query_folder;
    for(unsigned int i = 0 ; i < parts.size(); i++){
        result += "#"+parts[i].cluster_conf.toStdString()+"#"+toString(parts[i].num_parts);
    }
    return result;
}

string Database::printFormatted(){
    string result;
    result+="Database name: " +this->name+"\n#Triples: "+toString(this->num_triples)+"\n#Predicates: "+toString(this->num_preds)+"\n#Vertices: "+toString(this->num_verts)
            +"\n#Partitioning Configurations: "+toString(parts.size());
    return result;
}

bool Database::conf_exists(QString cluster_conf, int num_parts){
    for(unsigned int i = 0 ; i < parts.size(); i++){
        if((cluster_conf == parts[i].cluster_conf) && (num_parts == parts[i].num_parts))
            return true;
    }
    return false;
}

void Database::removeConf(QString confText){
    QStringList splits = confText.split(",");
    vector<part_conf> tmp;
    int num_parts = splits[1].toInt();
    for(unsigned int i = 0 ; i < parts.size() ; i++){
        if((parts[i].cluster_conf == splits[0]) && (parts[i].num_parts == num_parts))
            continue;
        tmp.push_back(parts[i]);
    }
    parts.swap(tmp);
}

bool Database::usesClusterConfiguration(string &toBeRemoved){
    for(unsigned int i = 0 ; i < parts.size(); i++){
        if((toBeRemoved == parts[i].cluster_conf.toStdString()))
            return true;
    }
    return false;
}
