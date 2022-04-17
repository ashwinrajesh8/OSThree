//
// Created by Ashwi on 4/12/2022.
//
#include <iostream>
#include <sstream>
#include <fstream>
#include<bits/stdc++.h> // for Graph class stuff
#include<pthread.h>
#include<unistd.h>


/// Imported Graph Class starts here - source: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
class Graph
{
    int V;    // No. of vertices
    std::list<int> *adj;    // Pointer to an array containing adjacency lists
    bool isCyclicUtil(int v, bool visited[], bool *rs);  // used by isCyclic()
public:
    Graph(int V);   // Constructor
    void addEdge(int v, int w);   // to add an edge to graph
    bool isCyclic();    // returns true if there is a cycle in this graph
};

Graph::Graph(int V)
{
    this->V = V;
    adj = new std::list<int>[V];
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}

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

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/// end of Global var declarations

/// Function sending broadcast to start threads
void startSignal(){
    pthread_cond_broadcast(&cond);
    printf("\nStart signal sent, delete me\n");
}
/// End of start signal

/// Request Function
void Request(int playerID, int resourceID){     // takes in person making request, and resource to be obtained

}
/// End of Request Function

/// Release Function
void Release(int playerID, int resourceID){     // takes in person releasing, and the list of resources they are releasing

}
/// End of Release Function

/// Thread function
void *playerThread(void* z){
    const int threadID = *((int*) z);

    pthread_mutex_lock(&lock);          // lock thread until start signal
    std::cout << "Thread " << threadID << " waiting for broadcast, delete me" << std::endl;
    pthread_cond_wait(&cond, &lock);    // wait for start signal
    std::cout << "Thread " << threadID << " recieved start, delete me" << std::endl;
    pthread_mutex_unlock(&lock);        // unlock thread after signal broadcast

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
        }
        players.push_back(temp_player);
    }

    for(int i = 0; i < players.size(); i++){
        std::cout << players[i].name << " " << players[i].numNumbers << " ";
        for(int j = 0; j < players[i].numbers.size(); j ++){
            std::cout << players[i].numbers[j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    // Initialize graph to N vertices
    Graph graphy(N_in);
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


    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
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
