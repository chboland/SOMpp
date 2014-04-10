#pragma once

#include "../misc/defs.h"
#if GC_TYPE == MARK_SWEEP

#include "PagedHeap.h"

class MarkSweepHeap : public Heap {
    friend class MarkSweepCollector;
public:
    MarkSweepHeap(long objectSpaceSize = 1048576);
    AbstractVMObject* AllocateObject(size_t size);
private:
    vector<VMOBJECT_PTR>* allocatedObjects;
    size_t spcAlloc;
    long collectionLimit;

};

#endif
