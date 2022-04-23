//
// Created by Ashwi on 4/12/2022.
//
#include <iostream>
#include<bits/stdc++.h> // for Graph class stuff
#include<pthread.h>     // for Thread stuff
#include<unistd.h>
#include "dw_graph.h"

/*
 * Ashwin Rajesh, Operating Systems, Program 3
 * Due 22/04/2022
 * Graph class source: https://github.com/erenkeskin/directed-weighted-graph/blob/master/src/main.cpp
*/


/// player struct
struct player {
    std::string name;
    int numNumbers;
    std::vector<int> numbers;
    int thread_id;
};
/// end of player struct

/// Global vars
std::vector<player> players;        // vector will store metadata on each player
DirectedWeightedGraph graphie;
int graphSize = 0;      // we set graph size once we parse graph specs
std::vector<std::vector<int>> thread_resources;
std::vector<int> resourcesInUse;
std::vector<std::pair<int, int>> claimTrack;
std::vector<std::vector<int>> inputGraph;

int N_in = 0;
int k_in = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;
/// end of Global var declarations


/// Graph Printing Functions
void printClaims(){
    std::cout << "Threads to Resources:  ";
    std::set<pair<std::string, int>> print_list;
    for (int i = k_in+1; i < graphie.get_nodeCount(); i++){
        for (auto p : graphie.adjacencyList[i]){
            print_list.insert(make_pair(players[i-k_in-1].name, p.first));
        }
    }
    for(auto itr = print_list.begin(); itr != print_list.end(); itr++){
        std::cout << "(" << itr->first << ", " << itr->second << ") ";
    }
    std::cout << std::endl;
}

void printAssignments(){
    std::cout << "Resources to Threads:  ";
    for (int i = 0; i < graphie.get_nodeCount()-N_in; i++){
        for (auto p : graphie.adjacencyList[i]){
            std::cout << "(" << i << ", " << players[p.first-k_in-1].name << ") ";
        }
    }
    std::cout << std::endl;
}
/// End of Printing Functions

/// Function sending broadcast to start threads
void startSignal(){
    pthread_cond_broadcast(&cond);
    printf("\nStart signal sent, delete me\n");
}
/// End of start signal

/// Request Function
bool Request(int playerID, int resourceID){     // takes in person making request, and resource to be obtained, thnen will facilitate the request algorithm
    bool requestStatus;
    int playerNode = graphSize+playerID-N_in;
    DirectedWeightedGraph tempie;
    tempie = graphie;       // tempie will replicate our graph to discover cycles before implemented on graphie

    std::cout << "Person " << players[playerID].name << " requests resource " << players[playerID].numbers[resourceID] << std::endl;
    printAssignments();
    printClaims();

    bool is_assigned = false;
    for(int i = 0; i < resourcesInUse.size(); i++){                         // Check whether resource is already in use by another process
        if(resourcesInUse[i] == players[playerID].numbers[resourceID]){
            is_assigned = true;
        }
    }
    if(is_assigned == true){        // if resource is being used by another process, deny the request
        std::cout << "Person " << players[playerID].name << " requests resource " << players[playerID].numbers[resourceID] << ": denied" << std::endl;
        requestStatus = false;
    }
    else {
        // Below we change the edge from a claim edge to an assignment edge
        while(tempie.is_edge(playerNode, players[playerID].numbers[resourceID])){tempie.remove_edge(playerNode, players[playerID].numbers[resourceID]);}
        tempie.add_edge(players[playerID].numbers[resourceID], playerNode, 0);

        // We then check whether this change induces a cycle
        if (!tempie.is_acyclic()) {
            resourcesInUse.push_back(players[playerID].numbers[resourceID]);
            while(graphie.is_edge(playerNode, players[playerID].numbers[resourceID])){graphie.remove_edge(playerNode, players[playerID].numbers[resourceID]);}
            graphie.add_edge(players[playerID].numbers[resourceID], playerNode, 0);
            std::cout << "Person " << players[playerID].name << " requests resource " << players[playerID].numbers[resourceID] << ": accepted" << std::endl;
            printAssignments();
            printClaims();
            requestStatus = true;
        }
        else {
            std::cout << "Person " << players[playerID].name << " requests resource " << players[playerID].numbers[resourceID] << ": denied" << std::endl;
            requestStatus = false;
        }
    }
    return requestStatus;
}
/// End of Request Function

/// Release Function
void Release(int playerID, int resourceID, int option){     // takes in person releasing, and the list of resources they are releasing, and the option (first option is for individual release, second is for group release)
    if(option == 0){
        int playerNode = graphSize+playerID-N_in;
        for(int l = 0; l < resourcesInUse.size(); l++){
            if(resourcesInUse[l] == -players[playerID].numbers[resourceID]){
                resourcesInUse.erase(resourcesInUse.begin()+l);
                bool removeEdge = true;
                for(int p = resourceID+1; p < players[playerID].numbers.size(); p++){
                    if(players[playerID].numbers[p] == -players[playerID].numbers[resourceID]){
                        removeEdge = false;
                    }
                }
                if(removeEdge == true){
                    while(graphie.is_edge(playerNode, -players[playerID].numbers[resourceID])){graphie.remove_edge(playerNode, -players[playerID].numbers[resourceID]);}
                    while(graphie.is_edge(-players[playerID].numbers[resourceID], playerNode)){graphie.remove_edge(-players[playerID].numbers[resourceID], playerNode);}
                }
                else{
                    while(graphie.is_edge(playerNode, -players[playerID].numbers[resourceID])){graphie.remove_edge(playerNode, -players[playerID].numbers[resourceID]);}
                    while(graphie.is_edge(-players[playerID].numbers[resourceID], playerNode)){graphie.remove_edge(-players[playerID].numbers[resourceID], playerNode);}
                    graphie.add_edge(playerNode, -players[playerID].numbers[resourceID], 0);
                }
                std::cout << "Person " << players[playerID].name << " releases resource " << -players[playerID].numbers[resourceID] << std::endl;
                printAssignments();
                printClaims();
            }
        }
    }
    else if (option == 1){
        int playerNode = graphSize+playerID-N_in;
        for(int l = 0; l < resourcesInUse.size(); l++){
            if(resourcesInUse[l] == players[playerID].numbers[resourceID]){
                resourcesInUse.erase(resourcesInUse.begin()+l);
                while(graphie.is_edge(playerNode, players[playerID].numbers[resourceID])){graphie.remove_edge(playerNode, players[playerID].numbers[resourceID]);}
                while(graphie.is_edge(players[playerID].numbers[resourceID], playerNode)){graphie.remove_edge(players[playerID].numbers[resourceID], playerNode);}
                std::cout << "Person " << players[playerID].name << " releases resource " << players[playerID].numbers[resourceID] << std::endl;
            }
        }
    }
}
/// End of Release Function

/// Thread function
void *playerThread(void* z){
    const int threadID = *((int*) z);
    int totalResources = players[threadID].numNumbers;
    int itemVectorPosition = 0;
    std::vector<int> threadInUse;

    pthread_mutex_lock(&locker);          // lock thread until start signal
    //std::cout << "Thread " << threadID << " waiting for broadcast, delete me " << players[threadID].name << " " << players[threadID].numNumbers << std::endl;
    pthread_cond_wait(&cond, &locker);    // wait for start signal
    //std::cout << "Thread " << threadID << " recieved start, delete me" << std::endl;
    pthread_mutex_unlock(&locker);        // unlock thread after signal broadcast

    while(totalResources > 0){
        // below will compete with other threads to lock in their processes
        int lockStatus = pthread_mutex_lock(&locker);
        while(lockStatus != 0){
            lockStatus = pthread_mutex_lock(&locker);
        }

        // Determine whether next item is requesting or releasing a resource
        if(players[threadID].numbers[itemVectorPosition] > 0){  // REQUEST
            bool granted = Request(threadID, itemVectorPosition);
            if(granted == true){
                threadInUse.push_back(itemVectorPosition);
                itemVectorPosition++;
                totalResources -= 1;
                sleep(1+((std::rand()%100)/100));
            }
            else{
                sleep((std::rand()%100)/100);
            }
            pthread_mutex_unlock(&locker);
        }
        else{       // RELEASE
            Release(threadID, itemVectorPosition, 0);
            threadInUse.erase(std::remove(threadInUse.begin(), threadInUse.end(), -players[threadID].numbers[itemVectorPosition]), threadInUse.end());
            itemVectorPosition++;
            totalResources -= 1;
            pthread_mutex_unlock(&locker);  // unlock and allow other threads to capture
        }
    }

    sleep(1+((std::rand()%100)/100));           // sleep for (1+q/100) seconds (q is a random number between 0, 99)

    int numIterations = threadInUse.size();
    for(int m = 0; m < numIterations; m++){       // release all the resources that it is still holding
        int min = INT_MAX;
        int min_pos = 0;
        for(int n = 0; n < threadInUse.size(); n++){    // used to arrange descending order prescribed in document
            if(players[threadID].numbers[threadInUse[n]] < min){
                min = players[threadID].numbers[threadInUse[n]];
                min_pos = n;
            }
        }
        Release(threadID, threadInUse[min_pos], 1);
        threadInUse.erase(threadInUse.begin()+min_pos);
    }
    // When releasing all resources, go ahead and print the graphs
    printAssignments();
    printClaims();
    // exit
    return NULL;
}
/// End of thread

int main(int argc, char **argv) {
    /// File IO
    std::string N_string;
    std::string k_string;
    std::cout << "Name of Input File: " << argv[1] << std::endl << std::endl;     // "when program starts, you should print out name of file"
    std::ifstream input_file;
    input_file.open(argv[1]);

    std::string first_line;
    getline(input_file, first_line);
    std::istringstream  first_ss(first_line);
    getline(first_ss, N_string, ' ');
    N_in = std::stoi(N_string);
    getline(first_ss, k_string, '\n');
    k_in = std::stoi(k_string);

    graphSize = N_in + k_in + 1;
    graphie.set_nodeCount(graphSize);

    //std::cout << "N read: " << N_in << " k read: " << k_in << std::endl << std::endl;
    std::string curr_line;
    for(int i = 0; i < N_in; i++){      // iterate through all players
        player temp_player;
        vector<int> currInputGraph;
        getline(input_file, curr_line);
        std::istringstream ss(curr_line);
        std::string token;
        getline(ss, token, ' ');
        temp_player.name = token;
        getline(ss, token, ' ');
        temp_player.numNumbers = stoi(token);
        while(getline(ss, token, ' ')){
            temp_player.numbers.push_back(stoi(token));
            /// Adding Claim edges
            if(!std::count(claimTrack.begin(), claimTrack.end(), make_pair(graphSize+i-N_in, std::abs(stoi(token))))){
                //std::cout << std::endl << "Adding " << std::abs(stoi(token)) << " to P" << i << std::endl;
                currInputGraph.push_back(std::abs(stoi(token)));
                graphie.add_edge(graphSize+i-N_in, std::abs(stoi(token)), 0);
                claimTrack.push_back(make_pair(graphSize+i-N_in, std::abs(stoi(token))));
            }
            /// Done claiming
        }
        inputGraph.push_back(currInputGraph);
        players.push_back(temp_player);
        std::vector<int> newThread;
        thread_resources.push_back(newThread);
    }
    //to test graph library: printGraph(graphie);

    // Test file parse below:
//    for(int i = 0; i < players.size(); i++){
//        std::cout << players[i].name << " " << players[i].numNumbers << " ";
//        for(int j = 0; j < players[i].numbers.size(); j ++){
//            std::cout << players[i].numbers[j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
    /// End of File IO

    /// Create a thread for each person
    pthread_t tid[N_in];
    for(int j = 0; j < N_in; j++){
        players[j].thread_id = j;       // assign player thread id (even though its same as player id)
        pthread_create(&tid[j], NULL, playerThread, (void *) &players[j].thread_id);    // create thread
    }
    sleep(1);   // wait to give time for threads to finish necessary pre-processing
    /// End of thread creation

    /// Print initial Graph
    printAssignments();
    printClaims();
    sleep(1);
    /// End of Print

    /// Signal start
    startSignal();
    /// End of start signal


    /// Thread cleanup
    pthread_exit(NULL);
    /// end of thread exit

    std::cout << std::endl;
    return 0;
}
