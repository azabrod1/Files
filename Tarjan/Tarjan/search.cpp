//
//  search.cpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 4/26/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include "search.hpp"
#include "multiThreadedTarjan.hpp"
#include <algorithm>    // std::max

                              /*******************   Constructor and Destructor   ************/


Search::Search() : cellCount(0){
    ++s;
    this->cellBlockedOn    = nullptr;
}

Search::~Search(){
    --s;    

    assert(!deleted);
    deleted = true;
   // delete STACK_STAMP;
}

void Search::setRoot(Cell<Vid>* root){
   // root->setOwner(this->STACK_STAMP);
    pushToStacks(root);
    root->root = true;
}

/**
 Pre:  The cell must be owned by the thread
 Post: The cell is added to both the search's tarjan and control stack.
       Additionally, the cell is given an index and we increment cellCount
 
 @param newCell Pointer to the cell we are pushing to our stacks
 */
void Search::pushToStacks(Cell<Vid>* newCell){
    tarjanStack.push_back(newCell); controlStack.push_back(newCell);
    newCell->initIndex(cellCount++);
}

void Search::refreshCellCount(){
    cellCount = tarjanStack.back()->index + 1;
}


/**
 Pre: The cell must be owned by the thread 
 Post: The cell's queue of neighboring cells is created. Each vertex that is a neighbor
      of the vertex represented by parameter cell will have a unique corresponding
      cell object after this call exists
 
 @param cell Pointer to the cell whose neighbors we are identifying
 
void Search::initNeighbors(Cell<Vid>* cell){

    //get set of all the vertex's neighbors
    const std::unordered_set<Vid>& neighbors = graph.getNeighbors(cell->vertex);
        
    std::vector<Cell<Vid>*>* succs = new std::vector<Cell<Vid>*>();
    
    //create cell objects for the successors of vertex that have not yet been seen
    for(auto& succ: neighbors){
        
        spare->vertex = succ; //Set vertex
        auto status = dict.put(succ, spare);
        
        //We used up the cell object to store the neighbor so we need a new one
        //for next time
        if(status.second)
            spare = new Cell<Vid>();
        
        Cell<Vid>* neighbor = status.first;
        
        if(!neighbor->isComplete()) //If the neighbor is complete, no search has to visit it again
            succs->push_back(neighbor);
        
    }
    
    cell->setNeighbors(succs);
}
*/

/*
void Search::buildSCC(Cell<Vid>* const head, SCC_Set* const SCCs){

    std::unordered_set<Vid>* scc(new std::unordered_set<Vid>());
    Cell<Vid>* curr;

    do{
        curr = tarjanStack.back();
        tarjanStack.pop_back();
        curr->markComplete(); //need to unpause searches blocked on the cell AND delete cell.blockedON
        scheduler.resumeAllBlockedOn(curr);
        
        scc->insert(curr->vertex);
    }while(curr != head);
    
    SCCs->insert(scc);
    
}
*/
                        /************* Methods for node transfer ***************/



        /*Transfer cells from source to destination */



/**
 Transfer Cells

 @param src Search from which cells are being transfered
 @param dest The Search 'S', the destination of the cells being transfered
 @param conflictCell The cell from src blocking the previous search
 */
void Search::transferCells(const SearchPtr& src, const SearchPtr& dest, Cell<Vid>*& conflictCell, std::vector<SearchPtr>& blockedSearches){
    
    src->trans =  src->trans == 1? 3:0;
    if(src->controlStackEmpty())
        src->trans += 1000;
    dest->trans = dest->trans == 0? 4:1;

    TarjanStack*  srcTarjanStack   = &src->tarjanStack;
    ControlStack* srcControlStack  = &src->controlStack;
        
    //We use a reverse iterator since we are starting from the top of the stack
    auto reverseIt = srcTarjanStack->rbegin();
    
    //Find Li, the furtherest element in Si that is definitely a member of the partial SCC we are transferring
    Cell<Vid>* next = *reverseIt; Vid minRank(next->rank);
    bool reachedCC(next == conflictCell);
    assert(conflictCell->getOwner() == src);
    while(!reachedCC || next->index > minRank){
        next = *(++reverseIt); //Increment iterator and get the next element
        minRank = std::min(minRank, next->rank);
        if(next == conflictCell) reachedCC = true;
    }
    
    //Transfer the relevent cells from src's tarjan stack to dest's tarjan stack
    
    Cell<Vid>* const last(next);
    
    Vid delta(dest->cellCount - last->index);
    
    dest->tarjanStack.insert(dest->tarjanStack.end(), (reverseIt+1).base(), srcTarjanStack->end());
    
    for(auto it = (reverseIt+1).base(); it != srcTarjanStack->end(); ++it)
        (*it)->transfer(delta, dest.get(), blockedSearches);
    
    srcTarjanStack->erase((reverseIt+1).base(), srcTarjanStack->end());
    
    auto rit = srcControlStack->rbegin();
    
    while(*rit != last) ++rit;
    
    //Transfer cells from src control stack to destination control stack
    
    dest->controlStack.insert(dest->controlStack.end(), (rit+1).base(), srcControlStack->end());
    
    srcControlStack->erase((rit+1).base(), srcControlStack->end());
    
    bool searchDone = srcTarjanStack->empty(); //if the tarjan stack empty, the search should terminate
    
    //Find the cell the search was previously blocked on. The next time this function is called, it will be called
    //with this value for conflictCell to make sure the next search (the one blocking src) has the conflict cell
    //transfered to dest
    
    
    conflictCell = src->cellBlockedOn;
    conflictCell->unblockSearch(src.get());
    
    //Src now blocks on the deepest cell that we transfered to dest
    if(!searchDone){
        src->setCellBlockedOn(last);                       //TODO: relax mem order
        last->blockSearch(src);
    }
    dest->refreshCellCount();
}

void Search::lockFreeTransfer(const SearchPtr& src, const SearchPtr& dest, Cell<Vid>* conflictCell){
    
    TarjanStack&  srcTarjanStack   = src->tarjanStack;
    ControlStack& srcControlStack  = src->controlStack;
    
    //We use a reverse iterator since we are starting from the top of the stack
    auto reverseIt = srcTarjanStack.rbegin();
    
    Cell<Vid>* next = *reverseIt; Vid minRank(next->rank);
    bool reachedCC(next == conflictCell);
    assert(conflictCell->getOwner() == src);
    while(!reachedCC || next->index > minRank){
        next = *(++reverseIt); //Increment iterator and get the next element
        minRank = std::min(minRank, next->rank);
        if(next == conflictCell) reachedCC = true;
    }
    
    //Transfer the relevent cells from src's tarjan stack to dest's tarjan stack
    
    Cell<Vid>* const last(next);
    
    Vid delta(dest->cellCount - last->index);
    
    dest->tarjanStack.insert(dest->tarjanStack.end(), (reverseIt+1).base(), srcTarjanStack.end());
    
    for(auto it = (reverseIt+1).base(); it != srcTarjanStack.end(); ++it)
        (*it)->transfer(delta, dest.get());
    
    //Erase the cells transferred to dest from src's tarjan stack 
    srcTarjanStack.erase((reverseIt+1).base(), srcTarjanStack.end());
    
    auto rit = srcControlStack.rbegin();
    
    while(*rit != last) ++rit;
    
    //Transfer cells from src control stack to destination control stack
    
    dest->controlStack.insert(dest->controlStack.end(), (rit+1).base(), srcControlStack.end());
    
    srcControlStack.erase((rit+1).base(), srcControlStack.end());
    
    //Find the cell the search was previously blocked on. The next time this function is called, it will be called
    //with this value for conflictCell to make sure the next search (the one blocking src) has the conflict cell
    //transfered to dest
    
    dest->refreshCellCount();
    
    //((src->cellBlockedOn).load())->unblockSearch(src.get());
    
    //Src now blocks on the deepest cell in the tarjan stack that we transfered to dest
    if(!srcTarjanStack.empty()){
        src->setCellBlockedOn(last);                       //TODO: relax mem order
        last->blockSearch(src);
    }
    else
        src->removeCellBlockedOn();
    
}

//TODO STRONG POINTERS TOO MUCH REF


///** simple test function for the cell transfer method(s) */
//void Search::testCellTransfer(const Graph<Vid>& _graph, Dictionary<Vid, Cell<Vid>*>& _dict){
//    
//    MultiThreadedTarjan dummy(_graph, _dict, 1, *new LockedPendingQueue, *new UnrootedStealingQueue(0,0,_dict,1));
//    
//    
//    Search s1, s2;
//    
//    //Search 1 tarjan stack
//    //Cell<Vid> b0(0), b1(1), b2(2), l0(3), m1(4), m2(5), m3(6), m4(7), n1(8), m5(9), m6(10), t1(11);
//    
//    //Search 1 control stack b0(0), b1(1), b2(2),.... l0(3) ...., m1(4), ......n1(8), ......m5(9), t1(11)
//    
//    Cell<Vid> s1Cells[12];
//    
//    for(int c = 0; c <= 11; ++c){
//        s1Cells[c].conquer(&s1);
//        s1Cells[c].index = c;
//        s1Cells[c].rank = c;
//        s1.tarjanStack.push_back(s1Cells + c);
//    }
//    
//    for(int c : {0,1,2,3,4,8,9,10})
//        s1.controlStack.push_back(s1Cells+c);
//    
//    for(int c : {5,6,7}) s1Cells[c].rank = 4;
//    
//    s1Cells[10].rank = 3;
//    s1Cells[11].rank = 9;
//
//
//    //Search 2 tarjan stack
//    // before...  0,1,2,3,4,5,6  ....   7 ... 8,9,10,11,12,13...14...15,16,17
//    
//    //control stack    {0,5,6,7,8,13,14,15,17}
//    
//    Cell<Vid> s2Cells[18];
//    
//    for(int c = 0; c <= 17; ++c){
//        s2Cells[c].conquer(&s2);
//        s2Cells[c].index = c;
//        s2.tarjanStack.push_back(s2Cells + c);
//        s2Cells[c].rank = c;
//
//    }
//    
//    for(int c : {0,5,6,7,8,13,14,15,17})
//        s2.controlStack.push_back(s2Cells+c);
//    
//    for(int c : {0,1,2,3,4}) s2Cells[c].rank = 0;
//
//    s2Cells[15].rank = 7;
//    s2Cells[17].rank = 13;
//    
//    s1.cellCount = 12;    s2.cellCount = 18;
//
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks before\n";
//    s2.printStacks();
//    
//    s1.cellBlockedOn = s2Cells + 7;
//    s2.cellBlockedOn = s1Cells + 8;
//
//    //s1.searchBlockedOn = &s2;
//    //s2.searchBlockedOn = &s1;
//    
//    std::vector<Search*> blockedSearches;
//    
//    Cell<Vid>* blockingCell = s2.cellBlockedOn;
//    
//    Search::transferCells(&s1, &s2, blockingCell, blockedSearches);
//
//    
//    
//    std::cout <<"\n**************\ns1 stacks after\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks after\n";
//    s2.printStacks();
//    
//}
//
///** Test function for suspension, cycle detection and cycle resolution */
//void Search::testCellTransfer2(){
//    
//    Graph<Vid>* dummy = new DirectedHashGraph<Vid>;
//    
//    MutexDict<Vid, Cell<Vid>*> dict;
//    LockedPendingQueue q;
//    
//    MultiThreadedTarjan sample(*dummy, dict, 1, q, *new UnrootedStealingQueue(0,0, dict,1));
//    
//    //The "loop"
//    Search s1,s2,s3;
//    
//    // Auxilary Searches that will be blocked on s1..s3
//    Search s4,s5,s6,s7;
//
//    
//    //Search 1: blocked on S2 cell 7. l0 = 0. n1 = 8, suspended @ 11
//    
//    Cell<Vid> s1Cells[14], s2Cells[19], s3Cells[13];
//    
//    for(int c = 0; c < 14; ++c){
//        s1Cells[c].conquer(&s1);
//        s1Cells[c].index = c;
//        s1Cells[c].rank = c;
//        s1.tarjanStack.push_back(s1Cells + c);
//    }
//    
//    for(int c : {0,1,2,3,4,8,9,11})
//        s1.controlStack.push_back(s1Cells+c);
//    
//    s1Cells[5].rank = 0;
//    s1Cells[9].rank = 4;
//    s1Cells[13].rank = 9;
//    s1Cells[6].rank = 3;
//
//    s1.cellCount    = 14; s2.cellCount    = 19; s3.cellCount = 15;
//
//    //Search 2: blocked on s3 cell 8. l0 = 5, n1 = 7. suspended @ 15
//    
//    
//    for(int c = 0; c < 19; ++c){
//        s2Cells[c].conquer(&s2);
//        s2Cells[c].index = c;
//        s2Cells[c].rank = c;
//        s2.tarjanStack.push_back(s2Cells + c);
//    }
//    
//    for(int c : {0,5,6,7,8,13,14,15})
//        s2.controlStack.push_back(s2Cells+c);
//    
//    for(int c : {0,1,2,3,4}) s2Cells[c].rank = 0;
//    
//    s2Cells[7].rank = 5;
//    s2Cells[15].rank = 7;
//    s2Cells[12].rank = 7;
//    s2Cells[17].rank = 13;
//    
//    //Search 3: wants to suspend on s1 after cell 10. lo = 1, n1 = 8
//    
//    for(int c = 0; c < 13; ++c){
//        s3Cells[c].conquer(&s3);
//        s3Cells[c].index = c;
//        s3Cells[c].rank = c;
//        s3.tarjanStack.push_back(s3Cells + c);
//    }
//    
//    for(int c : {0,1,5,6,7,8,9,10})
//        s3.controlStack.push_back(s3Cells+c);
//    
//    s3Cells[8].rank = 5;
//    s3Cells[6].rank = 1;
//    
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks before\n";
//    s2.printStacks();
//    std::cout <<"s3 stacks before\n";
//    s3.printStacks();
//    
//    assert(s1Cells[8].conquer(&s4) == CellStatus::OCCUPIED);
//    sample.suspend(&s4, s1Cells+8);
//    
//    assert(s2Cells[14].conquer(&s5) == CellStatus::OCCUPIED);
//    sample.suspend(&s5, s2Cells+14);
//    
//    
//    //First S2 suspends on S3...
//    assert(s3Cells[8].conquer(&s2) == CellStatus::OCCUPIED);
//    sample.suspend(&s2, s3Cells+8);
//    
//    //Now S1 suspends on S2
//
//    assert(s2Cells[7].conquer(&s1) == CellStatus::OCCUPIED);
//    sample.suspend(&s1, s2Cells+7);
//    
//    //Now S3 suspends on S1 to form the cycle
//
//    assert(s1Cells[8].conquer(&s3) == CellStatus::OCCUPIED);
//    sample.suspend(&s3, s1Cells+8);
//    
//    //Did the searches get properly redirected??
//    assert(s4.getBlockingSearch() == &s3);
//    //Did the search get properly redirected??
//    assert(s5.getBlockingSearch() == &s3);
//    
//    assert(s3.tarjanStackTop()->rank == s3Cells[8].index);
//    
//    std::cout <<"******************stacks after! *******************\n" <<std::endl;
//    
//    std::cout <<"s1 stacks after\n";
//    s1.printStacks();
//    std::cout <<"s2 stacks after\n";
//    s2.printStacks();
//    std::cout <<"s3 stacks after\n";
//    s3.printStacks();
//    
//    
//}
//
///** Test function for suspension, cycle detection and cycle resolution */
//void Search::testCellTransfer3(){
//    
//    Graph<Vid>* dummy = new DirectedHashGraph<Vid>;
//    
//    MutexDict<Vid, Cell<Vid>*> dict;
//    LockedPendingQueue q;
//    
//    MultiThreadedTarjan sample(*dummy, dict, 1, q, *new UnrootedStealingQueue(0,0, dict,1));
//    
//    
//    //The "loop"
//    Search s1,s2,s3;
//    
//    // Auxilary Searches that will be blocked on s1..s3
//    Search s4,s5,s6,s7;
//    
//    
//    //Search 1: blocked on S2 cell 7. l0 = 0. n1 = 8, suspended @ 11
//    s1.cellCount = 1; s3.cellCount = 18;
//    
//    
//    
//    Cell<Vid> s1Cells[1], s3Cells[19];
//    
//    
//    s1Cells[0].conquer(&s1);
//    s1Cells[0].index = 0;
//    s1Cells[0].rank = 0;
//    s1.tarjanStack.push_back(s1Cells);
//    s1.controlStack.push_back(s1Cells);
//
//
//       //Search 3: wants to suspend on s1 after cell 10. lo = 1, n1 = 8
//    
//    for(int c = 0; c < 13; ++c){
//        s3Cells[c].conquer(&s3);
//        s3Cells[c].index = c;
//        s3Cells[c].rank = c;
//        s3.tarjanStack.push_back(s3Cells + c);
//    }
//    
//    for(int c : {0,1,5,6,7,8,9,10})
//        s3.controlStack.push_back(s3Cells+c);
//    
//    s3Cells[8].rank = 5;
//    s3Cells[6].rank = 0;
//    
//    
//    std::cout <<"s1 stacks before\n";
//    s1.printStacks();
//  
//    std::cout <<"s3 stacks before\n";
//    s3.printStacks();
//    
//
//    
//    //First S3 suspends on S1...
//    assert(s1Cells[0].conquer(&s3) == CellStatus::OCCUPIED);
//    sample.suspend(&s3, s1Cells);
//    
//    //Now S1 suspends on S3
//    
//    assert(s3Cells[7].conquer(&s1) == CellStatus::OCCUPIED);
//       sample.suspend(&s1,s3Cells + 7);
//   
//    
//    //assert(s3.tarjanStackTop()->rank == s3Cells[8].index);
//    
//    std::cout <<"******************stacks after! *******************\n" <<std::endl;
//    
//    std::cout <<"s1 stacks after\n";
//    s1.printStacks();
//   
//    std::cout <<"s3 stacks after\n";
//    s3.printStacks();
//    
//    
//}




void Search::printStacks(){
    
    std::cout<< "Tarjan stack " <<std::endl;
    
    for(Cell<Vid>* cell: tarjanStack)
        std::cout << cell->index << " ";
    
    std::cout<< std::endl;
    
    std::cout<< "control stack " <<std::endl;
    
    for(Cell<Vid>* cell: controlStack)
        std::cout << cell->index << " ";
    
    std::cout << "\ncell count: " << this->cellCount;
    
    std::cout<< std::endl;
    
    
}




