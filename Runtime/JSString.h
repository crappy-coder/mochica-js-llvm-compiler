#ifndef MOENJIN_RUNTIME_JSSTRING_H
#define MOENJIN_RUNTIME_JSSTRING_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSString : public JSObject
	{
		friend class JSStringRope;

		MO_NON_COPYABLE_CLASS(JSString);

		public:
			JS_CLASS();

			static JSString* Create(JSGlobalObject* globalObject);
			static JSString* Create(JSGlobalObject* globalObject, const UTF8String& str);
			static JSString* CreateEmpty(JSGlobalObject* globalObject);

			static JSString* FromString(JSGlobalObject* globalObject, const UTF8String& str);
			static JSString* FromString(JSGlobalObject* globalObject, JSString* str);
			static JSString* FromSubstring(JSGlobalObject* globalObject, const UTF8String& str, mo_unsigned offset, mo_unsigned length = 1);
			static JSString* FromSubstring(JSGlobalObject* globalObject, JSString* str, mo_unsigned offset, mo_unsigned length = 1);
			static JSString* FromCharacter(JSGlobalObject* globalObject, mo_utf8char ch);

			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);
			static mo_bool GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot);
			static JSObject* ToThisObjectImpl(JSStackFrame* exec, JSObject* obj);

		public:
			const UTF8String& GetValue() const;
			mo_unsigned GetLength() const { return mLength; }
			JSString* GetChar(mo_unsigned index);

			mo_bool ToBoolean(JSStackFrame* exec) const;
			mo_double ToNumber(JSStackFrame* exec) const;
			JSObject* ToObject(JSStackFrame* exec) const;
			JSValue ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const;

		private:
			JSString(JSGlobalObject* globalObject);
			JSString(JSGlobalObject* globalObject, const UTF8String& value);

		private:
			mo_bool IsRope() const { return mValue.IsNull(); }

			mo_unsigned mLength;
			mutable UTF8String mValue;
	};

	mo_inline JSString* jsEmptyString(JSGlobalObject* globalObject)
	{
		return JSString::CreateEmpty(globalObject);
	}

	mo_inline JSString* jsEmptyString(JSStackFrame* exec)
	{
		return jsEmptyString(exec->GetGlobalObject());
	}

	mo_inline JSString* jsString(JSGlobalObject* globalObject, const UTF8String& str)
	{
		return JSString::FromString(globalObject, str);
	}

	mo_inline JSString* jsString(JSStackFrame* exec, const UTF8String& str)
	{
		return jsString(exec->GetGlobalObject(), str);
	}
}

#endif