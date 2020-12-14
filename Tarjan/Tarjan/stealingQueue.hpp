//
//  StealingQueue.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/7/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef StealingQueue_hpp
#define StealingQueue_hpp

#include <stdio.h>
#include <iostream>
#include "cell.h"


class StealingQueue{

public:
    //Get a cell off the queue to explore
    virtual Cell<Vid>* next(uint w) = 0;
    
    virtual void bulkInsert(std::vector<Cell<Vid>*>& cells) = 0;

    virtual bool isEmpty() = 0;
    
    virtual void clear() = 0;

};

class UnrootedStealingQueue : public StealingQueue{
private:
    
    
    Vid* const vertices;
    std::atomic<Vid> index{0};
    const Vid ttlCells;
    const int NUM_THREADS;
    Cell<Vid>** spares;
    Dictionary<Vid, Cell<Vid>*>& dict;
    
    std::mutex mu;

    
public:
    
    UnrootedStealingQueue(Vid* toExplore, Vid size, Dictionary<Vid, Cell<Vid>*>& _dict, int _NUM_THREADS);
    ~UnrootedStealingQueue();
    
    Cell<Vid>* next(uint w);
    
    inline bool isEmpty(){
        return index.load() >= ttlCells;
    }
    
    inline void clear(){
        index = 0; 
    }
    
    inline void bulkInsert(std::vector<Cell<Vid>*>& cells){
        
        throw std::exception();

    }
    
    
};







#endif /* StealingQueue_hpp */
