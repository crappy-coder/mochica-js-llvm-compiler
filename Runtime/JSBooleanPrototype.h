#ifndef MOENJIN_RUNTIME_JSBOOLEANPROTOTYPE_H
#define MOENJIN_RUNTIME_JSBOOLEANPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSBooleanInstance.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSBooleanPrototype : public JSBooleanInstance
	{
		public:
			JS_CLASS();

			static JSBooleanPrototype* Create(JSGlobalObject* globalObject, JSValue prototype);

		private:
			JSBooleanPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif