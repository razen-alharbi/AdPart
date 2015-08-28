#include "engine/mastergui.h"
#include "ui_mastergui.h"
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
MasterGUI::MasterGUI(string &db_name, string &adhash_home, string &db_folder, string &file_name, string &query_folder, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MasterGUI)
{
    //setup parameters
    this->db_name = db_name;
    this->db_folder = db_folder;
    this->query_folder = query_folder;
    this->file_name = file_name;
    this->adhash_home = adhash_home;
    //engine related
    this->communicator = new Communicator();
    this->manager = new Statistics_Manager(this, MPI::COMM_WORLD.Get_size());
    this->q_index = new Query_Index(this);
    this->is_master_ready = false;
    this->are_clients_ready = false;
    this->logger = Logger(this->adhash_home);
    remove(string(this->adhash_home+"/images/progress.png").c_str());
    //UI
    ui->setupUi(this);
    this->currentRow = 0;
    masterStatLabel = new QLabel("Master:");
    clientsStatLabel = new QLabel("Clients:");
    masterStatImageLabel = new QLabel();
    clientsStatImageLabel = new QLabel();
    this->ui->resultTable->verticalHeader()->setVisible(false);
    this->ui->resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->resultTable->setSelectionMode(QAbstractItemView::SingleSelection);
    this->ui->resultTable->setStyleSheet("QTableView {selection-background-color: rgb(179,203,21);}");
    this->ui->resultTable->setGeometry(QApplication::desktop()->screenGeometry());
    this->ui->tabs->setTabText(this->ui->tabs->indexOf(this->ui->main_tab), QString(this->db_name.c_str()));
    this->ui->tabs->addTab(new ComparisonTab(this), "Comparison");
    QHeaderView* header = this->ui->resultTable->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    this->ui->gnuLabel->setVisible(false);
    QPixmap mypix(down_xpm);
    masterStatImageLabel->setPixmap(mypix);
    clientsStatImageLabel->setPixmap(mypix);
    ui->statusBar->addPermanentWidget(masterStatLabel);
    ui->statusBar->addPermanentWidget(masterStatImageLabel);
    ui->statusBar->addPermanentWidget(clientsStatLabel);
    ui->statusBar->addPermanentWidget(clientsStatImageLabel);
    ui->label->setPixmap(QPixmap(QString(string(this->adhash_home+"/images/header-kaust.jpg").c_str())));

    //startLogic
    connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browseQueryDir()));
    connect(ui->queryPath, SIGNAL(textChanged(QString)), this, SLOT(updateQList(QString)));
    connect(ui->qList, SIGNAL(currentIndexChanged(QString)), this, SLOT(queryFileSelected(QString)));
    if(!this->query_folder.empty()){
        this->ui->queryPath->setText(QString(this->query_folder.c_str()));
        this->ui->queryPath->setToolTip(QString(this->query_folder.c_str()));
    }
    connect(ui->execButton, SIGNAL(clicked()), this, SLOT(executeQuery()));
    connect(&dictionary_watcher, SIGNAL(finished()), this, SLOT(doneLoadingDic()));
    connect(&clients_watcher, SIGNAL(finished()), this, SLOT(clientsReady()));
    connect(this, SIGNAL(updateProgress(double)), this, SLOT(updateProgressFigure(double)));
    dictionary_future = QtConcurrent::run(this, &MasterGUI::loadDic);
    dictionary_watcher.setFuture(dictionary_future);
    clients_future = QtConcurrent::run(this, &MasterGUI::prepare_clients);
    clients_watcher.setFuture(clients_future);
}

MasterGUI::~MasterGUI()
{
    delete ui;
}

void MasterGUI::closeEvent(QCloseEvent *){
    vector<string> params;
    logger.writeToLog("Master sending KILL signals!", false);
    this->communicator->send_command(CMD_KILL, params, SEND_TO_ALL);
    this->communicator->sync();
    logger.writeToLog("Master terminated gracefully!", false);
    MPI::Finalize();
}

void MasterGUI::loadDic(){
    Profiler p;
    p.startTimer("all_dic");
    char value[1000000];
    string predsFile = this->db_folder+"dic/"+this->file_name+"_preds.dic";
    FILE * pFile = fopen(predsFile.c_str(), "rb");
    vector<int> aux(2);
    fseek (pFile , 0 , SEEK_END);
    long long size = ftell (pFile);
    long long read = 0;
    rewind (pFile);
    this->masterStatLabel->setText("Master: Loading dictionaries (1/3)");
    while(read < size){
        fread(&aux[0], sizeof(int), 2, pFile);
        fread(value, sizeof(char), aux[1], pFile);
        read += 8+aux[1];
        if(aux[0] >= preds_data.size())
            preds_data.resize(aux[0]+1);
        value[aux[1]] = '\0';
        preds_data[aux[0]] = string(value);
    }

    this->masterStatLabel->setText("Master: Loading dictionaries (2/3)");
    for(unsigned int i = 0 ; i < preds_data.size(); i++){
        preds_map[preds_data[i]] = i;
    }

    this->masterStatLabel->setText("Master: Loading dictionaries (3/3)");
    string vertsFile = this->db_folder+"dic/"+this->file_name+"_verts.dic";
    dict_entry entry;
    pFile = fopen(vertsFile.c_str(), "rb");
    fseek (pFile , 0 , SEEK_END);
    size = ftell (pFile);
    read = 0;
    rewind (pFile);
    while(read < size){
        fread(&aux[0], sizeof(int), 2, pFile);
        fread(value, sizeof(char), aux[1], pFile);
        read += 8+aux[1];
        if(aux[0] >= verts_data.size())
            verts_data.resize(aux[0]+1);
        value[aux[1]] = '\0';
        entry.text = string(value);
        entry.literal = isLiteral(entry.text);

        verts_data[aux[0]] = entry;
    }

    p.pauseTimer("all_dic");
    logger.writeToLog("Master: Dictionaries loaded in "+toString(p.readPeriod("all_dic")), false);
    p.clearTimer("all_dic");
}

void MasterGUI::doneLoadingDic(){
    this->masterStatLabel->setText("Master: Ready");
    QPixmap mypix(up_xpm);
    masterStatImageLabel->setPixmap(mypix);
    this->is_master_ready = true;
    if(is_master_ready && are_clients_ready){
        this->gnu_label_size.setHeight(this->ui->resultBox->height());
        this->gnu_label_size.setWidth(this->ui->resultBox->width());
        if(this->ui->qList->count() > 0)
            this->ui->execButton->setEnabled(true);
    }
}

void MasterGUI::prepare_clients(){
    int src;
    net_cmd cmd;
    vector<string> params;
    int num_workers = this->communicator->get_num_workers()-1, counter = 0, current_max_pred, current_max_verts;
    logger.writeToLog("Starting Master!", false);

    this->profiler.startTimer("loading_data_and_queries");
    logger.writeToLog(part_string, false);
    logger.writeToLog("Initializing threads and Loading Queries and Data!", false);
    this->communicator->send_command(CMD_LOAD_DATA, params, SEND_TO_ALL);
    for(int i = 0 ; i < num_workers; i++){
        src = this->communicator->receive_command(cmd, params);
        if(cmd != CMD_READY)
            throwException("Wrong Command Recieved from "+toString(src));
        counter++;
        logger.writeToLog("Process "+toString(src)+" loaded "+params[params.size()-3]+" records in "+params[params.size()-4]+" sec. ("+toString(counter)+"/"+toString(num_workers)+")", false);
        this->clientsStatLabel->setText(QString(string("Clients: "+toString(src)+" loaded "+params[params.size()-3]+" triples in "+params[params.size()-4]+" sec. ("+toString(counter)+"/"+toString(num_workers)+")").c_str()));
        current_max_pred = atoi(params[params.size()-2].c_str());
        current_max_verts = atoi(params[params.size()-1].c_str());
        if(this->manager->max_predicate_id < current_max_pred)
            this->manager->max_predicate_id = current_max_pred;
        if(this->manager->max_vertex_id < current_max_verts)
            this->manager->max_vertex_id = current_max_verts;
        params.resize(params.size()-4);
        this->manager->add_predicates(params);
    }
    logger.writeToLog("Max Predicate ID: "+toString(this->manager->max_predicate_id)+", Max Vertex ID: "+toString(this->manager->max_vertex_id), false);
    this->profiler.pauseTimer("loading_data_and_queries");
    logger.writeToLog("Queries and Data loaded in "+toString(this->profiler.readPeriod("loading_data_and_queries"))+" sec.", false);
    this->profiler.clearTimer("loading_data_and_queries");


    logger.writeToLog(part_string, false);
    this->profiler.startTimer("computing_stats");
    struct stat fileAtt;
    string stats_path = this->db_folder+"data/"+this->file_name+".stat";
    if (stat(stats_path.c_str(), &fileAtt) != 0){
        logger.writeToLog("Computing Statistics", false);
        this->clientsStatLabel->setText("Clients: Computing Statistics");
        this->compute_statistics();
    }
    else{
        logger.writeToLog("Loading Statistics", false);
        this->clientsStatLabel->setText("Clients: Loading Statistics");
        this->load_statistics();
    }
    this->profiler.pauseTimer("computing_stats");
    logger.writeToLog("Computing Statistics Done in "+toString(this->profiler.readPeriod("computing_stats"))+" sec.", false);
    logger.writeToLog(part_string, false);
    logger.writeToLog("Clients are Ready!", false);
    this->manager->update_triple_load();
    logger.writeToLog(part_string, false);
    this->communicator->comm_volume = 0;
}

void MasterGUI::clientsReady(){
    this->clientsStatLabel->setText("Clients: Ready");
    QPixmap mypix(up_xpm);
    clientsStatImageLabel->setPixmap(mypix);
    this->are_clients_ready = true;
    if(is_master_ready && are_clients_ready){
        this->gnu_label_size.setHeight(this->ui->resultBox->height());
        this->gnu_label_size.setWidth(this->ui->resultBox->width());
        if(this->ui->qList->count() > 0)
            this->ui->execButton->setEnabled(true);
    }
}

void MasterGUI::compute_statistics(){
    vector<string> params;
    params.resize(1);
    pred_stat_t::iterator it;
    net_cmd cmd;
    int start, end;

    this->profiler.startTimer("exchange_verts");
    logger.writeToLog("Exchanging Vertices Statistics: ", false);
    this->clientsStatLabel->setText("Clients: Exchanging Vertices Statistics");
    this->communicator->send_command(CMD_EXCHANGE_VERTS_STATS, params, SEND_TO_ALL);
    for(int i = 1 ; i < this->communicator->get_num_workers(); i++){
        this->communicator->receive_command(cmd, params);
    }
    this->profiler.pauseTimer("exchange_verts");
    logger.writeToLog("Done in "+toString(this->profiler.readPeriod("exchange_verts"))+" sec.", false);
    this->profiler.clearTimer("exchange_verts");

    start = numeric_limits<int>::max();
    end = numeric_limits<int>::min();
    for(it = this->manager->predicates_stats.begin(); it != this->manager->predicates_stats.end(); it++){
        if(it->second.predicate_text <start)
            start = it->second.predicate_text;
        if(it->second.predicate_text > end)
            end = it->second.predicate_text;
    }
    for(int i = start ; i <= end; i++){
        logger.writeToLog("Computing Stats for: "+toString(i), false);
        this->clientsStatLabel->setText(QString("Clients: Computing Stats ("+QString::number(i)+"/"+QString::number(this->manager->predicates_stats.size())+")"));
        it = this->manager->predicates_stats.find(toString(i));
        params[0] = it->first;
        this->communicator->send_command(CMD_COMPUTE_PREDICATE_STATS, params, SEND_TO_ALL);
        for(int i = 1 ; i < this->communicator->get_num_workers(); i++){
            this->communicator->receive_command(cmd, params);
            it->second.subject_total_degree += atoi(params[0].c_str());
            it->second.subject_uniques = atoi(params[1].c_str());
            it->second.subject_score += atof(params[2].c_str());
            it->second.object_total_degree += atoi(params[3].c_str());
            it->second.object_uniques = atoi(params[4].c_str());
            it->second.object_score += atof(params[5].c_str());
            it->second.total_count += atoi(params[6].c_str());
        }
        if(it->second.total_count > 0){
            it->second.pred_per_subj = (float)it->second.total_count/it->second.subject_uniques;
            it->second.pred_per_obj = (float)it->second.total_count/it->second.object_uniques;
        }
        logger.writeToLog(it->second.print(true), false);
    }

    this->communicator->send_command(CMD_CLEAN_VERTS_DATA, params, SEND_TO_ALL);
    for(int i = 1 ; i < this->communicator->get_num_workers(); i++){
        this->communicator->receive_command(cmd, params);
    }
    this->manager->analyze_predicates();

    params.clear();

    for(pred_stat_t::iterator it = manager->predicates_stats.begin(); it != manager->predicates_stats.end(); it++){
        params.push_back(it->first+"*"+toString(it->second.pred_per_subj)+"*"+toString(it->second.pred_per_obj));
    }

    this->communicator->send_command(CMD_STORE_STATS, params, SEND_TO_ALL);

    for(int i = 1 ; i < this->communicator->get_num_workers(); i++){
        this->communicator->receive_command(cmd, params);
    }
    this->output_stats_to_file(start, end);
}

void MasterGUI::load_statistics(){
    vector<string> params;
    pred_stat_t::iterator it;
    net_cmd cmd;
    string stat_file = this->db_folder+"data/"+this->file_name+".stat";
    ifstream in(stat_file.c_str());
    string line;
    predicate_stat stat;

    if (in.is_open()){
        while(getline(in, line)){
            stat = predicate_stat(line);
            this->manager->predicates_stats[toString(stat.predicate_text)] = stat;
        }
        in.close();
    }


    for(pred_stat_t::iterator it = manager->predicates_stats.begin(); it != manager->predicates_stats.end(); it++){
        params.push_back(it->first+"*"+toString(it->second.pred_per_subj)+"*"+toString(it->second.pred_per_obj));
    }

    this->communicator->send_command(CMD_STORE_STATS, params, SEND_TO_ALL);

    for(int i = 1 ; i < this->communicator->get_num_workers(); i++){
        this->communicator->receive_command(cmd, params);
    }
}

void MasterGUI::output_stats_to_file(int start, int end){
    string stat_file = this->db_folder+"data/"+this->file_name+".stat";
    ofstream output(stat_file.c_str());
    for(int i = start; i <= end; i++){
        output<<manager->predicates_stats[toString(i)].print2()<<endl;
    }
    output.close();
}

void MasterGUI::browseQueryDir(){
    QString path;

    if(!this->query_folder.empty()){
        path = QString(this->query_folder.c_str());
    }
    else{
        path = QString(this->db_folder.c_str());
    }
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select queries directory"), path);

    if (!directory.isEmpty()) {
        this->ui->queryPath->setText(directory);
    }
}

void MasterGUI::updateQList(QString path){
    QStringList list;
    list.append("*.q");
    QDir dir = QDir(path);
    QFileInfoList file_list = dir.entryInfoList(list, QDir::Files, QDir::Name);
    this->ui->qList->clear();
    for(int i = 0 ; i < file_list.size(); i++){
        this->ui->qList->addItem(file_list.at(i).fileName());
    }
    this->ui->loaded_label->setText(QString::number(file_list.size()));
    this->query_folder = path.toStdString();
}

void MasterGUI::queryFileSelected(QString fileName){
    vector<Query> queries;
    load_queries(QString(QString::fromStdString(query_folder)+"/"+fileName).toStdString(), queries);
    if(queries.size() > 1){
        this->ui->displayResultsCB->setChecked(false);
        this->ui->displayResultsCB->setEnabled(false);
    }
    else{
        this->ui->displayResultsCB->setChecked(true);
        this->ui->displayResultsCB->setEnabled(true);
    }
    this->cumm_times.clear();
    this->cumm_times_na.clear();
    this->ui->num_queries_label->setText(QString(QString::number(queries.size())));
    QFile file(QString(QString::fromStdString(query_folder)+"/"+fileName));
    this->ui->fileContent->clear();

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        ui->fileContent->setPlainText(stream.readAll());
    }
    file.close();
}

void MasterGUI::executeQuery(){
    string path = QString(QString::fromStdString(query_folder)+"/"+this->ui->qList->currentText()).toStdString();
    vector<Query> queries;
    Exec_info info;

    ui->execButton->setEnabled(false);
    ui->gnuLabel->setVisible(false);
    ui->resultTable->setVisible(true);
    load_queries(path, queries);
    this->ui->resultTable->clear();
    if(queries.size() < 2){
        ui->execButton->setEnabled(false);
        this->ui->resultTable->setRowCount(0);
        this->ui->resultTable->setColumnCount(0);
        Master_Executer executer = Master_Executer(this, queries[0], this->ui->enableAdaptivity->isChecked(), &info, !this->ui->displayResultsCB->isChecked());
        executer.execute();
        this->update_exec_results(info, false);
        ui->execButton->setEnabled(true);
    }
    else{
        ui->enableAdaptivity->setEnabled(false);
        ui->gnuLabel->setVisible(true);
        ui->resultTable->setVisible(false);
        this->ui->resultBox->setTitle("");
        QtConcurrent::run(this, &MasterGUI::exec_load, queries);
    }
}

void MasterGUI::exec_load(vector<Query> &queries){
    Exec_info info, cummulative;
    boost::unordered_set<PLAN_OP> exec_types;

    for(unsigned i = 0 ; i < queries.size(); i++){
        info.reset();
        Master_Executer executer = Master_Executer(this, queries[i], this->ui->enableAdaptivity->isChecked(), &info, !this->ui->displayResultsCB->isChecked());
        executer.execute();
        cummulative.parallel_joins += info.parallel_joins;
        cummulative.distributed_joins += info.distributed_joins;
        cummulative.wall_time += info.wall_time;
        if(info.exec_type == PARALLEL || info.exec_type == STAR)
            exec_types.insert(PARALLEL);
        else
            exec_types.insert(SEMI_JOIN);
        emit updateProgress(info.wall_time);
    }
    if(exec_types.size() > 1){
        cummulative.exec_type = PHD_PARALLEL;
    }
    else{
        cummulative.exec_type = *exec_types.begin();
    }
    this->update_exec_results(cummulative, true);
    emit updateProgress(-1);
}

void MasterGUI::updateProgressFigure(double time){
    if(time == -1){
        ui->execButton->setEnabled(true);
        ui->enableAdaptivity->setEnabled(true);
        if(ui->enableAdaptivity->isChecked()){
            this->plot_figure_adaptive();
        }
        else{
            this->plot_figure_none_adaptive();
        }
        ui->gnuLabel->setPixmap(QPixmap(QString::fromStdString(this->adhash_home+"/images/progress.png")));
        return;
    }
    if(ui->enableAdaptivity->isChecked()){
        if(this->cumm_times.size() == 0)
            this->cumm_times.push_back(make_pair(this->cumm_times.size()+1, time));
        else
            this->cumm_times.push_back(make_pair(this->cumm_times.size()+1, this->cumm_times[this->cumm_times.size()-1].second+time));
        if((this->cumm_times.size() % 10) == 0)
            this->plot_figure_adaptive();
    }
    else{
        if(this->cumm_times_na.size() == 0)
            this->cumm_times_na.push_back(make_pair(this->cumm_times_na.size()+1, time));
        else
            this->cumm_times_na.push_back(make_pair(this->cumm_times_na.size()+1, this->cumm_times_na[this->cumm_times_na.size()-1].second+time));
        if((this->cumm_times_na.size() % 10) == 0)
            this->plot_figure_none_adaptive();
    }
}

void MasterGUI::plot_figure_adaptive(){
    Gnuplot gp;
    QString path = QString::fromStdString(this->adhash_home+"/images/progress.png");
    gp<<"set terminal png size "<<(this->gnu_label_size.width())<<","<<(this->gnu_label_size.height())<<" enhanced font 'Verdana,10'\n";
    gp<<"set output \""+path.toStdString()+"\"\n";
    gp<<"set pointsize 7\n";
    gp<<"set bar 7\n";
    gp<<"set key  left top vertical samplen 1 enhanced reverse Left\n";
    gp<<"set style line 12 lc rgb '#808080' lt 0 lw 1\n";
    gp<<"set grid back ls 12\n";
    gp<<"set border 3 linewidth 0.25\n";
    gp<<"set pointsize 2.5\n";
    gp<<"set xlabel \"Query\"\n";
    gp<<"set ylabel \"Cumulative time (sec)\"\n";
    gp<<"set tics nomirror\n";

    if(this->cumm_times_na.size() > 0){
        gp << "plot "<<gp.file1d(this->cumm_times_na)<<" title \"AdHash-NA\" with lines ls 6 lc rgb 'red', "
           << gp.file1d(this->cumm_times)<<" title \"AdHash\" with lines ls 1 lc rgb 'blue'\n";
    }
    else{
        gp << "plot "<<gp.file1d(this->cumm_times)<<" title \"AdHash\" with lines ls 1 lc rgb 'blue' \n";
    }
    ui->gnuLabel->setPixmap(QPixmap(path));
}

void MasterGUI::plot_figure_none_adaptive(){
    QString path = QString::fromStdString(this->adhash_home+"/images/progress.png");
    Gnuplot gp;

    gp<<"set terminal png size "<<(this->gnu_label_size.width())<<","<<(this->gnu_label_size.height())<<" enhanced font 'Verdana,10'\n";
    gp<<"set output \""+path.toStdString()+"\"\n";
    gp<<"set pointsize 7\n";
    gp<<"set bar 7\n";
    gp<<"set key  left top vertical samplen 1 enhanced reverse Left\n";
    gp<<"set style line 12 lc rgb '#808080' lt 0 lw 1\n";
    gp<<"set grid back ls 12\n";
    gp<<"set border 3 linewidth 0.25\n";
    gp<<"set pointsize 2.5\n";
    gp<<"set xlabel \"Query\"\n";
    gp<<"set ylabel \"Cumulative time (sec)\"\n";
    gp<<"set tics nomirror\n";

    if(this->cumm_times.size() > 0){
        gp << "plot "<<gp.file1d(this->cumm_times_na)<<" title \"AdHash-NA\" with lines ls 6 lc rgb 'red', "
           << gp.file1d(this->cumm_times)<<" title \"AdHash\" with lines ls 1 lc rgb 'blue'\n";
    }
    else{
        gp << "plot "<<gp.file1d(this->cumm_times_na)<<" title \"AdHash\" with lines ls 1 lc rgb 'blue' \n";
    }
    ui->gnuLabel->setPixmap(QPixmap(path));
}

void MasterGUI::update_exec_results(Exec_info &info, bool load){
    switch (info.exec_type) {
    case SEMI_JOIN:
        this->ui->exec_type->setText("Distributed");
        break;
    case PARALLEL:
        this->ui->exec_type->setText("Parallel");
        break;
    case STAR:
        this->ui->exec_type->setText("Parallel (star)");
        break;
    default:
        this->ui->exec_type->setText("Mix");
        break;
    }
    this->ui->wall_time_label->setText(QString::number(info.wall_time, 'f', 4 )+QString("  (sec)"));
    this->ui->num_distrib_label->setText(QString::number(info.distributed_joins));
    this->ui->num_parallel_label->setText(QString::number(info.parallel_joins));
    if(!load)
        this->ui->resultBox->setTitle(QString("Results: ")+QString::number(info.cardinality));
}

void MasterGUI::prepare_results_table(vector<string> &projection, int rows){
    this->currentRow = 0;
    this->ui->resultTable->setRowCount(rows);
    this->ui->resultTable->setColumnCount(projection.size());
    QStringList headerStrings;
    for(unsigned i = 0; i < projection.size(); i++){
        headerStrings.append(QString::fromStdString(projection[i]));
    }
    this->ui->resultTable->setHorizontalHeaderLabels(headerStrings);
    //    QHeaderView* header = this->ui->resultTable->horizontalHeader();
    //    //header->setSectionResizeMode(QHeaderView::Stretch);
}

void MasterGUI::update_results(vector<int> &pos, vector<int> &results, int row_size){
    int col = 0;

    for(unsigned i = 0 ; i < results.size() ; i+=row_size){
        for(unsigned j = 0 ; j < pos.size() ; j++){
            if(this->verts_data[results[i+pos[j]]].literal)
                this->ui->resultTable->setItem(currentRow, col++, new QTableWidgetItem(QString::fromStdString(this->verts_data[results[i+pos[j]]].text)));
            else
                this->ui->resultTable->setItem(currentRow, col++, new QTableWidgetItem(QString::fromStdString("<"+this->verts_data[results[i+pos[j]]].text+">")));
        }
        col = 0;
        currentRow++;
    }
    //this->ui->resultTable->resizeColumnsToContents();
}

void MasterGUI::load_queries(string queryFile, vector<Query>& queries) {
    ifstream queryIn(queryFile.c_str());
    string querystring, str;
    int qid = 1;

    if (!queryIn) {
        throwException("Query file '" + queryFile
                       + "' was not found. Please try again!\n");
    }

    // read the query string
    querystring = "";

    while (true) {
        queryIn >> str;
        if ((str == query_delim || queryIn.eof()) && !querystring.empty()) {
            SPARQLLexer lexer(querystring);
            SPARQLParser parser(lexer);
            try {
                parser.parse();
            } catch (const SPARQLParser::ParserException& e) {
                cerr << "parse error: " << e.message << endl;
                querystring = "";
                if (queryIn.eof())
                    break;

                continue;
            }
            Query query(parser);
            if(query.supported){
                query.qid = qid++;
                queries.push_back(query);
            }
            else{
                QMessageBox::critical(NULL, QObject::tr("Not Supported"),
                                      QObject::tr("AdHash currently supports BGP queries only!"),
                                      QMessageBox::Ok, QMessageBox::Ok);
            }
            querystring = "";
            if (queryIn.eof())
                break;

            continue;
        }
        querystring += str + " ";
    }
    queryIn.close();

}
