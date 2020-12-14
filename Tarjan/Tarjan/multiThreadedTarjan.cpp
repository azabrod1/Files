//
//  multiThreadedTarjan.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/30/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "multiThreadedTarjan.hpp"
#include "suspensionManager.hpp"
#include "tarjan.hpp"


MultiThreadedTarjan::~MultiThreadedTarjan(){
    
    //delete all the cell obejcts we constructed
    
    dict.deleteValues();
   
    //just implement delete keys instead 
}


SCC_Set* MultiThreadedTarjan::run()
{
    qCount = 0;
    SCC_Set* SCCs = new SCC_Set;
    
    
    std::vector<Worker> workers;
    std::vector<std::thread> threads;
    
    for(int ID = 0; ID < NUM_THREADS; ++ID)
        workers.emplace_back(ID, *this, graph, dict);
    
    for(Worker& worker: workers)
        threads.emplace_back(std::ref(worker));
    
    
    for(std::thread& thrd: threads )
        thrd.join();
    
    for(Worker& worker: workers)
        SCCs->insert(worker.SCCs.begin(), worker.SCCs.end());
    
   // for(Worker& worker: workers)
    //    worker.sc.printAccum(" profiler");
    
//    
//    SCC_Set* other = Tarjan::singleThreadedTarjan(graph);
//    
//    auto bad = Utility::bigDif(SCCs, other);
//    
//    std::cout << "bad size:  " << bad->size() << std::endl;
//    
//
//    std::vector<Cell<Vid>*>* cellV = dict.getValues();
//    
//    if(bad->size()){
//        
//        std::unordered_set<Cell<Vid>*> badCells;
//        
//        for(Cell<Vid>* cell: *cellV){
//            if(bad->count(cell->vertex))
//                badCells.insert(cell);
//            
//        }
//        
//        for(auto b: badCells){
//            std::cout << "tf: " << b->transferred <<std::endl;
//            std::cout << "root: " << b->root <<std::endl;
//            std::cout << "other: " << b->rec <<std::endl;
//            std::cout << "complete?: " << b->isComplete() <<std::endl;
//
//        }
//        
//        
//    }
//  assert(bad->size() == 0);
//    delete cellV; delete bad;  Utility::deleteSCCs(other);
    //std::cout << "cells " << s2 <<std::endl;
    
    return SCCs;
}



SearchPtr MultiThreadedTarjan::getSearch(Worker* worker){
    SearchPtr search;
    flags.store(0); //Can prob use relaxed mem order here
    bool updateFlagsASAP(true); //Should we update flags next loop iteration
    bool done(false);
    const long mask(worker->MASK);
    
    while(!done){

        //First, see if there is a pending Search we can resume
        search = pending.get();
        if(search)
            return search;
        
        //Otherwise, try to start new search by taking a cell off the Stealing queue
        Cell<Vid>* const root(cellQueue.next(worker->ID));
        
        /* We try to find a potential root cell on the stealing queue.
         * we need to try to conquer it before starting the search.
         * Otherwise, we could be starting a search on a cell already
         * in another search's stack! If the conquer suceeds, we can start the search
         * otherwise we continue this loop
         */
        
        
        if(root)
            if(root->conquerOrFail(worker->spareSearch.get())){ //Found a viable root for the new search
                worker->initNeighbors(root);
                search = worker->spareSearch;
                search->setRoot(root);
                worker->spareSearch = std::make_shared<Search>(); //Allocate a new spare search
                return search;
            }
         
        
        if(updateFlagsASAP)
            flags|=mask;
        
        //Update flags next iteration if they were not updated this time and our flag is not set
        updateFlagsASAP = (!updateFlagsASAP && ((flags.load() & mask) == 0));
        
        done = (flags.load() == ALL_FLAGS_SET);
        
        if(!done){ //Make more sophisticated later!
            //std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        }
        
    }
    
    return nullptr;
    
}
