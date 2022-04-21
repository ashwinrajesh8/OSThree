//
// Created by Ashwi on 4/12/2022.
//
#include <iostream>
#include<bits/stdc++.h> // for Graph class stuff
#include<pthread.h>     // for Thread stuff
#include<unistd.h>
#include "dw_graph.h"

/*
 * Ashwin Rajesh, Operating Systems, Program 1
*/


/*
 * Problems:
 * Directed graph may have no cycles, since we just add edges
 * We don't allocate resources to players, only players to resources
 * No join thread
 * No claim edges, we just test on new graph
 *
 * Resources and player node?
 * Resource already being used - no cycle so we rejec?
 * */

/// Imported Graph Class starts here - source: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
class Graph
{
    int V;    // No. of vertices
    std::list<int> *adj;    // Pointer to an array containing adjacency lists
    bool isCyclicUtil(int v, bool visited[], bool *rs);  // used by isCyclic()
public:
    Graph(int V);   // Constructor
    void addEdge(int v, int w);   // to add an edge to graph
    void removeEdge(int v, int w);
    void removeClaims(int v, int w);
    bool isCyclic();    // returns true if there is a cycle in this graph
    Graph(); // ADDED new constructor
    void addAttributes(int V);
};

/// add declaration to enable global variable
Graph::Graph(){

}
/// end new initialization
/// Enable object update
void Graph::addAttributes(int V){
    this->V = V;
    adj = new std::list<int>[V];
}
/// end object update

Graph::Graph(int V)
{
    this->V = V;
    adj = new std::list<int>[V];
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}

///
void Graph::removeEdge(int v, int w) {
//    auto index = adj[v].insert(adj[v].end(), w);
//    adj[v].erase(index);
    adj[v].remove(w);
}
void Graph::removeClaims(int v, int w){
    for(int i = 0; i < adj->size(); i++){
        for(int j = 0; j < adj[i].size(); j++){
            auto it = std::next(adj[i].begin(), j);
            if(*it == w){
                adj[i].remove(j);
            }
        }
    }
}
///

// This function is a variation of DFSUtil() in https://www.geeksforgeeks.org/archives/18212
bool Graph::isCyclicUtil(int v, bool visited[], bool *recStack)
{
    if(visited[v] == false)
    {
        // Mark the current node as visited and part of recursion stack
        visited[v] = true;
        recStack[v] = true;

        // Recur for all the vertices adjacent to this vertex
        std::list<int>::iterator i;
        for(i = adj[v].begin(); i != adj[v].end(); ++i)
        {
            if ( !visited[*i] && isCyclicUtil(*i, visited, recStack) )
                return true;
            else if (recStack[*i])
                return true;
        }

    }
    recStack[v] = false;  // remove the vertex from recursion stack
    return false;
}

// Returns true if the graph contains a cycle, else false.
// This function is a variation of DFS() in https://www.geeksforgeeks.org/archives/18212
bool Graph::isCyclic()
{
    // Mark all the vertices as not visited and not part of recursion
    // stack
    bool *visited = new bool[V];
    bool *recStack = new bool[V];
    for(int i = 0; i < V; i++)
    {
        visited[i] = false;
        recStack[i] = false;
    }

    // Call the recursive helper function to detect cycle in different
    // DFS trees
    for(int i = 0; i < V; i++)
        if ( !visited[i] && isCyclicUtil(i, visited, recStack))
            return true;

    return false;
}

/// End of imported Graphing Class

/// player struct
struct player {
    std::string name;
    int numNumbers;
    std::vector<int> numbers;
    int thread_id;
};
/// end of player struct

/// Global vars
std::vector<player> players;
Graph graphy;
int graphSize = 0; // assuming individual k does not exceed 100
std::vector<std::vector<int>> thread_resources;
std::vector<int> resourcesInUse;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;
/// end of Global var declarations

/// Function sending broadcast to start threads
void startSignal(){
    pthread_cond_broadcast(&cond);
    printf("\nStart signal sent, delete me\n");
}
/// End of start signal

/// Request Function
bool Request(int playerID, int resourceID){     // takes in person making request, and resource to be obtained
    bool requestStatus = true;
    int playerNode = graphSize+playerID-1;
    Graph tempy = graphy;

    bool is_assigned = false;
    for(int i = 0; i < resourcesInUse.size(); i++){
        if(resourcesInUse[i] == players[playerID].numbers[resourceID]){
            is_assigned = true;
        }
    }
    if(is_assigned == true){
        Graph tempy_two = graphy;
        ////tempy_two.addEdge(playerNode, players[playerID].numbers[resourceID]);
//            if(!tempy_two.isCyclic()){
//                std::cout << "Player " << players[playerID].name << " requested resource " << players[playerID].numbers[resourceID] << ": assigned" << std::endl;
//                //graphy.addEdge(playerNode, players[playerID].numbers[resourceID]);
//            }
//            else{
//                std::cout << "Player " << players[playerID].name << " requested resource " << players[playerID].numbers[resourceID] << ": rejected" << std::endl;
//            }
        //// comment out bellow
        std::cout << "Player " << players[playerID].name << " requested resource " << players[playerID].numbers[resourceID] << ": rejected" << std::endl;
        requestStatus = false;
    }
    else {
        tempy.removeEdge(playerNode, players[playerID].numbers[resourceID]);    ////
        tempy.addEdge(players[playerID].numbers[resourceID], playerNode); ////
        //tempy.addEdge(playerNode, players[playerID].numbers[resourceID]);   ////
        //tempy.removeClaims(playerNode, players[playerID].numbers[resourceID]); ////
////    tempy.addEdge(players[playerID].numbers[resourceID], playerNode);
        if (!tempy.isCyclic()) {
            /// try check thre resource vector check if assigned

            //else{
            resourcesInUse.push_back(players[playerID].numbers[resourceID]);
            graphy.removeEdge(playerNode, players[playerID].numbers[resourceID]);    ////
            graphy.addEdge(players[playerID].numbers[resourceID], playerNode);
            //graphy.removeClaims(playerNode, players[playerID].numbers[resourceID]); ////
            std::cout << "Player " << players[playerID].name << " requested resource "
                      << players[playerID].numbers[resourceID] << ": accepted" << std::endl;
            requestStatus = true;
            //}
            ///graphy.addEdge(playerNode, players[playerID].numbers[resourceID]);
//        graphy.addEdge(players[playerID].numbers[resourceID], playerNode);
///        std::cout << "Player " << players[playerID].name << " requested resource " << players[playerID].numbers[resourceID] << ": accepted" << std::endl;
///        requestStatus = true;
        } else {
            std::cout << "Player " << players[playerID].name << " requested resource "
                      << players[playerID].numbers[resourceID] << ": denied" << std::endl;
            requestStatus = false;
        }
    }

    return requestStatus;
}
/// End of Request Function

/// Release Function
void Release(int playerID, int resourceID){     // takes in person releasing, and the list of resources they are releasing
    int playerNode = graphSize+playerID-1;
    for(int l = 0; l < resourcesInUse.size(); l++){
        if(resourcesInUse[l] == -players[playerID].numbers[resourceID]){
            resourcesInUse.erase(resourcesInUse.begin()+l);
            std::cout << "Player " << players[playerID].name << " released " << -players[playerID].numbers[resourceID] << std::endl;
            graphy.removeEdge(playerNode, -players[playerID].numbers[resourceID]); ////
            graphy.removeEdge(-players[playerID].numbers[resourceID], playerNode);
        }
    }
}
/// End of Release Function

/// Thread function
void *playerThread(void* z){
    const int threadID = *((int*) z);
    //std::vector<int> thread_resources;
    int totalResources = players[threadID].numNumbers;
    int itemVectorPosition = 0;

    pthread_mutex_lock(&locker);          // lock thread until start signal
    std::cout << "Thread " << threadID << " waiting for broadcast, delete me " << players[threadID].name << " " << players[threadID].numNumbers << std::endl;
    pthread_cond_wait(&cond, &locker);    // wait for start signal
    std::cout << "Thread " << threadID << " recieved start, delete me" << std::endl;
    pthread_mutex_unlock(&locker);        // unlock thread after signal broadcast

    while(totalResources > 0){
        int lockStatus = pthread_mutex_lock(&locker);
        while(lockStatus != 0){
            lockStatus = pthread_mutex_lock(&locker);
        }

        // Determine whether next item is requesting or releasing a resource
        if(players[threadID].numbers[itemVectorPosition] > 0){  // REQUEST
            bool granted = Request(threadID, itemVectorPosition);
//            pthread_mutex_unlock(&locker);
            if(granted == true){
                itemVectorPosition++;
                totalResources -= 1;
                sleep(1+((std::rand()%100)/100));
            }
            else{
                // skip
//                itemVectorPosition++; ////
//                totalResources -= 1; ////
                // end skip
                sleep((std::rand()%100)/100);
            }
            pthread_mutex_unlock(&locker);
        }
        else{       // RELEASE
            Release(threadID, itemVectorPosition);
            itemVectorPosition++;
            totalResources -= 1;
            pthread_mutex_unlock(&locker);
        }

//        totalResources -= 1;
        std::cout << "Thread " << threadID << " total: " << totalResources << std::endl;

//        pthread_mutex_unlock(&locker);
//        sleep(1);   /// EVALUATE NEED FOR SLEEP!!!! if i remove, then same player for each turn until done.
    }


    sleep(1+((std::rand()%100)/100));           // sleep for (1+q/100) seconds (q is a random number between 0, 99)

    for(int k = 0; k < thread_resources[threadID].size(); k++){       // release all the resources that it is still holding
        Release(threadID, thread_resources[threadID][k]);
    }
    std::cout << "Thread " << threadID << " released everything, delete me"<< std::endl;
    // exit
    return NULL;
}
/// End of thread

int main(int argc, char **argv) {
    std::cout << "\nTest cmake project execution." << std::endl << std::endl;

    /// File IO
    int N_in = 0;
    std::string N_string;
    int k_in = 0;
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

    graphSize = 100;
    graphy.addAttributes(graphSize+N_in);

    std::cout << "N read: " << N_in << " k read: " << k_in << std::endl << std::endl;
    std::string curr_line;
    for(int i = 0; i < N_in; i++){
        player temp_player;
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
            graphy.addEdge(graphSize+i-1, std::abs(stoi(token)));
            if(graphy.isCyclic()){std::cout << "Graphy contains cycle\n\n";}
            /// Done claiming
        }
        players.push_back(temp_player);
        std::vector<int> newThread;
        thread_resources.push_back(newThread);
    }

    for(int i = 0; i < players.size(); i++){
        std::cout << players[i].name << " " << players[i].numNumbers << " ";
        for(int j = 0; j < players[i].numbers.size(); j ++){
            std::cout << players[i].numbers[j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

//    // Initialize graph to N vertices
//    graphy.addAttributes(N_in);
    /// End of File IO

    /// Create a thread for each person
    pthread_t tid[N_in];
    for(int j = 0; j < N_in; j++){
        players[j].thread_id = j;       // assign player thread id (even though its same as player id)
        pthread_create(&tid[j], NULL, playerThread, (void *) &players[j].thread_id);    // create thread
    }
    sleep(2);   // wait to give time for threads to finish necessary pre-processing
    /// End of thread creation

    /// Signal start
    startSignal();
    /// End of start signal

        ///
    // Generating Graph with size 5
    DirectedWeightedGraph di_graph(5);

    // Adding New Edges to Graph
    di_graph.add_edge(0, 1, 2);
    di_graph.add_edge(0, 2, 3);
    di_graph.add_edge(1, 2, 4);
    di_graph.add_edge(2, 3, 5);
    di_graph.add_edge(3, 3, 6);
    if(di_graph.is_acyclic() == true){std::cout<<"this shi cyclic"<<std::endl;}

    // Printing Graph
    printGraph(di_graph);
        ///

    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    //g.addEdge(2, 0);
    g.addEdge(2, 3);
    g.addEdge(3, 3);

    if(g.isCyclic()){std::cout << "Graph contains cycle\n\n";}
    else {std::cout << "Graph doesn't contain cycle\n\n";}


    /// Thread cleanup
    pthread_exit(NULL);
    /// end of thread exit

    std::cout << std::endl;
    return 0;
}
