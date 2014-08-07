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

#include "VMPrimitive.h"
#include "VMSymbol.h"
#include "VMClass.h"

#include "../interpreter/Interpreter.h"
#include "../vm/Universe.h"

//needed to instanciate the Routine object for the  empty routine
#include "../primitivesCore/Routine.h"

pVMPrimitive VMPrimitive::GetEmptyPrimitive( pVMSymbol sig ) {
#if GC_TYPE==GENERATIONAL
    pVMPrimitive prim = new (_HEAP, _PAGE) VMPrimitive(sig);
#elif GC_TYPE==PAUSELESS
    pVMPrimitive prim = new (_PAGE) VMPrimitive(sig);
#else
    pVMPrimitive prim = new (_HEAP) VMPrimitive(sig);
#endif
    prim->empty = true;
    prim->SetRoutine(new Routine<VMPrimitive>(prim, &VMPrimitive::EmptyRoutine));
    return prim;
}

const int VMPrimitive::VMPrimitiveNumberOfFields = 2;

VMPrimitive::VMPrimitive(pVMSymbol signature) : VMInvokable(VMPrimitiveNumberOfFields) {
    //the only class that explicitly does this.
    PG_HEAP(ReadBarrier((void**)(&primitiveClass)));
    this->SetClass(primitiveClass);

    this->SetSignature(signature);
    this->routine = NULL;
    this->empty = false;
}

pVMPrimitive VMPrimitive::Clone() /*const*/ {
    pVMPrimitive prim;
#if GC_TYPE==GENERATIONAL
    prim = new (_HEAP, _PAGE, 0, true) VMPrimitive(*this);
#elif GC_TYPE==PAUSELESS
    prim = new (_PAGE) VMPrimitive(*this);
#else
    prim = new (_HEAP) VMPrimitive(*this);
#endif
    return prim;
}

void VMPrimitive::EmptyRoutine( pVMObject _self, pVMFrame /*frame*/) {
    pVMInvokable self = static_cast<pVMInvokable>(_self);
    pVMSymbol sig = self->GetSignature();
    cout << "undefined primitive called: " << sig->GetChars() << endl;
}

#if GC_TYPE==PAUSELESS
void VMPrimitive::MarkReferences(Worklist* worklist) {
    worklist->AddWork(clazz);
    worklist->AddWork(signature);
    worklist->AddWork(holder);
}
#else
void VMPrimitive::WalkObjects(VMOBJECT_PTR (*walk)(VMOBJECT_PTR)) {
    clazz     = static_cast<pVMClass>(walk(clazz));
    signature = static_cast<pVMSymbol>(walk(signature));
    holder    = static_cast<pVMClass>(walk(holder));
}
#endif
