//
//  algorithmManager.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/3/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "suspensionManager2.hpp"

bool SuspensionManager2::suspend(const SearchPtr& Sn, Cell<Vid>* const conflictCell){
    
    
    //There is no need to suspend if the cell became complete
    if(conflictCell->isComplete()) return RESUME;
    
    
        //Conflict cell seems to be in progress, so mark this search as suspended
        
        Sn->setCellBlockedOn(conflictCell);

        //Find the search S1 that is directly blocking S
        SearchPtr S1(conflictCell->getOwner());
    
        /*The cell may have become complete and all searches associated with it unblocked just before we 
         set cellBlockedOn. In this case, the search would never become unblocked since it was regestered as suspended 
         after unsuspend() was called. To fix this issue, we check if the cell became complete while we 
         suspended the search, and unsuspend it here if the cell did in fact become complete
         */
        
        //In case resumeAll() already happened, we do its work for it and remove cell blocked on
        if(conflictCell->isComplete()){
            if(Sn->suspendedOnCompareAndExchange(conflictCell, nullptr))
                return RESUME;
            else
                return SUSPEND;
        }
    
        std::vector<SearchPtr> S; std::vector<Cell<Vid>*> C;


    
        /******* Check for cycle ********/
    
        //Path: first pass
        
        SearchPtr Si(Sn); Cell<Vid>* Ci;
        
        //is there already a blocking path from S1 to Sn?
        
        
        /*S1 --> S2 -->  S3 --> S4 -->...---> Sn-->..
          ^       ^      ^       ^            ^
          |       |      |       |            |
          C1      C2    C3       C4           Cn
         
         */
        
        do{
            Ci = Si->getBlockingCell(); //Here Si is S, Si must exist due to use of smart pointer
            if(!Ci) return SUSPEND;
            
            Si = Ci->getOwner();
            if(!Si) return SUSPEND;
            
            S.push_back(Si);
            C.push_back(Ci);
            
            
        }while(Si != Sn);
        
        //Second pass
        
        Si = Sn;
        
        Cell<Vid>* minPtr(C[0]); //We look for cell with the smallest pointer value

        int lo(0);               //Index of cell with smallest pointer value
        
        int pathSize(S.size());
        
        for(int i = 0; i < pathSize; ++i){
            
            Ci = Si->getBlockingCell();
            
            if(Ci != C[i])
                return SUSPEND;
            
            Si = Ci->getOwner();
            
            if(Si != S[i])
                return SUSPEND;
            
            if(Ci->isComplete())
                return SUSPEND;
            
            if(Ci < minPtr){
                minPtr = Ci;
                lo = i;
            }
        
        }
        //Was the link leading to the second path valid?
        if(C[0] != conflictCell || C[0]->isComplete()) ///May need to check S[0]
            return SUSPEND;
        
        //If we get here, the loop is confirmed
        
        //Acts as a consensus protocol
        if(!S[(lo-1+pathSize)%pathSize]->suspendedOnCompareAndExchange(minPtr, nullptr))
            return SUSPEND;
        
    runCellTransfer(S, C);

    //Last two what do we do????
    Sn->removeCellBlockedOn();
    conflictCell->unblockSearch(Sn.get());
    
    return RESUME;
    
}

/**
 @param S The blocking path from the search directly blocking S, sn, all the way to S
 
 The path vector is represented as this:
 
 s1 < ----- s1 <----s2 <--- ....<-----  sn-1 <----- sn
 
 */

void SuspensionManager2::runCellTransfer(const std::vector<SearchPtr>& S, const std::vector<Cell<Vid>*>& C)
{
    const SearchPtr& Sn(S.back());
    
    
    //Traverse the path starting with the search that S is directly blocked on
    for(int i = 0; i < S.size() - 1; ++i){
        Search::lockFreeTransfer(S[i], Sn, C[i]);
        C[i+1]->unblockSearch(S[i].get());
    }
    
    
    //Makes sure S is "aware" that all transferred cells are in the same SCC,
    //make the new top of S's tarjan stack (last of the transfered cells) have rank
    //of the cell of S that S(n-1) was blocked on
    
    Sn->controlStackTop()->promote(C.back()->index);
    
    
}

//Note: a search can only be moved from blocked on one cell to another during cycle but in that case the cell won't become complete randomly

void SuspensionManager2::bulkUnsuspend(std::vector<SearchPtr>* const suspendedList, Cell<Vid>* const prevBlockedOn){
    
    
    for(SearchPtr& search: *suspendedList){
        //This call may be from a pre-mature resume, make sure we do not accidently wake another later real suspension
        //by making sure that the cell the search is blocked on.
        //This can't fail if search is actually suspended since it would not change suspOn(Cell)
        if(!search->removeCellBlockedOn(prevBlockedOn)) //TODO: change to CAS to make more clear
            search = nullptr; //Removes the search from suspended list so we do not later add it to pending
                              //since we found out it is not actually blocked
        
        
    }
    
}



