#ifndef MASTERGUI_H
#define MASTERGUI_H

#include "common/xpm_images.h"
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include "engine/statisticsManager.h"
#include "engine/communicator.h"
#include "engine/master_executer.h"
#include "engine/plan.h"
#include <QFile>
#include "common/gnuplot-iostream.h"
#include "engine/SPARQLParser.hpp"
#include "engine/SPARQLLexer.hpp"
#include "engine/comparisontab.h"
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
class Query_Index;
class Statistics_Manager;
struct Exec_info;

namespace Ui {
class MasterGUI;
}

class MasterGUI : public QMainWindow
{
    Q_OBJECT

public:
    MasterGUI(string &db_name, string &adhash_home, string &db_folder, string &file_name, string &query_folder, QWidget *parent = 0);
    ~MasterGUI();
    Communicator* communicator;
    Statistics_Manager * manager;
    Query_Index * q_index;
    vector<string> preds_data;
    vector<dict_entry> verts_data;
    boost::unordered_map<string, int> preds_map;
    void update_results(vector<int> &pos, vector<int> &results, int row_size);
    void prepare_results_table(vector<string> &projection, int rows);
    Logger logger;
    string adhash_home;
private:
    Ui::MasterGUI *ui;
    QLabel * masterStatImageLabel;
    QLabel * clientsStatImageLabel;
    QLabel * masterStatLabel;
    QLabel * clientsStatLabel;
    string db_folder;
    string query_folder;
    string file_name;

    string db_name;
    int currentRow;
    QSize gnu_label_size;

    QFutureWatcher<void> dictionary_watcher;
    QFuture<void> dictionary_future;
    QFutureWatcher<void> clients_watcher;
    QFuture<void> clients_future;
    void loadDic();
    void prepare_clients();
    void compute_statistics();
    void output_stats_to_file(int start, int end);
    void load_statistics();
    Profiler profiler;
    bool is_master_ready;
    bool are_clients_ready;
    void update_exec_results(Exec_info &info, bool load);
    void exec_load(vector<Query> &queries);
    vector<pair<int,double> > cumm_times;
    vector<pair<int,double> > cumm_times_na;
    void plot_figure_adaptive();
    void plot_figure_none_adaptive();
    void closeEvent(QCloseEvent *);
    void load_queries(string queryFile, vector<Query>& queries);
private slots:
    void doneLoadingDic();
    void clientsReady();
    void browseQueryDir();
    void updateQList(QString path);
    void queryFileSelected(QString file);
    void executeQuery();
    void updateProgressFigure(double time);
signals:
    void updateProgress(double time);
};

#endif // MASTERGUI_H
