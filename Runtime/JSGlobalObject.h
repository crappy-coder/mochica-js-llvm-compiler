#ifndef MOENJIN_RUNTIME_JSGLOBALOBJECT_H
#define MOENJIN_RUNTIME_JSGLOBALOBJECT_H

#include "Platform/Configuration.h"
#include "Runtime/JSBooleanPrototype.h"
#include "Runtime/JSErrorPrototype.h"
#include "Runtime/JSFunctionPrototype.h"
#include "Runtime/JSNativeExceptionHandler.h"
#include "Runtime/JSNumberPrototype.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSObjectPrototype.h"
#include "Runtime/JSStringPrototype.h"

namespace MoEnjin
{
	class JSExecutionEnvironment;
	class JSStackFrame;
	class JSSmallStrings;
	class JSNativeErrorConstructor;
	class JSNumericStringCache;

	class JSGlobalObject : public JSObject
	{
		MO_NON_COPYABLE_CLASS(JSGlobalObject);

		friend class JSCallFrame;

		public:
			JS_CLASS();

			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);

		public:
			static JSGlobalObject* Create();
			static JSGlobalObject* Create(JSValue prototype);
			static JSGlobalObject* Create(JSValue prototype, const JSClassInfo* classInfo);

		public:
			void SetCurrentStackFrame(JSStackFrame* frame) { mCurrentStackFrame = frame; }

			JSStackFrame* GetCurrentStackFrame() const { return mCurrentStackFrame; }
			JSStackFrame* GetGlobalStackFrame() const { return mFirstStackFrame; }
			JSExecutionEnvironment* GetEnvironment() const { return mGlobalEnv; }

			mo_bool HasException() const { return !mException.IsEmpty(); }
			mo_bool HasLastException() const { return !mLastException.IsEmpty(); }
			mo_bool HasUnhandledExceptionHandler() const { return mUnhandledExceptionHandler != mo_null; }

			JSValue GetException() const { return mException; }
			JSValue GetLastException() const { return mLastException; }
			JSNativeExceptionHandler* GetUnhandledExceptionHandler() const { return mUnhandledExceptionHandler; }

			void SetUnhandledExceptionHandler(JSNativeExceptionHandler* handler) { mUnhandledExceptionHandler = handler; }
			void SetException(JSValue exception);

			void ClearException();

			JSObjectPrototype* GetObjectPrototype() const { return mObjectPrototype; }
			JSBooleanPrototype* GetBooleanPrototype() const { return mBooleanPrototype; }
			JSErrorPrototype* GetErrorPrototype() const { return mErrorPrototype; }
			JSFunctionPrototype* GetFunctionPrototype() const { return mFunctionPrototype; }
			JSNumberPrototype* GetNumberPrototype() const { return mNumberPrototype; }
			JSStringPrototype* GetStringPrototype() const { return mStringPrototype; }

			// TODO - implement the real prototypes
			JSObjectPrototype* GetDatePrototype() const { return mObjectPrototype; }

			JSNativeErrorConstructor* GetEvalErrorConstructor() const { return mEvalErrorConstructor; }
			JSNativeErrorConstructor* GetRangeErrorConstructor() const { return mRangeErrorConstructor; }
			JSNativeErrorConstructor* GetReferenceErrorConstructor() const { return mReferenceErrorConstructor; }
			JSNativeErrorConstructor* GetSyntaxErrorConstructor() const { return mSyntaxErrorConstructor; }
			JSNativeErrorConstructor* GetTypeErrorConstructor() const { return mTypeErrorConstructor; }
			JSNativeErrorConstructor* GetURIErrorConstructor() const { return mURIErrorConstructor; }

			JSStackFrame* InitializeExecutionState();

			JSSmallStrings* SmallStrings;
			JSNumericStringCache* NumericStrings;

		private:
			JSGlobalObject(JSValue prototype, const JSClassInfo* classInfo);

			void InitializeBuiltInProperties(JSStackFrame* exec);

		private:
			JSStackFrame* mFirstStackFrame;
			JSStackFrame* mCurrentStackFrame;
			JSExecutionEnvironment* mGlobalEnv;
			JSNativeExceptionHandler* mUnhandledExceptionHandler;
			JSValue mException;
			JSValue mLastException;
			JSObjectPrototype* mObjectPrototype;
			JSBooleanPrototype* mBooleanPrototype;
			JSErrorPrototype* mErrorPrototype;
			JSFunctionPrototype* mFunctionPrototype;
			JSNumberPrototype* mNumberPrototype;
			JSStringPrototype* mStringPrototype;
			JSNativeErrorConstructor* mEvalErrorConstructor;
			JSNativeErrorConstructor* mRangeErrorConstructor;
			JSNativeErrorConstructor* mReferenceErrorConstructor;
			JSNativeErrorConstructor* mSyntaxErrorConstructor;
			JSNativeErrorConstructor* mTypeErrorConstructor;
			JSNativeErrorConstructor* mURIErrorConstructor;
	};
}

#endif