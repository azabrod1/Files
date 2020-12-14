//
//  search.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/26/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef search_hpp
#define search_hpp

#include <stdio.h>
#include "tarjanStack.h"
#include "cell.h"
#include <unordered_set>
#include <stack>
#include "directedHashGraph.h"



class Search{
    
private:
    
    Graph<Vid>& graph;
    Vid cellCount = 0;
    TarjanStack<Vid> stack;
    Search* waitingFor;
    unordered_map<Vid, Cell<Vid>*>& dict;
    
    std::stack<Cell<Vid>> controlStack; //Simulates the call stack to avoid recursion 
    
    //The strongly connected components discovered by this search represented by linked lists
    std::unordered_set<Cell<Vid>*> SSCs;

    
    
public:
    
    Search(Graph<Vid>& _graph, unordered_map<Vid, Cell<Vid>*>& _dict);
    void run();
    void exploreNext();
    
    /*Adds the cell to the search by assigning it an index*/
    inline void addToSearch(Cell<Vid>* cell){
        
        cell->index = cellCount;
        cell->rank  = cellCount;
        ++cellCount;
        
    }
    
};

#endif /* search_hpp */

