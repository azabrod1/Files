//
//  tarjan.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/27/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef tarjan_hpp
#define tarjan_hpp

#include <stdio.h>
#include <unordered_set>
#include "cell.h"
#include "typedefs.h"
#include "graph.h"
#include "singleThreadedTarjan.h"


class Tarjan{
    
public:
    static SCC_Set* singleThreadedTarjan(Graph<Vid> _graph){
        SingleThreadedTarjan algorithm(_graph);
        return algorithm.run();
    }
    
};



#endif /* tarjan_hpp */
