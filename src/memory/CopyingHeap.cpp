#include "CopyingHeap.h"
#include "CopyingCollector.h"

#if GC_TYPE==COPYING

#include "../vmobjects/AbstractObject.h"
#include "../vm/Universe.h"

CopyingHeap::CopyingHeap(long objectSpaceSize) : Heap(objectSpaceSize) {
    gc = new CopyingCollector(this);
    size_t bufSize = objectSpaceSize;
    currentBuffer = malloc(bufSize);
    oldBuffer = malloc(bufSize);
    memset(currentBuffer, 0x0, bufSize);
    memset(oldBuffer, 0x0, bufSize);
    currentBufferEnd = (void*)((size_t)currentBuffer + bufSize);
    collectionLimit = (void*)((size_t)currentBuffer + ((size_t)(bufSize *
                            0.9)));
    nextFreePosition = currentBuffer;
}

void CopyingHeap::switchBuffers() {
    size_t bufSize = (size_t)currentBufferEnd - (size_t)currentBuffer;
    void* tmp = oldBuffer;
    oldBuffer = currentBuffer;
    currentBuffer = tmp;
    currentBufferEnd = (void*)((size_t)currentBuffer + bufSize);
    nextFreePosition = currentBuffer;
    collectionLimit = (void*)((size_t)currentBuffer + (size_t)(0.9 *
                    bufSize));
}

AbstractVMObject* CopyingHeap::AllocateObject(size_t size) {
    pthread_mutex_lock(&allocationLock);
    AbstractVMObject* newObject = (AbstractVMObject*) nextFreePosition;
    nextFreePosition = (void*)((size_t)nextFreePosition + size);
    if (nextFreePosition > currentBufferEnd) {
        cout << "Failed to allocate " << size << " Bytes." << endl;
        GetUniverse()->Quit(-1);
    }
    //let's see if we have to trigger the GC
    if (nextFreePosition > collectionLimit)
    triggerGC();
    pthread_mutex_unlock(&allocationLock);
    return newObject;
}
#endif
