//
//  Utility.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/27/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "utility.hpp"
#include <math.h>
#include <stdlib.h>

std::random_device rd;

std::mt19937 Utility::engine(rd()); //Select random number generator engine

std::uniform_real_distribution<double> Utility::dist(0,1);

bool Utility::initialized = false;

/**Generates random graph with cardinality verticies. For any two vertices v,w we create an edge from v to w with probabilty edgeProb*/

Graph<Vid>* Utility::generateRandomGraph(double edgeProb, const Vid& cardinality){
    
    Graph<Vid>* graph = new DirectedHashGraph<Vid>;
    
    for(Vid v1 = 0; v1 < cardinality; ++v1)
        for(Vid v2 = 0; v2 < cardinality; ++v2)
            if(dist(engine) < edgeProb)
                graph->insertEdge(v1, v2);
            
    
    return graph;
    
    
}


/** Many graphs exhibit "clustoring" of verticies. In other words, there are verticies that have many edges amongest each other
 * For example, consider a graph representing a social network. If Alice and Bob are friends and Bob and Olivia are friends, there is a relatively high probability that Alice and Olivia are friends. Most social networks use this realization to suggest connections to people one might know. This function generates a graph that exhibits this clustering property.
 *  We do this by assigning each vertex a "location" on the unit square. The closer (roughly euclidean distance) a vertex is to another, 
 *  the higher the probability it will have an edge into that vertex.
 *  Let verticies v1 and v2 be two verticies with X coodinates dx apart and Y coordinates dy apart. 
 *  We have that E = sqrt(dx^2+dy^2) is the Euclidean distance between the two points
 *  The function takes parameters 0<=Corr<=1 and edgeProb. A higher value for corr results in graphs with vertices that are more “clustered together”
 *   while Corr=0 results in the same function as the one above. The constant edgeProb is used to determine how dense the graph is.
 * 
 *  The probability that v1 will have an edge into v2 is:
 
 *   P(v1,v2) = edgeProb([(1- sqrt(E))* corr + (1-corr)]    =  edgeProb(1 - C*sqrt(E))
 *
 * It is easy to see how corr acts as a "weight" and edgeProb determines density. The extra square root is not a mistake, we want to penalize far away edges more than the
 * standard eucldean distance. Note that we treat negative probabilities as 0.
 
 @return A pointer to a graph with the properties above
*/

Graph<Vid>* Utility::GeoGenerateRandomGraph(double edgeProb, const double& corr, const Vid& cardinality){
    
    Graph<Vid>* graph = new DirectedHashGraph<Vid>;
    
    
    double* xCoordinates = new double[cardinality];
    double* yCoordinates = new double[cardinality];
    
    //Assign each coordinate a random location on the unit square ( 0<x<1, 0<y<1)
    for(int v = 0; v < cardinality; ++v){
        xCoordinates[v] = dist(engine); yCoordinates[v] = dist(engine);
    }
    
    double distance, dx, dy;
    
    
    for(Vid v1 = 0; v1 < cardinality; ++v1)
        for(Vid v2 = 0; v2 < cardinality; ++v2){
            
            if(v1 == v2) continue;
           
            dx = abs(xCoordinates[v1] - xCoordinates[v2]);    dy = abs(yCoordinates[v1] - yCoordinates[v2]);
            
            distance = sqrt(sqrt(dx*dx + dy*dy)); //Almost euclidean metric but we take to 1/4 power instead 1/4 to further "punish" far away values
            
            if(dist(engine) < edgeProb*(1-corr*distance))
                graph->insertEdge(v1, v2);
            
        }
    
    delete[] xCoordinates; delete[] yCoordinates;
    return graph;
}
