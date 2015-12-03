#include "Runtime/JSStringPrototype.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
	static JSValueImpl JSStringPrototype_ToString(JSStackFrame*);
	static JSValueImpl JSStringPrototype_ValueOf(JSStackFrame*);
	static JSValueImpl JSStringPrototype_CharAt(JSStackFrame*);
	static JSValueImpl JSStringPrototype_CharCodeAt(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Concat(JSStackFrame*);
	static JSValueImpl JSStringPrototype_IndexOf(JSStackFrame*);
	static JSValueImpl JSStringPrototype_LastIndexOf(JSStackFrame*);
	static JSValueImpl JSStringPrototype_LocaleCompare(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Match(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Replace(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Search(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Slice(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Split(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Substr(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Substring(JSStackFrame*);
	static JSValueImpl JSStringPrototype_ToLowerCase(JSStackFrame*);
	static JSValueImpl JSStringPrototype_ToLocaleLowerCase(JSStackFrame*);
	static JSValueImpl JSStringPrototype_ToUpperCase(JSStackFrame*);
	static JSValueImpl JSStringPrototype_ToLocaleUpperCase(JSStackFrame*);
	static JSValueImpl JSStringPrototype_Trim(JSStackFrame*);
}

#include "Runtime/Tables/JSStringPrototypeTable.h"

namespace MoEnjin
{
	JS_DEFINE_AS_SUBCLASS(JSStringPrototype, String, JSStringInstance);

	JSStringPrototype::JSStringPrototype(JSGlobalObject* globalObject, JSValue prototype)
		: JSStringInstance(globalObject, jsEmptyString(globalObject), prototype, &ClassInfo) { }

	mo_bool JSStringPrototype::GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor)
	{
		return GetStaticFunctionDescriptor<JSStringInstance>(exec, &JSStringPrototypeTable, JSConvert::Cast<JSStringPrototype*>(obj), propertyName, descriptor);
	}

	mo_bool JSStringPrototype::GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot)
	{
		return GetStaticFunctionSlot<JSStringInstance>(exec, &JSStringPrototypeTable, JSConvert::Cast<JSStringPrototype*>(obj), propertyName, slot);
	}

	//
	// String Prototype Functions
	//
	JSValueImpl JSStringPrototype_ToString(JSStackFrame* exec)
	{
		JSValue me = exec->GetThisValue();

		if(me.IsString())
			return JSValue::Encode(me);

		if(me.IsSubClassOf(&JSStringInstance::ClassInfo))
			return JSValue::Encode(JSConvert::Cast<JSStringInstance*>(me)->GetValue());

		return JSValue::Encode(JSString::Create(exec->GetGlobalObject(), "ERROR: String.toString threw a TypeError."));
	}

	JSValueImpl JSStringPrototype_ValueOf(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_CharAt(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_CharCodeAt(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Concat(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_IndexOf(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_LastIndexOf(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_LocaleCompare(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Match(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Replace(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Search(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Slice(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Split(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Substr(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Substring(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_ToLowerCase(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_ToLocaleLowerCase(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_ToUpperCase(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_ToLocaleUpperCase(JSStackFrame* exec)
	{
		return jsUndef();
	}

	JSValueImpl JSStringPrototype_Trim(JSStackFrame* exec)
	{
		return jsUndef();
	}
}