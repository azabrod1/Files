//
//  singleThreadedTarjan.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/28/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "singleThreadedTarjan.h"


SCC_Set* SingleThreadedTarjan::run(){

    std::cout << graph.size() <<std::endl;
    
    std::unordered_set<Vid>* verts = graph.getVerticies();
    
    for(auto& vertex: *verts){
        if(!lookup.count(vertex)){
            SingleCell* root = new SingleCell(vertex);
            lookup[vertex] = root;
            search(root);
        }
    }
    
    delete verts;
    return SCCs;
}



void SingleThreadedTarjan::search(SingleCell* root){
    SingleCell* curr, * child;
    conquer(root); //Add the root of the search to the stack
    
    while(!controlStack.empty()){
        
        curr = controlStack.top();
        
        //Curr might have more neighbors not yet assigned to an SCC
        if(!curr->unassignedNeighbors.empty()){
            child = curr->unassignedNeighbors.top();
            curr->unassignedNeighbors.pop(); //Remove that child from list of neighbors left to inspect
            
            if(child->onStack)
                curr->updateRank(child->index);
            
            else if(!child->assigned) //Not yet on stack nor assigned to an SCC
                conquer(child);
            
        }
        
        //curr has no more neighbors left to explore
        else{
            controlStack.pop();
            
            //Update rank of the node that linked to curr
            if(!controlStack.empty())
                controlStack.top()->updateRank(curr->rank);
            
            if(curr->rank == curr->index){
                std::unordered_set<Vid>* scc = new std::unordered_set<Vid>();
                SingleCell* cell = NULL;
                
                do{
                    cell = tarjanStack.top();
                    tarjanStack.pop();
                    cell->assigned = true; cell->onStack = false;
                    scc->insert(cell->vertex);
                }while(cell != curr);
                
                SCCs->insert(scc);
                
            }
            
        }
        
    }
}

//Delete all the nodes we created to assist our search
SingleThreadedTarjan::~SingleThreadedTarjan(){
    for(auto& entry : lookup)
        delete entry.second;
    
}

