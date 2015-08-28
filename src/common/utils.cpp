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
void throwException(string exc) {
    long myId;
    myId = MPI::COMM_WORLD.Get_rank();
    if (myId == 0)
        cout << myId << " - RUNTIME ERROR: " << exc << endl;
    else
        cout<<("RUNTIME ERROR: " + exc)<<endl;
    MPI::Finalize();
    exit(0);
}

bool isVariable(const string &name) {

    if (name[0] == '?')
        return true;

    return false;
}

bool isLiteral(string &name) {

    if (name[0] == '"')
        return true;

    return false;
}

string trim(const string & s) {

    string str(s);
    string::size_type pos = str.find_last_not_of(' ');

    if (pos != string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(" \t\r\n");
        if (pos != string::npos)
            str.erase(0, pos);
    } else
        str.erase(str.begin(), str.end());

    return str;
}
double gini_coef(vector<long long>& vec) {
    double gini = 0.0, mean = 0.0;

    // calculate the mean
    for (unsigned i = 1; i < vec.size(); i++)
        mean += vec[i];

    mean /= vec.size()-1;

    for (unsigned i = 1; i < vec.size(); i++)
        for (unsigned j = 1; j < vec.size(); j++)
            gini += fabs(vec[i] - vec[j]);

    gini /= mean;
    gini /= (2 * (vec.size()-1) * (vec.size()-1));

    return gini;
}

void split_string(string& text, string delim, vector<string>& splits) {
    size_t  start = 0, end = 0;
    splits.clear();
    if(text.length() > 0){
        while ( end != string::npos)
        {
            end = text.find( delim, start);
            splits.push_back( text.substr( start,	(end == string::npos) ? string::npos : end - start));
            start = (   ( end > (string::npos - delim.size()) )
                        ?  string::npos  :  end + delim.size());
        }
    }
}
double timespec_to_double(timespec &ts){
    return ts.tv_sec + ts.tv_nsec/1000000000.0;
}

int b_search(vector<dict_entry> &data, string &value){
    int left = 0, right = data.size()-1, mid;

    while (left <= right) {
        mid = (int) ((left + right) / 2);
        if (value == data[mid].text) {
            return mid;
        }
        else if (value > data[mid].text)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}
