#include "Runtime/JSStackFrame.h"
#include "Runtime/JSErrors.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSScriptEnvironment.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	static mo_bool IsThisObjectValid(JSStackFrame* exec, JSValue thisValue);

	JSStackFrame* JSStackFrame::Create(JSGlobalObject* globalObject, JSObject* callee, JSValue thisValue, JSStackFrame* caller, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv)
	{
		// stack size limit has been reached!
		if(caller && caller->mIndex+1 == MaximumStackSize)
			return mo_null;

		return new JSStackFrame(globalObject, callee, thisValue, caller, lexicalEnv, variableEnv);
	}

	JSStackFrame::JSStackFrame(JSGlobalObject* globalObject, JSObject* callee, JSValue thisValue, JSStackFrame* caller, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv)
		: mGlobalObject(globalObject)
		, mCallee(callee)
		, mThisValue(thisValue)
		, mCallerFrame(caller)
		, mLexicalEnv(lexicalEnv)
		, mVariableEnv(variableEnv)
		, mIndex(caller == mo_null ? 0 : caller->mIndex+1)
		, mExceptionHandler(mo_null) { }

	JSValue JSStackFrame::GetArgument(mo_int32 index) const
	{
		return mArguments.Get(index);
	}

	void JSStackFrame::AddArgument(JSValue value)
	{
		mArguments.Add(value);
	}

	mo_bool JSStackFrame::HasException() const
	{
		return mGlobalObject->HasException();
	}

	JSStackFrame* JSStackFrame::Push(JSObject* callee, JSValue thisValue, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv)
	{
		JSStackFrame* newFrame = JSStackFrame::Create(mGlobalObject, callee, thisValue, this, lexicalEnv, variableEnv);

		if(!newFrame)
			return mo_null;

		mGlobalObject->SetCurrentStackFrame(newFrame);

		return newFrame;
	}

	JSStackFrame* JSStackFrame::Pop()
	{
		if(mCallerFrame)
			mGlobalObject->SetCurrentStackFrame(mCallerFrame);

		return mGlobalObject->GetCurrentStackFrame();
	}

	// ECMA 5.1, §10.2.2.1 and §10.3.1
	JSValue JSStackFrame::ResolveBase(const UTF8String& name)
	{
		JSExecutionEnvironment* env = GetLexicalEnvironment();

		while(env)
		{
			if(env->HasBinding(this, name))
				return jsObject(env);

			env = env->GetParent();
		}

		return jsUndef();
	}

	// ECMA 5.1, §10.4.3 - Entering Function Code
	JSValue JSStackFrame::ExecuteCall(JSObject* function, JSCallInfo info, JSValue thisValue, const JSArgList& args)
	{
		MO_ASSERT(!HasException());

		JSExecutionEnvironment* scope = GetVariableEnvironment();
		JSValue result = jsUndef();

		// if this is a js call then we want to create a local
		// environment for this call to execute in
		if(info.scope)
			scope = JSScriptEnvironment::Create(mGlobalObject, info.scope);

		mo_int32 x = 0;

		// create new execution context
		JSStackFrame* newFrame = Push(function, thisValue.ToThisObject(this), scope, scope);

		// unable to create any more new stack frames
		if(!newFrame)
			jsThrowStackOverflowError(this);
		else
		{
			AutoPopStackFrame newFrameScope(newFrame);

			// assign the arguments to the call frame
			for(mo_int32 i = 0; i < args.GetLength(); ++i)
				newFrame->AddArgument(args.Get(i));

			JSFunction* f = mo_cast<JSFunction*>(function);
			mo_debug_print("ExecuteCall begin, function=%s, address=%p, depth=%d\n", f->GetName(this).GetChars(), info.function, mIndex);

			// call the native function, the native function will handle the last step, which is
			// to Perform Declaration Binding Instantiation (ECMA 5.1, §10.5)
			result = JSValue::Decode(info.function(newFrame));

			mo_debug_print("ExecuteCall end, function=%s, address=%p, depth=%d\n", f->GetName(this).GetChars(), info.function, mIndex);
		}

		return result;
	}

	JSObject* JSStackFrame::ExecuteConstruct(JSObject* constructor, JSCallInfo info, const JSArgList& args)
	{
		JSValue result;
		JSObject* thisObj = GetGlobalThis();

		// host constructs are handled in their own native function
		if(info.type == kJSCallTypeScript)
		{
			JSValue protoValue = constructor->Get(this, KnownIdentifiers::prototype);
			JSObject* proto = (protoValue.IsNativeObject() ? protoValue.ToNativeObject() : mGlobalObject->GetObjectPrototype());

			thisObj = JSObject::Create(this, proto);
		}

		result = ExecuteCall(constructor, info, thisObj, args);

		if(result.IsObject())
			return result.ToObject(this);

		return (info.type == kJSCallTypeHost ? mo_null : thisObj);
	}

	void JSStackFrame::EnterExceptionEnvironment()
	{
		JSExecutionEnvironment* env = JSScriptEnvironment::Create(mGlobalObject, mLexicalEnv);
		mLexicalEnv = env;
	}

	void JSStackFrame::ExitExceptionEnvironment()
	{
		MO_ASSERT(mLexicalEnv->GetParent() != mo_null && "ExitExceptionEnvironment: Existing lexical environment is the top-most environment!");
		mLexicalEnv = mLexicalEnv->GetParent();		
	}

	void JSStackFrame::ThrowRuntimeException(JSStackFrame* sourceCallFrame, JSValue exception)
	{
		// unwind callframe until we find the nearest handler or we've
		// reached the end
		JSStackFrame* callframe = sourceCallFrame;
		JSGlobalObject* globalObject = callframe->GetGlobalObject();
		JSNativeExceptionHandler* exceptionHandler = mo_null;
		JSNativeExceptionHandler* unhandledExceptionHandler = globalObject->GetUnhandledExceptionHandler();

		while(true)
		{
			// check if this callframe has a handler
			exceptionHandler = callframe->GetExceptionHandler();

			if(exceptionHandler)
				break;

			// go to the next callframe in the stack 
			if(callframe->HasCaller())
			{
				callframe = callframe->Pop();
				continue;
			}

			break;
		}

		// no handler exists, check if we have a handler for unhandled exceptions
		if(!exceptionHandler)
		{
			mo_debug_print("Warning: ThrowRuntimeException, no catch handler available for exception! Checking for unhandled exception handler..\n");
			exceptionHandler = unhandledExceptionHandler;
		}

		// TODO: should terminate the program here since this would just crash.. though
		//       by default we always install an unhandled exception handler just in case
		//       shit goes bad, so this is very unlikely
		//
		if(!exceptionHandler)
		{
			mo_debug_print("Error: ThrowRuntimeException, no handlers available to catch the exception!\n");
			return;
		}

		mo_debug_print("Exception thrown, will be handled by the catch handler: %p.\n", exceptionHandler);

		// restore the callframes previous exception handler in case we are in a 
		// nested try/catch
		if(exceptionHandler != unhandledExceptionHandler)
		{
			// restore the previous handler and clear it's reference on this handler
			JSNativeExceptionHandler* previousHandler = exceptionHandler->previous;
			exceptionHandler->previous = mo_null;

			callframe->SetExceptionHandler(previousHandler);
		}

		// must clear the exception, otherwise subsequent code would just rethrow
		globalObject->ClearException();

		// save the exception value in the handler, this is what we'll use from
		// now on to get access to it
		exceptionHandler->exceptionValue = JSValue::Encode(exception);

		// jump to the catch handler block
		longjmp(exceptionHandler->jmpInfo, 1);
	}

	mo_bool IsThisObjectValid(JSStackFrame* exec, JSValue thisValue)
	{
		return (!thisValue.IsObject() || thisValue.ToThisObject(exec) == thisValue);
	}
}