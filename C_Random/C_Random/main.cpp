//
//  main.cpp
//  C_Random
//
//  Created by Alex Zabrodskiy on 4/19/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <time.h>
#include <memory>
#include <random>
#include "Aqueue.h"

#include "tbb/critical_section.h"
#include "tbb/concurrent_unordered_map.h"
#include <tbb/concurrent_queue.h>

using namespace std;

const static int N = 150;

mutex mu;

std::atomic<int> accum(0);

struct A : enable_shared_from_this<A>{
    shared_ptr<int> ptr;
    int e;
    
};

struct B : enable_shared_from_this<B>{
    weak_ptr<int> ptr;
    int useless;
    short balls;
    //pointer to object, some refernece count, pointer to strong
};




tbb::concurrent_unordered_map<int, int> map(100);

int M = 100;

void func(int base){
    
    for(int i = 0; i < M; ++i)
        map[i] = i+1000;
    
}

//void func(_Args &&__args...){
    
//}

  //m.emplace(<#_Args &&__args...#>)

typedef tbb::tbb_allocator<std::pair<const int, int>> alloc;

int main(int argc, const char * argv[]) {
    
    // 2 bits = 4   2^6 = 64  2
    
    map[3] = 4;
    
    weak_ptr<int> weakX;
    
    {
    shared_ptr<int> x = make_shared<int>(6);
    shared_ptr<int> y = make_shared<int>(5);
    
  
    weakX = x;

    
    cout << *x << endl;
    
    }
    
    cout << "expired " << weakX.expired() <<endl;
    
    shared_ptr<int> xCopy = weakX.lock();
    
    cout << (xCopy == nullptr) <<endl;
    
    //Each search object gets pointer to a weak pointer of themself...
    
   // weak_ptr<int> cp = weakX;
   // cout << (weakX == cp) <<endl;
    
   // thats why use pointers instead...
    
   // atomic<shared_ptr<int>> ptr1;
    
    cout << sizeof(weak_ptr<int>) << "  " << sizeof(B) <<endl;
    
    struct Y{
        int n;
    };
    int x;
    
    cerr << sizeof(Y) << "    " << sizeof(int) << endl;
    
  //  cout << "lf " << std::atomic_is_lock_free(x) <<endl;

    
    
}


