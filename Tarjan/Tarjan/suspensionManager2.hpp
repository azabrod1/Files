//
//  suspensionManager2.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef suspensionManager2_hpp
#define suspensionManager2_hpp

#include <stdio.h>
#include <mutex>
#include "stealingQueue.hpp"
#include <vector>

#define SUSPEND true

#define RESUME false

class SuspensionManager2{
    
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
        search->removeCellBlockedOn();
    }
    
    void runCellTransfer(const std::vector<SearchPtr>& S, const std::vector<Cell<Vid>*>& C);
    
    std::mutex suspendMu;

    
public:
    
    bool suspend(const SearchPtr& S, Cell<Vid>* conflictCell);
    
    void bulkUnsuspend(std::vector<SearchPtr>* const suspendedList, Cell<Vid>* const prevBlockedOn);
    
    
};



#endif /* suspensionManager2_hpp */
