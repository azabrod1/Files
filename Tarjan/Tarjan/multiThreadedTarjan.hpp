//
//  multiThreadedTarjan.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef multiThreadedTarjan_hpp
#define multiThreadedTarjan_hpp

#include <stdio.h>
#include "typedefs.h"
#include "search.hpp"
#include "suspensionManager.hpp"
#include "suspensionManager2.hpp"
#include "pending.hpp"
#include "stealingQueue.hpp"
#include "worker.hpp"
#include "utilities.hpp"


class MultiThreadedTarjan{
    

private:
    
    //Constants
    const unsigned int NUM_THREADS;
    const long ALL_FLAGS_SET;
    
    //Data structures
    const Graph<Vid>& graph;
    Dictionary<Vid, Cell<Vid>*>&  dict;
    SuspensionManager2 susMgr;
    Pending& pending;
    StealingQueue& cellQueue;
    
    //Member Variables
    std::atomic<long> flags{0};
    
public:
    
    SCC_Set*  run();

    SearchPtr getSearch(Worker* worker);
    

    inline bool suspend(const SearchPtr S, Cell<Vid>* conflictCell){
        return susMgr.suspend(S, conflictCell);
    }
    
    inline void resumeAllBlockedOn(Cell<Vid>* completeCell){
        std::vector<SearchPtr>* suspendedList(completeCell->removeBlockedList());
        
        if(!suspendedList->empty()){
            
            susMgr.bulkUnsuspend(suspendedList, completeCell);
            pending.addPending(suspendedList);
            
        }
        
        delete suspendedList;
    }
  
    
    MultiThreadedTarjan(const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict, unsigned int num_threads , Pending& _pending, StealingQueue& _queue) : graph(_graph), dict(_dict), NUM_THREADS(num_threads), ALL_FLAGS_SET((1 << num_threads) - 1), pending(_pending), cellQueue(_queue){
        
        
        
    }
   
    ~MultiThreadedTarjan();
    
    SCC_Set*  runWithOneThread();

    
};

#endif /* multiThreadedTarjan_hpp */

