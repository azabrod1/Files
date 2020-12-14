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

enum CellState {Occupied, Finished};
class Search;

template <class V>
struct Cell{
    
    CellState status;
    V     vertex;
    unsigned int index;
    unsigned int rank;
    Search* search = NULL; //What search does the cell belong to?
    
    //prev??

    
};


#endif /* node_h */
