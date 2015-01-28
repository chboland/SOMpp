#pragma once

#include "../misc/defs.h"

#include "GarbageCollector.h"

class MarkSweepHeap;
class MarkSweepCollector : public GarbageCollector<MarkSweepHeap> {
public:
    MarkSweepCollector(MarkSweepHeap* heap) : GarbageCollector(heap) {}
    virtual void Collect();

private:
    void markReachableObjects();
};
