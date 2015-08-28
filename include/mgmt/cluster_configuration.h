#ifndef CONFIGURATION
#define CONFIGURATION

#include "common/utils.h"
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
class ClusterConfiguration {
public:

    ClusterConfiguration();
    ClusterConfiguration(string name);
    ~ClusterConfiguration();
public:
    string name;
    vector<string> hosts;
    vector<string> up_hosts;
    vector<string> down_hosts;
    string print();
};

#endif // CONFIGURATION

