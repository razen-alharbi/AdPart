//#define RETURN_RESULTS
//#define REVERSE_WALK
//#define MAX_SUM
//#define BATCH_UPDATE
#define PERCENTILE
//#define DEBUG
//#define COMM_VOLUME
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <stdio.h>
#include <mpi.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <algorithm>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <set>
#include <unordered_set>
#include <queue>
#include "assert.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <QProgressDialog>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QTimer>
#include <QSplashScreen>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
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

#define BUFFERSIZE 1048576 //has to be multiple of 16
#define RECORDSIZE 16
typedef struct data_block data_block_t;
struct data_block{
	char data[BUFFERSIZE];
	int ptr;
};
using namespace std;

typedef int ll;
//---------------------------------------------------------------CONSTANTS-----------------------------------------------------
const string intermediate_delim = "|";
const string part_string = "********************************************************************";
const string query_delim = "#EOQ#";
const string cmd_delimiter = "^";
const string VAR_STRING = "?#MV";
//---------------------------------------------------------------ENUMS---------------------------------------------------------
enum hash_type {
	HS,
	HO,
	RAND,
	HSO
};
enum trav_col {
	/* 0 	*/SUBJ,
	/* 1 	*/PRED,
	/* 2 	*/OBJ
};
enum PLAN_OP {SEMI_JOIN, STAR, PARALLEL, PHD_PARALLEL, PHD_SEMI_JOIN, EMPTY, FAIL};
enum PREPARE_AHEAD {PREPARE_RANDOM, PREPARE_LOCALITY, NO_PREPARE, PREPARE_LOCALITY_SUBJECT};

#endif /* CONSTANTS_H_ */

