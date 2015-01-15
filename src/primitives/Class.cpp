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

#include "Class.h"

#include "../primitivesCore/Routine.h"

#include <vmobjects/VMObject.h>
#include <vmobjects/VMFrame.h>
#include <vmobjects/VMClass.h>

#include <vm/Universe.h>

_Class::_Class() : PrimitiveContainer() {
    SetPrimitive("new",        new Routine<_Class>(this, &_Class::New));
    SetPrimitive("name",       new Routine<_Class>(this, &_Class::Name));
    SetPrimitive("superclass", new Routine<_Class>(this, &_Class::Superclass));
    SetPrimitive("fields",     new Routine<_Class>(this, &_Class::Fields));
    SetPrimitive("methods",    new Routine<_Class>(this, &_Class::Methods));
}

    frame->Push(_UNIVERSE->NewInstance(self));
void _Class::New(VMObject* /*object*/, VMFrame* frame) {
    VMClass* self = static_cast<VMClass*>(frame->Pop());
}

void _Class::Name(VMObject*, VMFrame* frame) {
    VMClass* self = static_cast<VMClass*>(frame->Pop());
    frame->Push(self->GetName());
}

void _Class::Superclass(VMObject*, VMFrame* frame) {
    VMClass* self = static_cast<VMClass*>(frame->Pop());
    frame->Push(self->GetSuperClass());
}

void _Class::Methods(VMObject*, VMFrame* frame) {
    VMClass* self = static_cast<VMClass*>(frame->Pop());
    frame->Push(self->GetInstanceInvokables());
}

void _Class::Fields(VMObject*, VMFrame* frame) {
    VMClass* self = static_cast<VMClass*>(frame->Pop());
    frame->Push(self->GetInstanceFields());
}
