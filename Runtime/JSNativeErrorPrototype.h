#ifndef MOENJIN_RUNTIME_JSNATIVEERRORPROTOTYPE_H
#define MOENJIN_RUNTIME_JSNATIVEERRORPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSErrorPrototype.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSNativeErrorConstructor;
	class JSStackFrame;

	class JSNativeErrorPrototype : public JSErrorPrototype
	{
		public:
			static JSNativeErrorPrototype* Create(JSStackFrame* exec, JSValue prototype, const UTF8String& name, JSNativeErrorConstructor* constructor);

		private:
			JSNativeErrorPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif