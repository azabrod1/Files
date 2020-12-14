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

typedef unsigned int Vid; //Identifier for vertexes in graph

typedef std::unordered_set<std::unordered_set<Vid>*> SCC_Set;


#endif /* typedefs_h */
