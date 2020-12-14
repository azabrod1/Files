//
//  node.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/25/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef cell_h
#define cell_h

#include "typedefs.h"
#include <vector>
#include <mutex>
#include <iostream>
#include <algorithm>
#include "search.hpp"


template <class V>
struct Cell{
    
private:
    
    //We save the neighbor queue as a pointer so we can delete it
    //after we
    std::vector<Cell<V>*>* neighborQueue; //Neighbors to visit
    std::mutex  mu;
    std::vector<SearchPtr>* blockedOn;     //What searches are blocked on this cell
    
public:
    V           vertex;
    Vid         index;
    Vid         rank;
    Status      status;
    bool        transferred;
    bool        root = false;

    
    Cell(){
        transferred = false;
        neighborQueue = nullptr;
        status = CellStatus::NEW_CELL;
        blockedOn = new std::vector<SearchPtr>;
    }
    
    Cell(V _vertex) : Cell(){
        this->vertex = _vertex;
        
    }
    
    ~Cell(){  }
    
    /*Non synchronized methods ;for use when only a guess of the cell status is desired
     Many of these methods have synchronized analogs if it is neccesary
    ***********************************************************************************/

    
    inline Search* owner(){
       // if(status == CellStatus::NEW_CELL)
        //    return nullptr;
        
        return status;
    }

    
    inline bool isComplete(){

        return status == CellStatus::COMPLETE_CELL;
    }
    
    inline bool isNew(){
        return status == CellStatus::NEW_CELL;
    }
    
    inline bool isUnclaimed(){
        return status == CellStatus::NEW_CELL || status == CellStatus::COMPLETE_CELL;
    }
    
    inline bool isOwnedBy(Search* search){
        
        return search == status;
    }
    /* Pre: Must be the owner of the cell and all its neighbors must have been explored
     * This function effectivly retires the cell
     */
    inline void markComplete(){
        std::lock_guard<std::mutex> lock(mu); //Can prob remove sync if careful
        status = CellStatus::COMPLETE_CELL;
    }
    
    inline std::vector<SearchPtr>* removeBlockedList(){
        std::vector<SearchPtr>* toReturn = blockedOn;
        blockedOn = nullptr;
        return toReturn;
        
    }
    
    inline void initIndex(const Vid& idx){
        this->index = idx; this->rank = idx;
    }
    
    inline void promote(Vid rankUpdate){
        this->rank = std::min(this->rank, rankUpdate);
    }
    
    /*Synchronized methods
     ***********************************************************************************/
    
    /** Transfer: The function changes the stack label of the cell to another search, officially transfering
     * ownership of the cell. Delta gives the cell an updated index to match the indicies of the new stack.
     * We add any searches that were blocked on the cell to the passed in by reference vector
     *blockedSearches
     */
    
    
    inline void transfer(Vid delta, Status const newOwner, std::vector<SearchPtr>& blockedSearches){
        std::lock_guard<std::mutex> lock(mu);
        transferred = true; 
        rank +=delta; index += delta;
        status = newOwner;
        for(SearchPtr search: *blockedOn)
            blockedSearches.push_back(search);
        
    }
    
    inline void transfer(const Vid delta, Status const newOwner){
        std::lock_guard<std::mutex> lock(mu);

        transferred = true;
        rank +=delta; index += delta;
        status = newOwner;
    }
    
    

    
    
    /*Removes a search from the cell's blocked on list*/
    void unblockSearch(Search* const search){
        std::lock_guard<std::mutex> lock(mu);
        
        for(auto it = blockedOn->begin(); it != blockedOn->end(); ++it){
            if((*it).get() == search){
                blockedOn->erase(it);
                return;
            }
        }
    }
    
    //Synchronized version of the owner() method
    inline SearchPtr getOwner(){
        
        std::lock_guard<std::mutex> lock(mu);
        
        if(status == CellStatus::NEW_CELL || status == CellStatus::COMPLETE_CELL) return nullptr;
        
        assert(status != nullptr);
        
        return status->shared_from_this();
    }
    
    
    inline void blockSearch(const SearchPtr& search){
        std::lock_guard<std::mutex> lock(mu);
        blockedOn->push_back(search);
    }

    //Returns true if and only if the search successfully took ownership of the cell
    //If conquer succeeds, the cell should be added to the conquerer's stack
    char conquer(const SearchPtr& conquerer){
        
        //We need to synchronize the part where the search takes ownership of the
        //cell as more than one search may try to claim it at once
        
        std::lock_guard<std::mutex> lock(mu);
        
        if(isComplete()) //cell complete  (#TODO: move outside lock also)
            return CellStatus::COMPLETE;
        if(!isNew()){ //Somebody owns the cell, so we cant conquer it
        
            blockedOn->push_back(conquerer);

            return CellStatus::OCCUPIED;
        }
        
        //Otherwise, nobody owns the cell and it is not complete so we can claim it
        
        status = conquerer.get();
        
        return CellStatus::CONQUERED;
        
    }
    
    /**Attemts to conquer the cell on behalf of the search conquerer so that it can be added 
     *to conquerer's stack. It is similar to the function above, but it either succeeds in
     *conquering the cell and returns true or fails and returns false and does not 
     *put the search on the cell;s blockedOn list. 
     * The main difference between this function and the normal conquer is that if the search fails
     *to conquer the cell, nothing else happens so it is as if the search never tried.
     */
    bool conquerOrFail(Search* S){
        
        std::lock_guard<std::mutex> lock(mu);

        if(isNew()){
            status = S;
            return true;
        }
        else
            return false;
    
    }
    
    /*** Methods Not Requiring Sychronization Due To Cell Ownship Precondition******/
    
    /* All methods in this section assume the cell is owned by the search calling the method*/
    
   
    //The purpose of the method is to label a cell which is reserved for a specific owner
    //and hence faces no race conditions from competing threads 
    inline void setOwner(Search* newOwner){
        this->status = newOwner;
    }
    
    
    inline void setNeighbors(std::vector<Cell<V>*>* neighbors){
        if(neighbors->size() == 0) //Cell has no neighbors
            delete neighbors;
        else
            this->neighborQueue = neighbors;
        
    }
    
    inline bool allNeighborsDone(){
        return neighborQueue == nullptr; //the queue is deleted when we run out of neighbors
    }
    
    /*Returns a neighbor of this node to be explored. 
     The function tries to choose a node that is not currently 
     being explored by another search if one exists.
     The function assumes the neighbors queue is nonempty*/
    inline Cell<Vid>* getBestNeighbor(){

        Cell<Vid>* candidate = neighborQueue->back();
        
        //If the next element in the queue is not owned by another search,
        //return it
        if(candidate->isUnclaimed()|| candidate->isOwnedBy(this->owner()))
            neighborQueue->pop_back();
        
        //otherwise try to find a cell in the queue that is not occupied by another
        //search to avoid conflict. If non found, we hace no choice but to return an occupied cell
        //Note that this is not sychronized, so the cell might become occupied by the time
        //we return. This is ok since we make no guarantee that we return an unoccupied cell,
        //we just try to do so as an optimization
        
        else{
            
            auto it = neighborQueue->begin();
            
            //return first unoccupied cell
            while(it != neighborQueue->end()){
                candidate = *it;
                
                //is this neighbor occupped? If the neighbor is unclaimed or owned
                //by the current search, we return it for inspection
                if(candidate->isUnclaimed() || candidate->isOwnedBy(this->owner()))
                    break;
                ++it;
            }
            
            if(it == neighborQueue->end())
                neighborQueue->pop_back();
            
            else
                neighborQueue->erase(it);
        }
        
        
        //delete the old array to save memory once elements in the queue have been used up
        //This is important because a graph may have > 100,000 verticies so having a neighbor array for
        //all of them will result in a massive memory footprint
        
        if(neighborQueue->size() == 0){
            delete neighborQueue;
            neighborQueue = nullptr;
        }
        
        return candidate;
    }
    

};




#endif /* node_h */
