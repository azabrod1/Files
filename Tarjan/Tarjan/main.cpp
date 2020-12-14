//
//  main.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/24/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include "typedefs.h"
#include "unordered_map"
#include "search.hpp"
#include "tarjan.hpp"
#include <thread>
#include "mutexDictIterator.hpp"
#include "simpleClock.h"
#include "utilities.hpp"
#include "SimpleSharded.h"
#include "suspensionManager2.hpp"


using namespace std;

void testWithRandomGraphs();
void testSingles();
void deleteSCCs(SCC_Set*);

class M{
public:
    M(int _x) {x = _x;}
    int x;
    int array[5];
    bool operator==(M &other){
        return this == &other;
    }
    M(M&& m){
        x = m.x;
        array[0] = 13;
        m.array[0] = 34;
    }
    M(M& m)  = default;
};


class W{
public:
    M m;
    W(M _m): m(_m){;}
    
    
};


mutex mu;
int x; int N = 10;

//Dictionary<int, int>* map = new MutexDict<int, int>;

unordered_map<int, M*> umap;

void test3(int num){
    volatile long i = 0; volatile long k = 10000000000;
    while(i < k){
        ++i;
    }
    
    cout << i <<endl;
    
}

void sleeps(int secs){
    std::this_thread::sleep_for(std::chrono::seconds(secs));
}


int main(int argc, const char * argv[]) {

   // thread t1(test3,0);  thread t2(test3,1); thread t3(test3,2); thread t4(test3, 3);
   // t1.join();  t2.join();  t3.join(); t4.join();
    
        //Search::testCellTransfer2();
   
    //thread t1(test3,0);  thread t2(test3,1); thread t3(test3,1);
  //  t1.join();  t2.join();
    
    testWithRandomGraphs();

    //Search::testCellTransfer3();
    
    SuspensionManager2 s;
    
    return 0;
}

void testSingles(){
    
    DirectedHashGraph<Vid> graph;
    
    graph.insertEdge(0,4);  graph.insertEdge(4,5); graph.insertEdge(5,6); graph.insertEdge(5, 5);
    graph.insertEdge(6, 4); graph.insertEdge(0, 5);
    graph.insertEdge(10, 15);     graph.insertEdge(15, 10);


    graph.insertEdge(10, 106);
    graph.insertEdge(101, 102);
    graph.insertEdge(102, 103);
    graph.insertEdge(103, 101);
    graph.insertEdge(104, 103);
    graph.insertEdge(104, 102);
    graph.insertEdge(104, 105);
    graph.insertEdge(105, 105);
    graph.insertEdge(105, 104);
    graph.insertEdge(105, 106);
    graph.insertEdge(106, 103);
    graph.insertEdge(106, 107);
    graph.insertEdge(107, 106);
    graph.insertEdge(108, 108);
    graph.insertEdge(108, 107);
    graph.insertEdge(108, 109);
    graph.insertEdge(109, 110);
    graph.insertEdge(110, 111);
    graph.insertEdge(109, 112);
    graph.insertEdge(112, 112);
    graph.insertEdge(113, 112);
    graph.insertEdge(113, 114);
    graph.insertEdge(114, 113);
    graph.insertEdge(114, 109);
    graph.insertEdge(200, 201);
    graph.insertEdge(200, 202);
    graph.insertEdge(201, 203);
    graph.insertEdge(201, 205);
    graph.insertEdge(202, 206);
    graph.insertEdge(203, 205);
    graph.insertEdge(204, 202);
    graph.insertEdge(205, 201);
    graph.insertEdge(205, 207);
    graph.insertEdge(206, 204);
    graph.insertEdge(207, 205);
    graph.insertEdge(209, 203);
    


    SCC_Set* SCCs  = Tarjan::singleThreadedTarjan(graph);
    SCC_Set* SCCs2 = Tarjan::multiThreadedTarjan(graph);

    
    for(auto& set: *SCCs){
        
        cout<< "(";
        
        for(auto& vert: *set)
            cout << vert << " ";
        
        
        
        cout <<")" << endl;
        
    }
    
    cout << SCCs->size() << " " << SCCs2->size() <<endl;
    
    delete SCCs; delete SCCs2;
    
}


void testWithRandomGraphs(){
    // insert code here...
    
    SimpleClock c;

   // Graph<Vid>* graph  = Utility::generateRandomGraph(0.0001,40000);

    //Graph<Vid>* graph  = Utility::GeoGenerateRandomGraph(0.0057, 1, 3000);
   
    Graph<Vid>* graph   = Utility::clusters(1000, 1000, 5, 25);
    
    
    c.begin();
    auto sccs = Tarjan::singleThreadedTarjan(*graph);
    c.end();
    
    cout << sccs->size() <<endl;

    
    const int N(200);
    SCC_Set* sets[N];
    
    for(int i = 0; i < N; ++i){
        
        c.begin();
        sets[i] = Tarjan::multiThreadedTarjan(*graph,4);
        c.end();
        //if(!i)
        //    for(auto set: *sets[i]){
          //      cout << set->size() <<endl;
          //  }
        cout << "cells leaked " << s << endl;
        cout << sets[i]->size() <<endl;
        Utility::deleteSCCs(sets[i]);
    }
    
    
    
    c.begin();
    auto sccs2 = Tarjan::singleThreadedTarjan(*graph);
    c.end("Single Threaded Tarjan");
    cout << sccs->size() << endl;

    cout << sccs2->size() << endl;
    
    //for(int i =0; i < N; ++i)
    //    cout << sets[i]->size() <<endl;


    
    int set1Card(0); int set2Card(0);
    for(auto set: *sccs){
        set1Card+= set->size();
        
    }
    
    for(auto set: *sccs2){
        set2Card+= set->size();
    }
    
    
    cout << set1Card << "   :   " << set2Card <<endl;


    delete graph;
    Utility::deleteSCCs(sccs); Utility::deleteSCCs(sccs2);


}
