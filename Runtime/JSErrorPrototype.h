#ifndef MOENJIN_RUNTIME_JSERRORPROTOTYPE_H
#define MOENJIN_RUNTIME_JSERRORPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSErrorInstance.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSErrorPrototype : public JSErrorInstance
	{
		public:
			JS_CLASS();

			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);

			static JSErrorPrototype* Create(JSGlobalObject* globalObject, JSValue prototype);

		protected:
			JSErrorPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif