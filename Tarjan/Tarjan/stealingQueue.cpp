//
//  StealingQueue.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/7/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "stealingQueue.hpp"

UnrootedStealingQueue::UnrootedStealingQueue(Vid* toExplore, Vid size, Dictionary<Vid, Cell<Vid>*>& _dict, int _NUM_THREADS) :
vertices(toExplore), ttlCells(size), dict(_dict), NUM_THREADS(_NUM_THREADS) {
    
    spares = new Cell<Vid>*[NUM_THREADS];
    
    for(int s = 0; s < NUM_THREADS; ++s)
        spares[s] = new Cell<Vid>;
    

}

UnrootedStealingQueue::~UnrootedStealingQueue(){
    
    for(int s = 0; s < NUM_THREADS; ++s)
        delete spares[s];
    
    delete[] spares;

}

Cell<Vid>*  UnrootedStealingQueue::next(uint w){
    Vid next(index++);
    while(next < ttlCells){
        spares[w]->vertex = vertices[next];
        auto status = dict.put(vertices[next], spares[w]);
        
        if(status.second)
            spares[w] = new Cell<Vid>;
                
        if(status.first->isNew())
            return status.first;

        
        next = index++;
    }
    return nullptr; //All done with cells
}
