//
// Created by Ashwi on 4/17/2022.
//

//
// Created by Seun Suberu on 4/3/22.
//
#include "iostream"
#include <list>
#include <map>
using namespace std;
#ifndef PROGRAM_3_DGRAPH2_H
#define PROGRAM_3_DGRAPH2_H
template <typename T>
class DGraph2 {
private:
    map<T, list<PairWrapper<T> >*> edges;
    typename map<T, list<PairWrapper<T> >*>::iterator vertexIterator;
    map<T, typename list<PairWrapper<T> >::iterator> edgeIterators;
public:
    DGraph2(){

    }
    DGraph2(DGraph2<T>& source){
        this->edges =  source.edges;
    }
    DGraph2(DGraph2<T>* source){
        this->edges =  source->edges;
    }

    //add edge
    void addEdge(T from, T to, double weight) {
        if (edges[from] == nullptr) {
            edges[from] = new list<PairWrapper<T> >();
        }
        for (PairWrapper<T> pair : *(edges[from])) {
            if (pair.getVertex() == to) {
                return;
            }
        }
        PairWrapper<T> newPair(to, weight);
        edges[from]->push_back(newPair);
        edgeIterators[from] = edges[from]->begin();
    }

    // remove edge
    void removeEdge(T from, T to) {
        list<PairWrapper<T> >* innerList = edges[from];
        if (innerList == nullptr) {
            return;
        }
        for (auto itr = innerList->begin(); itr != innerList->end(); itr++) {
            if (itr->getVertex() == to) {
                innerList->erase(itr);
                if (edges[from] == nullptr || edges[from]->size() == 0) {
                    edges.erase(from);
                }
                return;
            }
        }
    }

    //print graph
    void printAdjacencyList() {
        for(const auto& keyVal : edges) {
            list<PairWrapper<T> >* innerList = keyVal.second;
            cout << keyVal.first << ": " ;
            for (PairWrapper<T> pair : *innerList) {
                cout << pair.getVertex() << " " ;
            }
            cout << endl;
        }
        cout << endl;
    }

    //print graph based on weight
    void printAdjacencyList(double weight) {
        string out;
        for(const auto& keyVal : edges) {
            list<PairWrapper<T> >* innerList = keyVal.second;
            if(innerList == nullptr) {
                continue;
            }
            for (PairWrapper<T>& pair : *innerList) {
                if(pair.getWeight() == weight) {
                    out += "(" + keyVal.first + "," + pair.getVertex() + "):";
                }
            }
        }
        cout << out.substr(0, out.size() - 1);
        cout << endl;
    }

    //returns if edge exists
    bool edgeExists (T from, T to) {
        if(!edges.count(from)) {
            return false;
        }
        list<PairWrapper<T> >* innerList = (edges[from]);
        if (innerList == nullptr) {
            return false;
        }
        for (PairWrapper<T> pair: *innerList) {
            if (pair.getVertex() == to) {
                return true;
            }
        }
        return false;

    }

    // returns if vertex exists
    bool vertexExists (T from) {
        return edges[from] != nullptr ? edges.count(from) : false;
    }

    // returns weight of edge
    double getEdgeWeight (T from, T to) {
        list<PairWrapper<T> > innerList = *(edges[from]);
        for (PairWrapper<T>& pair : innerList) {
            if (pair.getVertex() == to) {
                return pair.getWeight();
            }
        }
        return 0;
    }

    // start iteration through vertexes
    typename map<T, list<PairWrapper<T> >*>::iterator startVertexIterator () {
        vertexIterator = edges.begin();
        return vertexIterator++;
    }

    // returns next vertex iteration
    typename map<T, list<PairWrapper<T> >*>::iterator nextVertexIterator () {
        return vertexIterator++;
    }

    //check if there is a next vertex iterator
    bool hasNextVertexIterator () {
        return vertexIterator != edges.end();
    }

    // start iteration through edges of vertex
    typename list<PairWrapper<T> >::iterator startEdgeIterator (T vertex) {
        edgeIterators[vertex] = edges[vertex]->begin();
        return edgeIterators[vertex];
    }

    // returns next edge iteration
    typename list<PairWrapper<T> >::iterator nextEdgeIterator (T vertex) {
        return ++edgeIterators[vertex];
    }

    //check if there is a next edge iterator
    bool hasNextEdgeIterator (T vertex) {
        auto x = edgeIterators[vertex];
        x++;
        return x != edges[vertex]->end();
    }

    // variation of Geeks For Geeks implementation of cycle detection, https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
    bool isCyclicUtil(T v, map<T, bool>* visited, map<T, bool>* recStack)
    {
        if((*visited)[v] == false)
        {
            // Mark the current node as visited and part of recursion stack
            (*visited)[v] = true;
            (*recStack)[v] = true;
            if(vertexExists(v)) {
                if (numberOfVertexEdges(v) > 0) {
                    T res = startEdgeIterator(v)->getVertex();

                    while (true) {
                        if (!(*visited)[res] && isCyclicUtil(res, visited, recStack)) {
                            return true;
                        } else if ((*recStack)[res]) {
                            return true;
                        }
                        if (hasNextEdgeIterator(v)) {
                            res = nextEdgeIterator(v)->getVertex();
                        } else {
                            break;
                        }
                    }
                }
            }

        }
        (*recStack)[v] = false;  // remove the vertex from recursion stack
        return false;
    }

    bool isCyclic()
    {
        // Mark all the vertices as not visited and not part of recursion
        // stack
        map<T, bool>* visited = new map<T, bool>();
        map<T, bool>* recStack = new map<T, bool>();
        for(const auto& keyVal : edges)
        {
            list<PairWrapper<T> >* innerList = keyVal.second;
            if(innerList != nullptr and !innerList->empty()) {
                T val = innerList->front().getVertex();
                (*visited)[val] = false;
                (*recStack)[val] = false;
            }
        }

        // Call the recursive helper function to detect cycle in different
        // DFS trees
        for(const auto& keyVal : edges) {
            list<PairWrapper<T> >* innerList = keyVal.second;
            T val = keyVal.first;
            if(innerList != nullptr) {
                if (!(*visited)[val] && isCyclicUtil(val, visited, recStack))
                    return true;
            }
        }
        return false;
    }

    // returns if vertex has any edges
    bool vertexHasEdges(T from) {
        if(!edges.count(from)) {
            return false;
        }
        return edges[from] != nullptr ? edges[from]->size() > 0 : false;
    }

    // drops all vertex edges leaving the list empty
    void dropAllVertexEdges(T vertex) {
        if (edges[vertex] != nullptr) {
            edges[vertex]->clear();
        }
    }

    // return number of edges for a particular vertex
    int numberOfVertexEdges(T vertex) {
        return edges[vertex] != nullptr ? edges[vertex]->size() : 0;
    }

    //sets the weight of an existing edge
    void setEdgeWeight(T from, T to, double weight) {
        list<PairWrapper<T> >* innerList = (edges[from]);
        if (innerList == nullptr) {
            return;
        }
        for (PairWrapper<T>& pair : *innerList) {
            if (pair.getVertex() == to) {
                pair.setWeight(weight);
            }
        }
    }
};

#endif //PROGRAM_3_DGRAPH2_H
