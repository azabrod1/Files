//
//  directedHashGraph.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/24/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//
// Implements the graph class in an ajacency list representation
//

#ifndef directedHashGraph_h
#define directedHashGraph_h

#include "graph.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

template <class V>
class DirectedHashGraph: public Graph<V>{
    
private:
    std::unordered_map<V, std::unordered_set<V>* > edges;
    
    
public:
    void insertVertex(V vertex);
    void insertEdge(V from, V to);
    const std::unordered_set<V>& getNeighbors(V vertex) const;
    std::unordered_set<V>* getVerticies() const;
    inline int size() const {return (int) edges.size();}
    void removeVertex(V vertex);
    void removeEdge(V from, V to);
    void bulkInsertEdges(const initializer_list<pair<V,V>>& list);
    bool edgeExists(V from, V to) const ;
    virtual ~DirectedHashGraph(){
        //Delete the unordered sets of verticies
        for(auto& vertex: edges)
            delete vertex.second;
        
        
    }
    
};

template<class V>
void DirectedHashGraph<V>::insertVertex(V vertex){
    
    if(edges.count(vertex) == 0) //If the vertex is not in the graph, add it
        edges[vertex] = new unordered_set<V>;
    
    
}

template<class V>
void DirectedHashGraph<V>::insertEdge(V from, V to){
    
    if(edges.count(from) == 0) //If the vertex is not in the graph, add it
        edges[from] = new unordered_set<V>;
    
    if(edges.count(to) == 0) //If the vertex is not in the graph, add it
        edges[to] = new unordered_set<V>;
    
    edges[from]->insert(to);
    
}

template<class V>
const unordered_set<V>& DirectedHashGraph<V>::getNeighbors(V vertex) const {
    return *(edges.at(vertex));
}

template<class V>
unordered_set<V>* DirectedHashGraph<V>::getVerticies() const {
    
    unordered_set<V>* vertices = new unordered_set<V>;
    
    for(auto& vertex: edges)
        vertices->insert(vertex.first);
        
    return vertices;
    
}

template<class V>
void DirectedHashGraph<V>::removeVertex(V vertex){
    edges.erase(vertex);
}

template<class V>
void DirectedHashGraph<V>::removeEdge(V from, V to){
    
    edges[from]->erase(to);
}

template<class V>
void DirectedHashGraph<V>::bulkInsertEdges(const initializer_list<pair<V,V>>& list){
    
    for(auto& pair: list)
        insertEdge(pair.first, pair.second);
    
    
}

template<class V>
bool DirectedHashGraph<V>::edgeExists(V from, V to) const {
    return (edges.at(from)->count(to)) != 0;
}


#endif /* directedHashGraph_h */
