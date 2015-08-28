#include "mgmt/cluster_configuration.h"
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
ClusterConfiguration::ClusterConfiguration(){

}

ClusterConfiguration::ClusterConfiguration(string name){
    this->name = name;
}

ClusterConfiguration::~ClusterConfiguration(){

}

string ClusterConfiguration::print(){
    string results = this->name+"#";
    for(unsigned i = 0 ; i < hosts.size()-1; i++){
        results += hosts[i]+"#";
    }
    if(!hosts.empty())
        results += hosts[hosts.size()-1];
    return results;
}
