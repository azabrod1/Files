//
//  neighborQueue.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/27/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef neighborQueue_h
#define neighborQueue_h

#include <vector>

template <class V> struct Cell;

//template <class V>
class NeighborQueue {
    
private:
    std::vector<Cell<long>*> unexplored; //Cells yet to be explored
    
public:
    
    Cell<long>* getUnexplored(){
        
        /**Try to select a neighbor that is not currently being explored by another search*/
        for(auto& vertex : unexplored){
            
           //vertex->add();
            
            
            
        }
        
        
        
        
        
        return 0;
    }
    
    inline bool isEmpty(){
        return unexplored.empty();
    }
    
    
    

};





#endif /* neighborQueue_h */
