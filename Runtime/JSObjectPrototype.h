#ifndef MOENJIN_RUNTIME_JSOBJECTPROTOTYPE_H
#define MOENJIN_RUNTIME_JSOBJECTPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSObjectPrototype : public JSObject
	{
		public:
			JS_CLASS();

			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);
			static mo_bool GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot);
			static mo_bool DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException);
			static void PutPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot);

			static JSObjectPrototype* Create(JSGlobalObject* globalObject);

		private:
			JSObjectPrototype(JSGlobalObject* globalObject);

			mo_bool mHasIntegerPropertyNames;
	};
}

#endif