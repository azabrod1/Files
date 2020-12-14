//
//  typedefs.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/28/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef typedefs_h
#define typedefs_h

#include <unordered_set>
#include <atomic>
#include <assert.h>     /* assert */
#include <thread>
#include <memory>
#include "simpleClock.h"

class Search;

typedef unsigned int Vid; //Identifier for vertexes in graph

typedef std::shared_ptr<Search> SearchPtr;

typedef std::unordered_set<std::unordered_set<Vid>*> SCC_Set;

class Search;

extern int qCount;
extern int qCount2;
extern SimpleClock profiler;
extern std::atomic<int> s;
extern std::atomic<int> s2;
namespace CellStatus{
    
    extern long complete[];
    extern Search* const COMPLETE_CELL;
    extern Search* const NEW_CELL;
    
    extern const char COMPLETE; // = 2;
    
    extern const char CONQUERED;// = 1;
    
    extern const char OCCUPIED; //= 0;
}

/*
struct CellStatus{
    
    std::weak_ptr<Search> owner;
    
    inline CellStatus(const std::shared_ptr<Search>& _owner){
        owner = _owner;
    }
    
    inline CellStatus(){
    }
    
    inline SearchPtr getOwner(){
        return owner.lock();
    }
    
    static const char COMPLETE; // = 2;
    
    static const char CONQUERED;// = 1;
    
    static const char OCCUPIED; //= 0;
    
    static  long        const dummyComplete[];
    static  CellStatus* const COMPLETE_CELL;
    static  CellStatus* const NEW_CELL;
};
*/







#endif /* typedefs_h */
