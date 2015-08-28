#ifndef UTILS
#define UTILS

#include "common/constants.h"

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

void throwException(string exc);
bool isVariable( string const &name);
bool isLiteral(string &name);
string trim(const string & s);
double gini_coef(vector<long long>& vec);
void split_string(string& text, string delim, vector<string>& splits);
double timespec_to_double(timespec &ts);

struct dict_entry{
    string text;
    bool literal;
};

template<class T>
std::string toString(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<class T>
string print_list(const vector<T>& vec, string delim) {

    string printout = "";

    for (unsigned i = 0; i < vec.size(); i++) {
        printout += toString(vec[i]);

        if (i < vec.size() - 1)
            printout += delim;
    }

    return printout;
}

int b_search(vector<dict_entry> &data, string &value);
#endif // UTILS

