#pragma once

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

#include <iostream>

#include "VMString.h"
#include "VMObject.h"

class VMInteger;
class VMInvokable;

class VMSymbol: public VMString {

public:
    typedef GCSymbol Stored;
    
    VMSymbol(const char* str);
    VMSymbol(const StdString& s);
    virtual StdString GetPlainString() const;
    virtual size_t GetObjectSize() const;
    virtual VMClass* GetClass();
    
    virtual void MarkObjectAsInvalid();
    
#if GC_TYPE==PAUSELESS
    virtual VMSymbol* Clone(Interpreter*);
    virtual VMSymbol* Clone(PauselessCollectorThread*);
    //virtual void MarkReferences();
#else
    virtual VMSymbol* Clone();
    //virtual void WalkObjects(VMOBJECT_PTR (VMOBJECT_PTR));
#endif
    
    
private:
//    const VMClass* cachedClass_invokable[3];
//    long nextCachePos;
//    VMInvokable* cachedInvokable[3];
//    inline VMInvokable* GetCachedInvokable(const VMClass*) const;
//    inline void UpdateCachedInvokable(const VMClass* cls, VMInvokable* invo);
    
    friend class VMClass;
};

//VMInvokable* VMSymbol::GetCachedInvokable(const VMClass* cls) const {
//    if (cls == cachedClass_invokable[0])
//        return cachedInvokable[0];
//    else if (cls == cachedClass_invokable[1])
//        return cachedInvokable[1];
//    else if (cls == cachedClass_invokable[2])
//        return cachedInvokable[2];
//    return NULL;
//}

//void VMSymbol::UpdateCachedInvokable(const VMClass* cls, VMInvokable* invo) {
//    cachedInvokable[nextCachePos] = invo;
//    cachedClass_invokable[nextCachePos] = cls;
//    nextCachePos = (nextCachePos + 1) % 3;
//    
//#if GC_TYPE==GENERATIONAL
//    _HEAP->WriteBarrier(this, (VMOBJECT_PTR) invo);
//    _HEAP->WriteBarrier(this, (VMOBJECT_PTR) cls);
//#endif
//}

