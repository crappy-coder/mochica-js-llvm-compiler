#ifndef MOENJIN_RUNTIME_JSSTACKFRAME_H
#define MOENJIN_RUNTIME_JSSTACKFRAME_H

#include "Platform/Configuration.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSList.h"
#include "Runtime/JSNativeExceptionHandler.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	#define JSNoArgs JSList()

	class JSExecutionEnvironment;
	class JSGlobalObject;
	class JSFunction;
	class JSObject;

	enum JSCallType
	{
		kJSCallTypeNone = 0,
		kJSCallTypeHost,
		kJSCallTypeScript
	};

	struct JSCallInfo
	{
		JSCallType type;
		NativeFunctionPtr function;
		JSExecutionEnvironment* scope;
	};

	class JSStackFrame
	{
		class AutoPopStackFrame 
		{
			public:
				AutoPopStackFrame(JSStackFrame* newFrame)
					: mStackFrame(newFrame) { }

				~AutoPopStackFrame() { mStackFrame->Pop(); }

				JSStackFrame* Get() const { return mStackFrame; }
				JSStackFrame* operator->() { return mStackFrame; }

			private:
				JSStackFrame* mStackFrame;
		};

		enum { MaximumStackSize = 512 };

		public:
			static JSStackFrame* Create(JSGlobalObject* globalObject, JSObject* callee, JSValue thisValue, JSStackFrame* caller, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv);

			static void ThrowRuntimeException(JSStackFrame* sourceCallFrame, JSValue exception);

		public:
			JSStackFrame* GetCallerFrame() const { return mCallerFrame; }
			JSValue GetThisValue() const { return mThisValue; }
			JSObject* GetCallee() const { return mCallee; }
			JSObject* GetGlobalThis() const { return mGlobalObject; }
			JSGlobalObject* GetGlobalObject() const { return mGlobalObject; }
			JSExecutionEnvironment* GetVariableEnvironment() const { return mVariableEnv; }
			JSExecutionEnvironment* GetLexicalEnvironment() const { return mLexicalEnv; }
			mo_int32 GetIndex() const { return mIndex; }
			mo_int32 GetArgumentCount() const { return mArguments.GetLength(); }
			JSNativeExceptionHandler* GetExceptionHandler() const { return mExceptionHandler; }

			JSList& GetArguments() { return mArguments; }
			JSValue GetArgument(mo_int32 index) const;

			mo_bool HasArguments() const { return !mArguments.IsEmpty(); }
			mo_bool HasException() const;
			mo_bool HasExceptionHandler() const { return mExceptionHandler != mo_null; }
			mo_bool HasCaller() const { return (mCallerFrame != mo_null); }

			void AddArgument(JSValue value);
			void SetExceptionHandler(JSNativeExceptionHandler* handler) { mExceptionHandler = handler; }

			JSStackFrame* Push(JSObject* callee, JSValue thisValue, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv);
			JSStackFrame* Pop();

			JSValue ResolveBase(const UTF8String& name);

			JSValue ExecuteCall(JSObject* function, JSCallInfo info, JSValue thisValue, const JSArgList& args);
			JSObject* ExecuteConstruct(JSObject* constructor, JSCallInfo info, const JSArgList& args);

			void EnterExceptionEnvironment();
			void ExitExceptionEnvironment();

		private:
			JSStackFrame(JSGlobalObject* globalObject, JSObject* callee, JSValue thisValue, JSStackFrame* caller, JSExecutionEnvironment* lexicalEnv, JSExecutionEnvironment* variableEnv);

		private:
			JSGlobalObject* mGlobalObject;
			JSValue mThisValue;
			JSObject* mCallee;
			JSStackFrame* mCallerFrame;
			JSExecutionEnvironment* mVariableEnv;
			JSExecutionEnvironment* mLexicalEnv;
			JSNativeExceptionHandler* mExceptionHandler;
			JSList mArguments;
			mo_int32 mIndex;
	};

	mo_inline JSCallInfo jsNoCallInfo()
	{
		JSCallInfo info;
		info.type = kJSCallTypeNone;
		info.function = mo_null;
		info.scope = mo_null;

		return info;
	}

	mo_inline JSCallInfo jsHostCallInfo(NativeFunctionPtr func)
	{
		JSCallInfo info;
		info.type = kJSCallTypeHost;
		info.function = func;
		info.scope = mo_null;

		return info;
	}

	mo_inline JSCallInfo jsScriptCallInfo(NativeFunctionPtr func, JSExecutionEnvironment* scope)
	{
		JSCallInfo info;
		info.type = kJSCallTypeScript;
		info.function = func;
		info.scope = scope;

		return info;
	}
}

#endif