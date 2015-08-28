#ifndef PROFILER_H_
#define PROFILER_H_

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

timespec diff(timespec start, timespec end);
timespec add(timespec t1, timespec t2);
bool operator<(timespec &t1, timespec &t2);
class Profiler {

private:

	map<string, timespec> timers;
	map<string, timespec> timerPeriods;
	map<string, bool> running;

public:

	void startTimer(string name);
	void pauseTimer(string name);

	void clearTimer(string name);

	double readPeriod(string name);
	bool isRunning(string name);

};

#endif /* PROFILER_H_ */
