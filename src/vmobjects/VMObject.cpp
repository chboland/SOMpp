/*
 *
 *
 Copyright (c) 2007 Michael Haupt, Tobias Pape, Arne Bergmann
 Software Architecture Group, Hasso Plattner Institute, Potsdam, Germany
 http://www.hpi.uni-potsdam.de/swa/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include "VMObject.h"
#include "VMClass.h"
#include "VMSymbol.h"
#include "VMFrame.h"
#include "VMInvokable.h"

// clazz is the only field of VMObject so
const size_t VMObject::VMObjectNumberOfGcPtrFields = 0;

VMObject::VMObject(size_t numOfGcPtrFields) : AbstractVMObject(),
        numberOfGcPtrFields(numOfGcPtrFields + VMObjectNumberOfGcPtrFields),
        hash((intptr_t) this) {
    // Object size was already set by the heap on allocation
    initializeGcFields();
}

VMObject* VMObject::Clone(Page* page) {
    VMObject* clone = new (page, objectSize - sizeof(VMObject) ALLOC_MATURE) VMObject(*this);
    memcpy(SHIFTED_PTR(clone, sizeof(VMObject)),
           SHIFTED_PTR(this,  sizeof(VMObject)), GetObjectSize() - sizeof(VMObject));
    intptr_t* cloneHash = const_cast<intptr_t*>(&clone->hash);
    *cloneHash = hash;
    return clone;
}

void VMObject::SetClass(VMClass* cl) {
    store_ptr(clazz, cl);
}

VMSymbol* VMObject::GetFieldName(long index) {
    return GetClass()->GetInstanceFieldName(index);
}

void VMObject::Assert(bool value) const {
    GetUniverse()->Assert(value);
}

void VMObject::WalkObjects(walk_heap_fn walk, Page* page) {
    clazz = static_cast<GCClass*>(walk(clazz, page));

    size_t numFields = GetNumberOfFields();
    for (size_t i = 0; i < numFields; ++i) {
        FIELDS[i] = walk(FIELDS[i], page);
    }
}

void VMObject::MarkObjectAsInvalid() {
    clazz = (GCClass*) INVALID_GC_POINTER;
    
    size_t numFields = GetNumberOfFields();
    for (size_t i = 0; i < numFields; ++i) {
        FIELDS[i] = INVALID_GC_POINTER;
    }
}

StdString VMObject::AsDebugString() {
    if (this == load_ptr(nilObject)) {
        return "nilObject";
    } else if (this == load_ptr(trueObject)) {
        return "trueObject";
    } else if (this == load_ptr(falseObject)) {
        return "falseObject";
    }
    return "Object(" + GetClass()->GetName()->GetStdString() + ")";
}
