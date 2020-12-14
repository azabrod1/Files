//
//  worker.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/10/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef worker_hpp
#define worker_hpp

#include <stdio.h>
#include "search.h"
#include "typedefs.h"
#include "graph.h"
#include "dictionary.h"
#include "stealingQueue.hpp"

class MultiThreadedTarjan;

template <class V> class Cell;
//Make sure pass ref
class Worker{
    
    friend class MultiThreadedTarjan;
    
private:
   
    //Variables
    MultiThreadedTarjan& scheduler;
    Cell<Vid>* spareCell;
    SearchPtr spareSearch;
    const Graph<Vid>& graph;
    Dictionary<Vid, Cell<Vid>*>& dict;
    const unsigned int ID;
    const long MASK;
    SCC_Set SCCs;
    SimpleClock sc;
    
    //Methods
    void explore(const SearchPtr&  search);
    
    void initNeighbors(Cell<Vid>* cell);
    
    void buildSCC(const SearchPtr&, Cell<Vid>*);
    
    void buildSCC2(const SearchPtr&, Cell<Vid>*);

    void addLoneSCC(Cell<Vid>* cell);
    
    void cleanUp();

    
public:

    Worker(unsigned int _ID, MultiThreadedTarjan& _algo, const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict);
    void operator()();
    
    
    
};




#endif /* worker_hpp */
