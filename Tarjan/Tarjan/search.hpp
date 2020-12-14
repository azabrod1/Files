//
//  search.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/26/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef search_hpp
#define search_hpp

#include <stdio.h>
#include <unordered_set>
#include <stack>
#include "directedHashGraph.h"
#include "mutexDict.h"
#include "typedefs.h"

typedef Search* Status;

template <class V>
class Cell;
class MultiThreadedTarjan;

typedef std::deque<Cell<Vid>*> TarjanStack;
typedef std::deque<Cell<Vid>*> ControlStack;


struct Search : std::enable_shared_from_this<Search>{
    
private:
    bool deleted = false;
    SearchPtr searchSuspendedOn;
    std::atomic<Cell<Vid>*> cellBlockedOn;
    TarjanStack  tarjanStack;
    ControlStack controlStack; //Simulates the call stack to avoid recursion    
    Vid cellCount;

    
public:
    //CellStatus* const STACK_STAMP; //The search's unique label that it stamps on a cell it occupies

    void pushToStacks(Cell<Vid>* cell);

    int trans = -1; //0 = src, 1 = dest, 2 = both

    TarjanStack* getTS(){
        return &tarjanStack;
    }
    
    ControlStack* getCS(){
        return &controlStack;
    }
    
    void buildSCC(Cell<Vid>* const head, SCC_Set* const SCCs);

    
    Search();
    void setRoot(Cell<Vid>* root);
    bool run(SCC_Set* const SCCs);
    inline void operator()(SCC_Set* const SCCs){
        run(SCCs);
    }
    
    
    inline void setCellBlockedOn(Cell<Vid>* const _cell){
        cellBlockedOn = _cell;
    }
    /*We use a compare and exchange when the search may have become blocked on a new cell before the function call
     *This can happen if the search resumes before an unsuspend finishes */
    inline bool removeCellBlockedOn(Cell<Vid>* blockedOn){
        return cellBlockedOn.compare_exchange_strong(blockedOn, nullptr);
    }
    
    inline bool suspendedOnCompareAndExchange(Cell<Vid>* expected, Cell<Vid>* desired){
        return cellBlockedOn.compare_exchange_strong(expected, desired);
    }

    //use only when you are sure there will not be race condition, otherwise use the one above.
    inline void removeCellBlockedOn(){
        cellBlockedOn = nullptr;
    }
    
    inline Cell<Vid>* tarjanStackTop(){
        return tarjanStack.back();
    }
    
    inline Cell<Vid>* controlStackTop(){
        return controlStack.back();
    }
    
    inline bool controlStackEmpty(){
        return controlStack.empty();
    }
    
    inline void controlStackPop(){
        controlStack.pop_back();
    }
    
    inline void tarjanStackPop(){
        tarjanStack.pop_back();
    }
    
    inline void setSuspendedOn(const SearchPtr& blockOn){
        searchSuspendedOn = blockOn;
    }

    
    inline void removeSuspendedOn(){
        searchSuspendedOn = nullptr;
    }
    
    inline SearchPtr getBlockingSearch(){
        return searchSuspendedOn;
    }
    
    inline bool isSuspended(){
        return searchSuspendedOn != nullptr;
    }
    
    inline Cell<Vid>* getBlockingCell(){
        return cellBlockedOn;
    }
    
    void refreshCellCount();
    
    inline bool done(){
        return controlStack.empty();
    }
    
    static void transferCells(const SearchPtr& src, const SearchPtr& dest, Cell<Vid>*& conflictCell, std::vector<SearchPtr>& blockedSearches);
    
    static void lockFreeTransfer(const SearchPtr& src, const SearchPtr& dest, Cell<Vid>* conflictCell);

    
    //methods for testing the class
    static void testCellTransfer(const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict);
    
    static void testCellTransfer2();
    static void testCellTransfer3();


    void printStacks();
    
    ~Search();

};

#endif /* search_hpp */

