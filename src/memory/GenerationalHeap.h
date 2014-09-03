#pragma once

#include "../misc/defs.h"
#include <assert.h>


#include "Heap.h"
#include "../vmobjects/VMObjectBase.h"

#include <vm/Universe.h>

#ifdef UNITTESTS
struct VMObjectCompare {
    bool operator() (pair<const oop_t, const oop_t> lhs, pair<const
            oop_t, const oop_t> rhs) const
    {   return (size_t)lhs.first<(size_t)rhs.first &&
        (size_t)lhs.second<(size_t)rhs.second;}
};
#endif

class GenerationalHeap : public Heap<GenerationalHeap> {
    friend class GenerationalCollector;
public:
    GenerationalHeap(long objectSpaceSize = 1048576);
    AbstractVMObject* AllocateNurseryObject(size_t size);
    AbstractVMObject* AllocateMatureObject(size_t size);
    size_t GetMaxNurseryObjectSize();
    void writeBarrier(oop_t holder, const oop_t referencedObject);
    inline bool isObjectInNursery(oop_t obj);
#ifdef UNITTESTS
    std::set<pair<const oop_t, const oop_t>, VMObjectCompare > writeBarrierCalledOn;
#endif
private:
    void* nursery;
    size_t nursery_end;
    size_t nurserySize;
    size_t maxNurseryObjSize;
    size_t matureObjectsSize;
    void* nextFreePosition;
    void writeBarrier_OldHolder(oop_t holder, const oop_t
            referencedObject);
    void* collectionLimit;
    vector<size_t>* oldObjsWithRefToYoungObjs;
    vector<oop_t>* allocatedObjects;
};

inline bool GenerationalHeap::isObjectInNursery(oop_t obj) {
    assert(Universe::IsValidObject(obj));
    
    return (size_t) obj >= (size_t)nursery && (size_t) obj < nursery_end;
}

inline size_t GenerationalHeap::GetMaxNurseryObjectSize() {
    return maxNurseryObjSize;
}

inline void GenerationalHeap::writeBarrier(oop_t holder, const oop_t referencedObject) {
#ifdef UNITTESTS
    writeBarrierCalledOn.insert(make_pair(holder, referencedObject));
#endif
    
    assert(Universe::IsValidObject(referencedObject));
    assert(Universe::IsValidObject(holder));

    size_t gcfield = *(((size_t*)holder)+1);
    if ((gcfield & 6 /* MASK_OBJECT_IS_OLD + MASK_SEEN_BY_WRITE_BARRIER */) == 2 /* MASK_OBJECT_IS_OLD */)
        writeBarrier_OldHolder(holder, referencedObject);
}
