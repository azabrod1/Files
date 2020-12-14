//
//  main.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/24/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include "typedefs.h"
#include "graph.h"
#include "directedHashGraph.h"
#include "unordered_map"
#include "tarjanStack.h"
#include "search.hpp"
#include "utility.hpp"
#include "tarjan.hpp"


using namespace std;

void testA();
void testSingle();


int main(int argc, const char * argv[]) {
    testA();
    return 0;
}

void testSingle(){
    
}




void testA(){
    // insert code here...
    
    TarjanStack<long> stack;
    
    DirectedHashGraph<Vid> graph;
    graph.insertEdge(0,4);   graph.insertEdge(0,2);
    
    unordered_map<Vid, Cell<Vid>*> dummy;
    Search search(graph, dummy);
    
    cout << graph.size() << endl;
    
    auto vert2 = graph.getVerticies();
    
    for(auto& v : *vert2)
        cout << v << " ";
    cout << endl;
    
    
    cout << "this should be at the end" <<endl;
    
    graph.bulkInsertEdges( {make_pair(1,2),make_pair(4,5) });
    
    graph.printGraph();
    
    
    Graph<Vid>* x = Utility::GeoGenerateRandomGraph(1, 1, 100);
    
    long n = 0;
    
    
    for(auto& vert : *x->getVerticies())
        n+= x->getNeighbors(vert).size();
    
    
    cerr << n << endl;
    Tarjan::singleThreadedTarjan(*x);

    delete x;
    
    
    

}
