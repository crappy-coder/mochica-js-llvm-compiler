#ifndef MOENJIN_RUNTIME_JSSTRINGPROTOTYPE_H
#define MOENJIN_RUNTIME_JSSTRINGPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSStringInstance.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSStringPrototype : public JSStringInstance
	{
		public:
			JS_CLASS();

			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);

			static JSStringPrototype* Create(JSGlobalObject* globalObject, JSValue prototype);

		private:
			JSStringPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif