#include "engine/logger.h"
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
Logger::Logger(){

}

Logger::Logger(string &adhash_home) {
	time_t rawtime;
	time(&rawtime);
	long myId;
	myId = MPI::COMM_WORLD.Get_rank();
    logFile = adhash_home+"/logs/log" + toString(myId) + ".txt";
	ofstream logStream(logFile.c_str());
	logStream << "Logging activity at " << ctime(&rawtime) << endl;
	logStream << "Process id: " << getpid() << endl << endl;
}

void Logger::writeToLog(string record, bool to_screen){
#ifdef enable_logging
    ofstream logStream(logFile.c_str(), ios::app);
    logStream << record << endl;
    logStream.flush();
    logStream.close();
#endif
    if(to_screen){
        printf("%s\n", record.c_str());
        //cout.flush();
    }
}

