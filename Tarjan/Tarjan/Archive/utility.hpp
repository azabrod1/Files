//
//  Utility.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/27/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef utility_hpp
#define utility_hpp

#include <stdio.h>
#include "directedHashGraph.h"
#include "cell.h"
#include <random>       // for pseudo-random number generators and distributions


class Utility{
    
public:
    
    static bool initialized;
    static Graph<Vid>* generateRandomGraph(double edgeProb, const Vid& size);
    static Graph<Vid>* GeoGenerateRandomGraph(double edgeProb, const double& corr, const Vid& size);
    
    static uniform_real_distribution<double> dist;
    static std::mt19937 engine;
    
    
};



#endif /* utility_hpp */
