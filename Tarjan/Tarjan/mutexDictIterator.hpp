//
//  mutexDictIterator.hpp
//  Tarjan
//
//  Created by Alex Zabrodskiy on 5/1/17.
//  Copyright © 2017 Alex Zabrodskiy. All rights reserved.
//

//#ifndef mutexDictIterator_hpp
//#define mutexDictIterator_hpp
//
//#include <stdio.h>
//#include "dictIterator.h"
//#include "mutexDict.h"
//
//
//template <class K, class V>
//class MutexIterator : public DictIterator<K,V> {
//    
//private:
//    std::__hash_map_const_iterator<std::__hash_const_iterator<std::__1::__hash_node<std::__1::__hash_value_type<int, int>, void *> *> >  myIt;
//    
//    
//public:
//    
//    MutexIterator(const MutexDict<K,V>& _dict) {
//        myIt = _dict.map.begin();
//    }
//    
//    virtual DictIterator<K,V>& operator++()  {
//        
//        ++myIt;
//        return *this;
//    }
//    
//    virtual bool operator==(const DictIterator<K,V>& that) {
//        
//        auto other = dynamic_cast<const MutexIterator<K,V>*>(&that);
//        
//        return (other->myIt == this->myIt);
//    }
//    
//    virtual bool operator!=(const DictIterator<K,V>& that){
//        auto other = dynamic_cast<const MutexIterator<K,V>*>(&that);
//        return this->myIt != other->myIt;
//        
//    }
//    
//    virtual const std::pair<K,V>& operator*() {
//        auto temp = &(*myIt);
//        return *temp;
//    }
//    
//};
//
//


//#endif /* mutexDictIterator_hpp */
