#pragma once
#ifndef VMMETHOD_H_
#define VMMETHOD_H_

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

#include "VMInvokable.h"
#include "VMInteger.h"

class VMArray;
class VMObject;
class VMInteger;
//class MethodGenerationContext;
class VMFrame;

class Interpreter;

class VMMethod: public VMInvokable {
    friend class Interpreter;

public:
    VMMethod(long bcCount, long numberOfConstants, long nof = 0);

    inline  long      GetNumberOfLocals() /*const*/;
            void      SetNumberOfLocals(long nol);
            long      GetMaximumNumberOfStackElements() const;
            void      SetMaximumNumberOfStackElements(long stel);
    inline  long      GetNumberOfArguments() /*const*/;
            void      SetNumberOfArguments(long);
            long      GetNumberOfBytecodes() const;
            void      SetHolderAll(pVMClass hld);
            pVMObject GetConstant(long indx) /*const*/;
    inline  uint8_t   GetBytecode(long indx) const;
    inline  void      SetBytecode(long indx, uint8_t);
    
#ifdef UNSAFE_FRAME_OPTIMIZATION
    void SetCachedFrame(pVMFrame frame);
    pVMFrame GetCachedFrame() const;
#endif
    
#if GC_TYPE==PAUSELESS
    virtual pVMMethod Clone(Page*);
    virtual void MarkReferences();
#else
    virtual pVMMethod Clone();
    virtual void WalkObjects(VMOBJECT_PTR (VMOBJECT_PTR));
#endif
    
    inline  long      GetNumberOfIndexableFields() /*const*/;

    inline  void      SetIndexableField(long idx, pVMObject item);

    //-----------VMInvokable-------------//
    //operator "()" to invoke the method
    virtual void operator()(pVMFrame frame);

    void SetSignature(pVMSymbol sig);

private:
    inline uint8_t* GetBytecodes() const;
    inline pVMObject GetIndexableField(long idx) /*const*/;

    pVMInteger numberOfLocals;
    pVMInteger maximumNumberOfStackElements;
    pVMInteger bcLength;
    pVMInteger numberOfArguments;
    pVMInteger numberOfConstants;
    
#ifdef UNSAFE_FRAME_OPTIMIZATION
    pVMFrame cachedFrame;
#endif
    
    pVMObject* indexableFields;
    uint8_t* bytecodes;
    static const long VMMethodNumberOfFields;
};

inline long VMMethod::GetNumberOfLocals() /*const*/ {
#ifdef USE_TAGGING
    return UNTAG_INTEGER(numberOfLocals);
#else
    PG_HEAP(ReadBarrier((void**)(&numberOfLocals)));
    return numberOfLocals->GetEmbeddedInteger();
#endif
}

long VMMethod::GetNumberOfIndexableFields() /*const*/ {
    //cannot be done using GetAdditionalSpaceConsumption,
    //as bytecodes need space, too, and there might be padding
#ifdef USE_TAGGING
    return UNTAG_INTEGER(this->numberOfConstants);
#else
    PG_HEAP(ReadBarrier((void**)(&this->numberOfConstants)));
    return this->numberOfConstants->GetEmbeddedInteger();
#endif
}

uint8_t* VMMethod::GetBytecodes() const {
    return bytecodes;
}

inline long VMMethod::GetNumberOfArguments() /*const*/ {
#ifdef USE_TAGGING
    return UNTAG_INTEGER(numberOfArguments);
#else
    PG_HEAP(ReadBarrier((void**)(&numberOfArguments)));
    return numberOfArguments->GetEmbeddedInteger();
#endif
}

pVMObject VMMethod::GetIndexableField(long idx) /*const*/ {
    PG_HEAP(ReadBarrier((void**)(&indexableFields[idx])));
    return indexableFields[idx];
}

void VMMethod::SetIndexableField(long idx, pVMObject item) {
    indexableFields[idx] = item;
#if GC_TYPE==GENERATIONAL
    _HEAP->WriteBarrier(this, item);
#endif
}

uint8_t VMMethod::GetBytecode(long indx) const {
    return bytecodes[indx];
}

void VMMethod::SetBytecode(long indx, uint8_t val) {
    bytecodes[indx] = val;
}

#endif
