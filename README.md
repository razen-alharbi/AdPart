#Accelerating SPARQL Queries by Exploiting Hash-based Locality and Adaptive Partitioning
## Introduction
State-of-the-art distributed RDF systems partition data across multiple computer nodes (workers). Some systems perform cheap hash partitioning, which may result in expensive query evaluation, while others apply heuristics aiming at minimizing inter-node communication during query evaluation. This requires an expensive data pre-processing phase, leading to high startup costs for very large RDF knowledge bases. Apriori knowledge of the query workload has also been used to create partitions, which however are static and do not adapt to workload changes; as a result, inter-node communication cannot be consistently avoided for queries that are not favored by the initial data partitioning.

We propose AdPart, a distributed RDF system, which addresses the shortcomings of previous work. First, AdPart applies lightweight partitioning on the initial data, that distributes triples by hashing on their subjects; this renders its startup overhead low. At the same time, the locality-aware query optimizer of AdPart takes full advantage of the partitioning to (i) support the fully parallel processing of join patterns on subjects and (ii) minimize data communication for general queries by applying hash distribution of intermediate results instead of broadcasting, wherever possible. Second, AdPart monitors the data access patterns and dynamically redistributes and replicates the instances of the most frequent ones among workers. As a result, the communication cost for future queries is drastically reduced or even eliminated. To control replication, AdPart implements an eviction policy for the redistributed patterns. Our experiments with synthetic and real data verify that AdPart (i) starts faster than all existing systems, (ii) processes thousands of queries before other systems become online, and (iii) gracefully adapts to the query load, being able to evaluate queries on billion-scale RDF data in sub-seconds. 

For more details, visit http://cloud.kaust.edu.sa/Pages/AdPart.aspx 

## License
AdPart is released under the [Creative Commons](http://creativecommons.org/licenses/by-nc-sa/3.0/) Attribution-Noncommercial-Share Alike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 
If you use AdPart in your research, please cite our paper:
 ```
  @article{DBLP:journals/pvldb/HarbiAKM15,
  author    = {Razen Harbi and
               Ibrahim Abdelaziz and
               Panos Kalnis and
               Nikos Mamoulis},
  title     = {Evaluating {SPARQL} Queries on Massive {RDF} Datasets},
  journal   = {{PVLDB}},
  volume    = {8},
  number    = {12},
  pages     = {1848--1859},
  year      = {2015},
  url       = {http://www.vldb.org/pvldb/vol8/p1848-harbi.pdf},
  timestamp = {Fri, 14 Aug 2015 15:24:11 +0200},
  biburl    = {http://dblp.uni-trier.de/rec/bib/journals/pvldb/HarbiAKM15},
  bibsource = {dblp computer science bibliography, http://dblp.org}
}
 ```
 
## Building
AdPart was tested on 64-bit Ubuntu Linux 12.04 (precise), 14.04 (trusty) and LinuxMint 17.01 (Rebecca). 
 
## Dependencies
 There are a few dependencies which must be satisfied in order to compile and run AdPart.
 
 * build-essential and g++ (>= 4.4.7) [Required]
   +  Needed for compiling AdPart.
 
 * openssh-server [Required]
    + Required to initialize MPI and establish connections among compute nodes.
 
 * MPICH2 [Required]
    + AdPart uses MPI for inter-node communication. Open MPI is not tested with AdPart.
 
 * Install a recent version of Boost (>= 1.46) [Required]
    + AdPart requires all headers of non-complied boost libraries (libboost-dev) and other compiled boost libraries. Specifically, AdPart uses boost's iostreams, system and filesystem libraries.
 
* Qt (>= 5.4) [Required]
    + AdPart has a simple Qt-based GUI for managing, partitioning and querying RDF datasets. AdPart links dynamically to Qt.
    
* lGL library [Required]
    + Needed by Qt for GUI rendering.
    
* gnuplot (>= 4.4) [Required]
    + Needed to plot some images within AdPart GUI.
    
## Satisfying Dependencies on Ubuntu and LinuxMint

All the dependencies can be satisfied from the Ubuntu/LinuxMint repositories:

     sudo apt-get update
     sudo apt-get install gcc g++ build-essential libopenssh-server libmpich2-dev libboost-dev libboost-iostreams-dev libboost-system-dev libboost-filesystem-dev libglu1-mesa-dev gnuplot git
     
Download Qt from http://www.qt.io/download/. Both online and offline installations were tested with AdPart.

Note: for MPI to work, all workers need to be able to ssh to each other without passwords.
 
## Downloading AdPart
 
 You can download AdPart from Github, which also offers a zip download of the repository through the website.
 
 To clone from Github using git, execute the following command:
 
     git clone https://github.com/razen-alharbi/AdPart.git
 
 
## Compiling and Running
 * Change your current working directory to the downloaded directory. This directory is referred to as ADHASH_HOME hereafter.
 ```
 cd ADHASH_HOME
 ```
* Generate the make files by running qmake.
  ```
qmake AdHashGUI.pro -r -spec linux-g++
 ```
* Compile AdPart. You can run multiple parallel build tasks. We used 4 parallel tasks.
  ```
make -j4
 ```
* Run the management console of AdPart by executing the following command. AdPart has a simple and self-explanatory GUI that is easy to follow for loading, partitioning and querying RDF datasets.
```
./Release/mgmt
 ```
## Running Issues
 If your system's locale is not set properly, AdPart may fail to run. This was specifically noticed when installing the desktop-environment on ubuntu servers. To go around this, execute the following command:
 ```
export LC_ALL="en_US.UTF-8"
 ```
 
 If you encounter issues please send an email to razen.harbi@kaust.edu.sa and ibrahim.abdelaziz@kaust.edu.sa
 
## Tested datasets
The following datasets/benchmarks were tested successfully by AdPart:
* LUBM Benchmark (http://swat.cse.lehigh.edu/projects/lubm/).
* WatDiv Benchmark (http://dsg.uwaterloo.ca/watdiv/).
* YAGO2 (http://yago-knowledge.org/).
* Bio2RDF (http://www.bio2rdf.org/).

