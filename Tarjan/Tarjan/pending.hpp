//
//  pending.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/7/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef pending_hpp
#define pending_hpp

#include <stdio.h>
#include "stealingQueue.hpp"
#include "typedefs.h"
#include <deque>
#include "tbb/concurrent_queue.h"

class Pending{
    
public:

    virtual void addPending(std::vector<SearchPtr>* const toAdd) = 0;
    
    virtual void addPending(SearchPtr const search) = 0;
    
    virtual SearchPtr get() = 0;
    
    virtual bool isDone() = 0;
    
    virtual ~Pending(){;}
    
};


class LockedPendingQueue : public Pending{
    
    std::mutex  mu;
    std::deque<SearchPtr> waiting;
    
public:
    
    inline void addPending(SearchPtr search){
        std::lock_guard<std::mutex> lock(mu);
        waiting.push_back(search);
    }
    
    inline void addPending(std::vector<SearchPtr>* const toAdd){
        std::lock_guard<std::mutex> lock(mu);
         for(SearchPtr search: *toAdd)
             if(search) //The ones that do not need to be resumed are marked null by the suspension manager
                 waiting.push_back(search);

    }
    
    SearchPtr get(){
        if(!waiting.empty()){
            std::lock_guard<std::mutex> lock(mu);
            if(!waiting.empty()){
                SearchPtr toReturn = waiting.front();
                waiting.pop_front();
                return toReturn;
            }
        }
        
        return nullptr;
    }
    
    
    bool isDone(){
        return waiting.empty();
    }
    
};

class LockFreePendingQueue : public Pending{
    tbb::concurrent_queue<SearchPtr> waiting;
    
    inline void addPending(SearchPtr search){
        waiting.push(search);
    }
    
    inline void addPending(std::vector<SearchPtr>* const toAdd){
        for(SearchPtr search: *toAdd)
            if(search)
                waiting.push(search);
        
    }
    
    SearchPtr get(){
        
        SearchPtr toReturn;
        if(waiting.try_pop(toReturn))
            return toReturn;
        else
            return nullptr;
    }
    
    bool isDone(){
        return waiting.empty();
    }
    
};






#endif /* pending_hpp */
