#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSBooleanConstructor.h"
#include "Runtime/JSErrorConstructor.h"
#include "Runtime/JSFunctionConstructor.h"
#include "Runtime/JSNativeErrorConstructor.h"
#include "Runtime/JSNativeErrorPrototype.h"
#include "Runtime/JSNumberConstructor.h"
#include "Runtime/JSNumericStringCache.h"
#include "Runtime/JSObjectConstructor.h"
#include "Runtime/JSObjectEnvironment.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSSmallStrings.h"
#include "Runtime/JSStringConstructor.h"
#include "Runtime/JSString.h"
#include "Runtime/JSValue.h"
#include "Runtime/KnownIdentifiers.h"
#include "Platform/StackInfo.h"

namespace MoEnjin
{
	static JSValueImpl JSGlobalObject_ParseInt(JSStackFrame*);
	static JSValueImpl JSGlobalObject_ParseFloat(JSStackFrame*);
	static JSValueImpl JSGlobalObject_IsNaN(JSStackFrame*);
	static JSValueImpl JSGlobalObject_IsFinite(JSStackFrame*);
	static JSValueImpl JSGlobalObject_Escape(JSStackFrame*);
	static JSValueImpl JSGlobalObject_Unescape(JSStackFrame*);
	static JSValueImpl JSGlobalObject_DecodeURI(JSStackFrame*);
	static JSValueImpl JSGlobalObject_DecodeURIComponent(JSStackFrame*);
	static JSValueImpl JSGlobalObject_EncodeURI(JSStackFrame*);
	static JSValueImpl JSGlobalObject_EncodeURIComponent(JSStackFrame*);
	static JSValueImpl JSGlobalObject_Print(JSStackFrame*);
}

#include "Runtime/Tables/JSGlobalObjectTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSGlobalObject, GlobalObject, JSObject);

	JSGlobalObject::JSGlobalObject(JSValue prototype, const JSClassInfo* classInfo)
		: JSObject(this, JSObjectTypeGlobal, prototype, classInfo)
		, mUnhandledExceptionHandler(mo_null)
		, mFirstStackFrame(mo_null)
		, mCurrentStackFrame(mo_null)
		, mGlobalEnv(mo_null)
		, mBooleanPrototype(mo_null)
		, mErrorPrototype(mo_null)
		, mFunctionPrototype(mo_null)
		, mObjectPrototype(mo_null)
		, mNumberPrototype(mo_null)
		, mStringPrototype(mo_null)
		, mEvalErrorConstructor(mo_null)
		, mRangeErrorConstructor(mo_null)
		, mReferenceErrorConstructor(mo_null)
		, mSyntaxErrorConstructor(mo_null)
		, mTypeErrorConstructor(mo_null)
		, mURIErrorConstructor(mo_null)
		, NumericStrings(new JSNumericStringCache())
		, SmallStrings(new JSSmallStrings()) { }

	JSStackFrame* JSGlobalObject::InitializeExecutionState()
	{
		// setup built-in prototypes
		mObjectPrototype = JSObjectPrototype::Create(this);
		mBooleanPrototype = JSBooleanPrototype::Create(this, mObjectPrototype);
		mErrorPrototype = JSErrorPrototype::Create(this, mObjectPrototype);
		mFunctionPrototype = JSFunctionPrototype::Create(this, mObjectPrototype);
		mNumberPrototype = JSNumberPrototype::Create(this, mObjectPrototype);
		mStringPrototype = JSStringPrototype::Create(this, mObjectPrototype);

		// setup the initial execution state for a program
		mException = JSValue::MakeEmpty();
		mLastException = JSValue::MakeEmpty();
		mGlobalEnv = JSObjectEnvironment::Create(this, this);
		mFirstStackFrame = JSStackFrame::Create(this, this, jsObject(this), mo_null, mGlobalEnv, mGlobalEnv);
		mCurrentStackFrame = mFirstStackFrame;

		// setup all the built-in properties, i.e. Object, Function, parseInt, undefined, NaN, etc...
		InitializeBuiltInProperties(mCurrentStackFrame);

		// send the top level stack frame back to the program init
		return mFirstStackFrame;
	}

	void JSGlobalObject::InitializeBuiltInProperties(JSStackFrame* exec)
	{
		JSObjectConstructor* objectConstructor = JSObjectConstructor::Create(exec);
		JSFunctionConstructor* functionConstructor = JSFunctionConstructor::Create(exec);
		JSBooleanConstructor* booleanConstructor = JSBooleanConstructor::Create(exec);
		JSErrorConstructor* errorConstructor = JSErrorConstructor::Create(exec);
		JSNumberConstructor* numberConstructor = JSNumberConstructor::Create(exec);
		JSStringConstructor* stringConstructor = JSStringConstructor::Create(exec);
		
		mEvalErrorConstructor = JSNativeErrorConstructor::Create(exec, "EvalError");
		mRangeErrorConstructor = JSNativeErrorConstructor::Create(exec, "RangeError");
		mReferenceErrorConstructor = JSNativeErrorConstructor::Create(exec, "ReferenceError");
		mSyntaxErrorConstructor = JSNativeErrorConstructor::Create(exec, "SyntaxError");
		mTypeErrorConstructor = JSNativeErrorConstructor::Create(exec, "TypeError");
		mURIErrorConstructor = JSNativeErrorConstructor::Create(exec, "URIError");

		// set prototype constructors
		mObjectPrototype->Put(exec, KnownIdentifiers::constructor, objectConstructor, JSPropertyAttributeDontEnum);
		mFunctionPrototype->Put(exec, KnownIdentifiers::constructor, functionConstructor, JSPropertyAttributeDontEnum);
		mBooleanPrototype->Put(exec, KnownIdentifiers::constructor, booleanConstructor, JSPropertyAttributeDontEnum);
		mErrorPrototype->Put(exec, KnownIdentifiers::constructor, errorConstructor, JSPropertyAttributeDontEnum);
		mNumberPrototype->Put(exec, KnownIdentifiers::constructor, numberConstructor, JSPropertyAttributeDontEnum);
		mStringPrototype->Put(exec, KnownIdentifiers::constructor, stringConstructor, JSPropertyAttributeDontEnum);

		// set built-in constructors
		Put(exec, "Object", objectConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "Function", functionConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "Boolean", booleanConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "Error", errorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "Number", numberConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "String", stringConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "EvalError", mEvalErrorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "RangeError", mRangeErrorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "ReferenceError", mReferenceErrorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "SyntaxError", mSyntaxErrorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "TypeError", mTypeErrorConstructor, JSPropertyAttributeDontEnum);
		Put(exec, "URIError", mURIErrorConstructor, JSPropertyAttributeDontEnum);
	}

	void JSGlobalObject::SetException(JSValue exception)
	{
		// make sure it's a new exception
		if(exception == mException)
			return;

		// clear the current exception, which also stores it as
		// the last exception
		ClearException();

		mException = exception;
	}

	void JSGlobalObject::ClearException()
	{
		// store as the last exception thrown, we can use this
		// for reporting, analysis, etc...
		if(!mException.IsEmpty())
			mLastException = mException;

		// clear it
		mException = JSValue::MakeEmpty();
	}

	mo_bool JSGlobalObject::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		JSGlobalObject* thisObject = JSConvert::Cast<JSGlobalObject*>(obj);

		if(GetStaticFunctionDescriptor<JSObject>(exec, &JSGlobalObjectTable, thisObject, propertyName, descriptor))
			return mo_true;

		return JSObject::GetOwnPropertyDescriptorImpl(exec, obj, propertyName, descriptor);
	}

	mo_bool JSGlobalObject::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		JSGlobalObject* thisObject = JSConvert::Cast<JSGlobalObject*>(obj);

		if(GetStaticFunctionSlot<JSObject>(exec, &JSGlobalObjectTable, thisObject, propertyName, slot))
			return mo_true;

		return JSObject::GetOwnPropertySlotImpl(exec, obj, propertyName, slot);	
	}

	//
	// Global Object Functions
	//
	JSValueImpl JSGlobalObject_ParseInt(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_ParseFloat(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_IsNaN(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_IsFinite(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_Escape(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_Unescape(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_DecodeURI(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_DecodeURIComponent(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_EncodeURI(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_EncodeURIComponent(JSStackFrame* exec)
	{
		return JSValue::Encode(jsUndef());
	}

	JSValueImpl JSGlobalObject_Print(JSStackFrame* exec)
	{
		// nothing to print
		if(!exec->HasArguments())
			return JSValue::Encode(jsUndef());

		UTF8String message;
		mo_int32 len = exec->GetArgumentCount();

		for(mo_int32 i = 0; i < len; ++i)
		{
			JSValue arg = exec->GetArgument(i);
			UTF8String str = arg.ToString(exec)->GetValue();

			message += (i > 0 ? " " : "") + (str.IsNull() ? " " : str);
		}

		mo_debug_print("%s\n", message.GetChars());
		return JSValue::Encode(jsUndef());
	}
}