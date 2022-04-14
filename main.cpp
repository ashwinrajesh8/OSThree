//
// Created by Ashwi on 4/12/2022.
//
#include <iostream>
#include <sstream>
#include <fstream>
#include<bits/stdc++.h> // for Graph class stuff
#include<pthread.h>


/// Imported Graph Class starts here
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
};
/// end of player struct

/// Global vars
std::vector<player> players;
/// end of Global var declarations

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
    getline(input_file, N_string, ' ');
    N_in = std::stoi(N_string);
    getline(input_file, k_string, ' ');
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
    std::cout << players[0].name << std::endl;
    std::cout << std::endl;

    /// End of File IO

    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    g.addEdge(2, 3);
    g.addEdge(3, 3);

    if(g.isCyclic()){std::cout << "Graph contains cycle";}
    else {std::cout << "Graph doesn't contain cycle";}


    std::cout << std::endl;
    return 0;
}
