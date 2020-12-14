//
//  algorithmManager.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/3/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef suspensionManager_hpp
#define suspensionManager_hpp

#include <stdio.h>
#include <mutex>
#include "typedefs.h"
#include "search.hpp"
#include <vector>
#include "stealingQueue.hpp"

#define SUSPEND true

#define RESUME false

class SuspensionManager{
    
private:
    
    /*Functions to suspend searches and retrieve which searches are suspended on which
     We abstact away the implementation of these functions from the rest of the class so that
     it is easy to change how we track suspended seaches by just altering these functions */
    
    inline SearchPtr suspendedOn(const SearchPtr& search){
        return search->getBlockingSearch();
    }
    
    inline void suspend(const SearchPtr& toSuspend, const SearchPtr& on){
        toSuspend->setSuspendedOn(on);
    }
    
    inline bool isSuspended(const SearchPtr& search){
        return search->getBlockingSearch() != nullptr;
    }
    
    inline void unSuspend(const SearchPtr& search){
        search->setSuspendedOn(nullptr);
    }
    
    void runCellTransfer(const SearchPtr& S, const std::vector<SearchPtr>& path);
    
    std::mutex suspendMu;



public:
    
    bool suspend(const SearchPtr& S, Cell<Vid>* conflictCell);
    
    void bulkUnsuspend(std::vector<SearchPtr>* const suspendedList, Cell<Vid>* prevBlockedOn);
    
    
};


#endif /* suspensionManager_hpp */
