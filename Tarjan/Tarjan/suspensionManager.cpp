//
//  algorithmManager.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/3/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "suspensionManager.hpp"

bool SuspensionManager::suspend(const SearchPtr& S, Cell<Vid>* const conflictCell){
    
    std::vector<SearchPtr> path;
    
    //Did the conflict cell become complete (hence no need to suspend??)
    if(conflictCell->isComplete()) return RESUME;

    //mark Complete ---> then mark search-->cellBlockedOn(null) -> add cell blocked on -->rest of unsuspend()
    
    // suspendCalled(), S->cellBlockedOn(wrong), mark complete(), clean up crap :)
    
    //mark complete after check, call unsuspend() which does nothing, then add bad value :(((
    
    //Do we have a way to check if unSuspend() ran inside the lock?, yes: set complete.
    
    //if it did not run, we good. If it did, cell would be marked complete, so we to
    
    // 1) clear blocked cells AFTER synchronized unSuspend(): clear(), susp(), unsusp()...never cleared :(
    // 2) set blocking cell to NULL before resuming: because resume may have happen while ago, need undo set block on
    
    
    S->setCellBlockedOn(conflictCell);
    
    {   std::lock_guard<std::mutex> lock(suspendMu);
        
        //Find the search Sn that is directly blocking S
        SearchPtr blockingSearch(conflictCell->getOwner()); //do we need sync method?
        
        if(conflictCell->isComplete()){ //Did the cell become complete??
            S->removeCellBlockedOn(); //In case resumeAll() already happened, we do its work for it and remove cell blocked on
            return RESUME;
        }
        
        SearchPtr curr(blockingSearch);
        
        //is there already a blocking path from Sn to S?
        while(curr && curr != S){
            path.push_back(curr);
            curr = suspendedOn(curr);
        }
        
        if(!curr){ //No cycle found
            suspend(S, blockingSearch);
            return SUSPEND;
        }
    
        
    }
   
    
    runCellTransfer(S, path);
    S->removeCellBlockedOn();
    conflictCell->unblockSearch(S.get());
    
 
    return RESUME;
    
}

/**
 @param path The blocking path from the search directly blocking S, sn, all the way to S
 
 The path vector is represented as this:
 
 s < ----- s1 <----s2 <--- ....<-----  sn-1 <----- sn
 
 */

void SuspensionManager::runCellTransfer(const SearchPtr& S, const std::vector<SearchPtr>& path)
{
    Cell<Vid>* blockingCell(S->getBlockingCell());
    
    //Searches that are blocked on the cells we move
    std::vector<SearchPtr>* blockedSearches = new std::vector<SearchPtr>[path.size()];
    
    //Traverse the path starting with the search that S is directly blocked on
    for(long transferFrom = 0; transferFrom < path.size(); ++transferFrom){
        Search::transferCells(path[transferFrom], S, blockingCell, blockedSearches[transferFrom]);
    }
    
    //Synchronized block
    { std::lock_guard<std::mutex> lock(suspendMu);
        
        //For every search on the path, ammend suspended
        
        for(int i = 0; i < path.size(); ++i){
            SearchPtr Si = path[i];
            
            for(SearchPtr bs: blockedSearches[i])
                if(isSuspended(bs) && suspendedOn(bs) == Si)
                    suspend(bs, S);
            
        }
        
    }
    
    /*No race condition as non of these searches can resume before S nor can they be part of a cycle since they
     *are blocked on S which is not suspended. Thus, we can keep this outside the synchronized block*/
    
    for(SearchPtr Si: path)
        if(Si->done())
            unSuspend(Si);
        else
           suspend(Si,S);
            
    
    
    //Makes sure S is "aware" that all transferred cells are in the same SCC,
    //make the new top of S's tarjan stack (last of the transfered cells) have rank
    //of the cell in S that Sn was blocked on
    S->controlStackTop()->promote(blockingCell->index);
    
    delete[] blockedSearches;
    
}

//Note: a search can only be moved from blocked on one cell to another during cycle but in that case the cell won't become complete randomly

void SuspensionManager::bulkUnsuspend(std::vector<SearchPtr>* const suspendedList, Cell<Vid>* prevBlockedOn){
    
    std::lock_guard<std::mutex> lock(suspendMu);
    
    for(SearchPtr& search: *suspendedList){
        //This call may be from a pre-mature resume, make sure we do not accidently wake another later real suspension
        //by making sure that the cell the search is blocked on. 
        //This can't fail if search is actually suspended since it would not change suspOn(Cell)
        if(search->removeCellBlockedOn(prevBlockedOn) && search->isSuspended()){
            search->removeSuspendedOn();
        }
        else{
            search = nullptr; //Removes the search from suspended list so we do not later add it to pending
                              //since we found out it is not actually blocked
        }

    }
    
}



