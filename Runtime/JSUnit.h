#ifndef MOENJIN_RUNTIME_JSUNIT_H
#define MOENJIN_RUNTIME_JSUNIT_H

#include "Platform/Configuration.h"
#include "Runtime/JSClassInfo.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSTypePreference.h"
#include "Runtime/JSValue.h"
#include "Runtime/JSValueWrapper.h"
#include "Text/UTF8String.h"

namespace MoEnjin
{
	class JSObject;
	class JSUnit
	{
		friend class JSValue;

		public:
			static UTF8String GetClassNameImpl(const JSObject* obj);
			static mo_bool IsExtensibleImpl(const JSObject* obj);
			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSPropertySlot& slot);
			static mo_bool GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, JSPropertySlot& slot);
			static void PutImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSValue value, mo_unsigned attributes);
			static void PutPropertyImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot);
			static void PutPropertyByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot);
			static mo_bool DeletePropertyImpl(JSStackFrame* exec, JSUnit* unit, const UTF8String& propertyName, mo_bool throwException);
			static mo_bool DeletePropertyByIndexImpl(JSStackFrame* exec, JSUnit* unit, mo_unsigned propertyName, mo_bool throwException);
			static mo_bool DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException);
			static JSValue GetDefaultValueImpl(JSStackFrame* exec, const JSObject* obj, JSTypePreference typeHint);
			static JSObject* ToThisObjectImpl(JSStackFrame* exec, JSUnit* unit);
			static mo_bool HasInstanceImpl(JSStackFrame* exec, JSObject* obj, JSValue value, JSValue prototype);
			static NativeFunction GetCallFunctionImpl(JSUnit* unit);
			static NativeFunction GetConstructFunctionImpl(JSUnit* unit);

		public:
			JSUnit(JSObject* prototype, const JSClassInfo* classInfo);

			JSObject* GetPrototype() const { return mPrototype; }
			void SetPrototype(JSObject* prototype) { mPrototype = prototype; }

			mo_bool IsSubClassOf(const JSClassInfo* info) const;
			mo_bool IsEnvironmentObject() const;

			const JSClassInfo* GetClassInfo() const { return mClassInfo; }
			const JSClassFunctionTable* GetClassTable() const;

			mo_bool ToBoolean(JSStackFrame* exec) const;
			mo_double ToNumber(JSStackFrame* exec) const;
			mo_bool ToString(JSStackFrame* exec, UTF8String& str) const;
			UTF8String ToString(JSStackFrame* exec) const;
			JSObject* ToObject(JSStackFrame* exec) const;
			JSObject* ToObjectRef(JSStackFrame* exec) const;
			JSValue ToPrimitive(JSStackFrame* exec, JSTypePreference preference) const;
			JSValue ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const;

			mo_bool IsString() const;
			mo_bool IsObject() const;
			mo_bool IsGetterOrSetter() const;

		private:
			const JSClassInfo* mClassInfo;
			JSObject* mPrototype;
	};
}

#endif