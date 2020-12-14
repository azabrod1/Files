//
//  SimpleSharded.h
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/18/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

#ifndef SimpleSharded_h
#define SimpleSharded_h


#include "dictionary.h"
#include <mutex>
#include <unordered_map>
/**Implements the sychronized dictionary class with sychronized blocks (a mutex)
 This implementation is likely to be much slower than a lock free one */


template <class K, class V>
class ShardedMap : public Dictionary<K, V> {
    
private:
    const static int BITS = 8;
    const static int LOCKS = 2 << BITS;
    std::mutex mu[LOCKS];
    std::unordered_map<K, V> map[LOCKS];
    
public:
    
    //Inserts a value into the hashtable if the key did no already exist in the dictionary.
    //Returns true if and only if the insertion is successful
    std::pair<V,bool> put(const K& key,const V& value){
        
        K key1 = key & (LOCKS - 1);
        K key2 = key >> BITS;
        
        std::pair<std::__hash_map_iterator<std::__hash_iterator<std::__1::__hash_node<std::__1::__hash_value_type<unsigned int, Cell<unsigned int> *>, void *> *> >, bool> val;
        
        {
        std::lock_guard<std::mutex> lock(mu[key1]);
        
        val =  map[key1].insert(std::make_pair(key2, value));
            
        }
        
        return std::make_pair((val.first)->second, val.second);
        
    }
    
    
    V&    get(const K& key) {
        
        K key1 = key & (LOCKS - 1);
        K key2 = key >> BITS;
        
        std::lock_guard<std::mutex> lock(mu[key1]);
        return map[key1].at(key2);
        
    }
    
    //Same as get
    V&    operator[](const K& key){
        K key1 = key & (LOCKS - 1);
        K key2 = key >> BITS;
        
        std::lock_guard<std::mutex> lock(mu[key1]);
        
        return map[key1].at(key2);
    }
    
    unsigned long size(){
        
        unsigned long size(0);
        
        for(int m = 0; m < LOCKS; ++ m)
            size += map[m].size();
        
        return size;
        
    }
    
    bool contains(const K& key){
        K key1 = key & (LOCKS - 1);
        K key2 = key >> BITS;
        
        std::lock_guard<std::mutex> lock(mu[key1]);
        return map[key1].count(key2) != 0;
    }
    
    std::vector<K>* getKeys(){
        
        std::vector<K>* toReturn = new std::vector<K>();
        toReturn->reserve(size());
        
        std::unordered_map<K, V>* curr;
        
        for(int m = 0; m < LOCKS; ++m){
            curr = map + m;
            
            for(auto& pair: *curr)
                toReturn->push_back(pair.first);
            
        }
 
        
        return toReturn;
    }
    
    std::vector<V>* getValues(){
        
        std::vector<V>* toReturn = new std::vector<V>();
        toReturn->reserve(size());
        
        std::unordered_map<K, V>* curr;

        for(int m = 0; m < LOCKS; ++m){
            curr = map + m;
            
            for(auto& pair: *curr)
                toReturn->push_back(pair.second);
            
            
        }        
        return toReturn;
    }
    
    void deleteValues(){
        
        std::unordered_map<K, V>* curr;
        for(int m = 0; m < LOCKS; ++m){
            curr = map + m;
            
            for(auto& pair: *curr)
                delete pair.second;
            
        }
    }
    
    
    
};





#endif /* SimpleSharded_h */
