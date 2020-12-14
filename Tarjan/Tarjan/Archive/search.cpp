//
//  search.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/26/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "search.hpp"


Search::Search(Graph<Vid>& _graph, unordered_map<Vid, Cell<Vid>*>& _dict) : graph(_graph), dict(_dict) {
    
}

void Search::run(){
    
    Cell<Vid>* child;
    
    while(!controlStack.empty()){
        
        Cell<Vid>& curr = controlStack.top();
        
        const unordered_set<Vid>& neighbors = graph.getNeighbors(curr.vertex); //Maybe save neighbors in cell??
        
        
        /**Try to select a neighbor that is not currently being explored by another search*/
        for(auto& vertex : neighbors){
            
            child = dict[vertex];
            
            if(child->search == this || child->search == NULL)
                break;
            
        }
        
    
    }
        
    
    
}
