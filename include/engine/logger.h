#ifndef LOGGER_H
#define LOGGER_H

#include "common/utils.h"

#define enable_logging
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
class Logger{
public:
    Logger();
    Logger(string &adHash_home);
    ~Logger(){

    }

    string logFile;
    void writeToLog(string record, bool to_screen);
};
#endif /* LOGGER_H */

