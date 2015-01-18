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

#include "Block.h"

#include "../primitivesCore/Routine.h"

#include "../interpreter/Interpreter.h"

#include <vmobjects/VMObject.h>
#include <vmobjects/VMFrame.h>
#include <vmobjects/VMMethod.h>
#include <vmobjects/VMClass.h>

#include <vmobjects/VMThread.h>

#include <interpreter/bytecodes.h>
#include <vm/Universe.h>

#include <vmobjects/VMMethod.inline.h>

void _Block::Value(Interpreter*, VMFrame*) {
    // intentionally left blank
}

void _Block::Value_(Interpreter*, VMFrame*) {
    // intentionally left blank
}

void _Block::Value_with_(Interpreter*, VMFrame*) {
    // intentionally left blank
}

void _Block::Restart(Interpreter*, VMFrame* frame) {
    frame->SetBytecodeIndex(0);
    frame->ResetStackPointer();
}

_Block::_Block() : PrimitiveContainer() {
    SetPrimitive("value",       new Routine<_Block>(this, &_Block::Value,       false));
    SetPrimitive("restart",     new Routine<_Block>(this, &_Block::Restart,     false));
    SetPrimitive("value_",      new Routine<_Block>(this, &_Block::Value_,      false));
    SetPrimitive("value_with_", new Routine<_Block>(this, &_Block::Value_with_, false));

    SetPrimitive("spawnWithArgument_", new Routine<_Block>(this, &_Block::SpawnWithArgument, false));
    SetPrimitive("spawn",       new Routine<_Block>(this, &_Block::Spawn,       false));
}

VMMethod* _Block::CreateFakeBootstrapMethod() {
    VMMethod* bootstrapVMMethod = GetUniverse()->NewMethod(GetUniverse()->SymbolForChars("bootstrap"), 1, 0);
    bootstrapVMMethod->SetBytecode(0, BC_HALT);
    bootstrapVMMethod->SetNumberOfLocals(0);
    bootstrapVMMethod->SetMaximumNumberOfStackElements(2);
    bootstrapVMMethod->SetHolder(GetUniverse()->GetBlockClass());
    return bootstrapVMMethod;
}

//setting up thread object
VMThread* _Block::CreateNewThread(VMBlock* block) {
    VMThread* thread = GetUniverse()->NewThread();
    VMSignal* signal = GetUniverse()->NewSignal();
    thread->SetResumeSignal(signal);
    thread->SetShouldStop(false);
    thread->SetBlockToRun(block);
    return thread;
}

void* _Block::ThreadForBlock(void* threadPointer) {
    //create new interpreter which will process the block
    Interpreter* interpreter = GetUniverse()->NewInterpreter();
    VMThread* thread = (VMThread*)threadPointer;
    VMBlock* block = thread->GetBlockToRun();
    interpreter->SetThread(thread);
    
    // fake bootstrap method to simplify later frame traversal
    VMMethod* bootstrapVMMethod = CreateFakeBootstrapMethod();
    // create a fake bootstrap frame with the block object on the stack
    VMFrame* bootstrapVMFrame = interpreter->PushNewFrame(bootstrapVMMethod);
    bootstrapVMFrame->Push((VMObject*)block);
    
    // lookup the initialize invokable on the system class
    VMInvokable* initialize = (VMInvokable*)GetUniverse()->GetBlockClass()->LookupInvokable(GetUniverse()->SymbolForChars("evaluate"));
    // invoke the initialize invokable
    initialize->Invoke(interpreter, bootstrapVMFrame);
    // start the interpreter
    interpreter->Start();
    
#if GC_TYPE != PAUSELESS
    // exit this thread and decrement the number of active threads, this is part of a stop the world thread barrier needed for GC
    _HEAP->DecrementThreadCount();
#endif
    
    GetUniverse()->RemoveInterpreter();
    
#if GC_TYPE!=PAUSELESS
    delete interpreter;
#endif

    pthread_exit(nullptr);
}

void* _Block::ThreadForBlockWithArgument(void* threadPointer) {
    //create new interpreter which will process the block
    Interpreter* interpreter = GetUniverse()->NewInterpreter();
    VMThread* thread = (VMThread*)threadPointer;
    VMBlock* block = thread->GetBlockToRun();
    interpreter->SetThread(thread);
    
    // fake bootstrap method to simplify later frame traversal
    VMMethod* bootstrapVMMethod = CreateFakeBootstrapMethod();
    // create a fake bootstrap frame with the block object on the stack
    VMFrame* bootstrapVMFrame = interpreter->PushNewFrame(bootstrapVMMethod);
    bootstrapVMFrame->Push((VMObject*)block);
    vm_oop_t arg = thread->GetArgument();
    bootstrapVMFrame->Push(arg);
    
    // lookup the initialize invokable on the system class
    VMInvokable* initialize = (VMInvokable*)GetUniverse()->GetBlockClass()->LookupInvokable(GetUniverse()->SymbolForChars("evaluate"));
    // invoke the initialize invokable
    initialize->Invoke(interpreter, bootstrapVMFrame);
    // start the interpreter
    interpreter->Start();
    
#if GC_TYPE != PAUSELESS
    // exit this thread and decrement the number of active threads, this is part of a stop the world thread barrier needed for GC
    _HEAP->DecrementThreadCount();
#endif
    
    GetUniverse()->RemoveInterpreter();
    
    //still need to happen at the end of each cycle of the pauseless, we should thus keep track of interpreters that need to be deleted
#if GC_TYPE!=PAUSELESS
    delete interpreter;
#endif
    
    pthread_exit(nullptr);
}

//spawning of new thread that will run the block
void _Block::Spawn(Interpreter*, VMFrame* frame) {
    pthread_t tid = 0;
    VMBlock* block = (VMBlock*)frame->Pop();
    // create the thread object and setting it up
    VMThread* thread = CreateNewThread(block);
    // create the pthread but first increment the number of active threads (this is part of a thread barrier needed for GC)
#if GC_TYPE!=PAUSELESS
    _HEAP->IncrementThreadCount();
#endif
    //GetUniverse()->IncrementThreadCount();
    pthread_create(&tid, nullptr, &ThreadForBlock, (void*)thread);
    thread->SetEmbeddedThreadId(tid);
    frame->Push(thread);
}

void _Block::SpawnWithArgument(Interpreter*, VMFrame* frame) {
    pthread_t tid = 0;
    // Get the argument
    vm_oop_t argument = frame->Pop();
    VMBlock* block = (VMBlock*)frame->Pop();
    // create the thread object and setting it up
    VMThread* thread = CreateNewThread(block);
    thread->SetArgument(argument);
    // create the pthread but first increment the number of active threads (this is part of a thread barrier needed for GC)
#if GC_TYPE!=PAUSELESS
    _HEAP->IncrementThreadCount();
#endif
    //GetUniverse()->IncrementThreadCount();
    pthread_create(&tid, nullptr, &ThreadForBlockWithArgument, (void *)thread);
    thread->SetEmbeddedThreadId(tid);
    frame->Push(thread);
}



//# define VALUE_TO_STRING(x) #x
//# define VALUE(x) VALUE_TO_STRING(x)
//# define VAR_NAME_VALUE(var) #var "=" VALUE(var)
//
//# pragma message (VAR_NAME_VALUE(GC_TYPE))
//# error test