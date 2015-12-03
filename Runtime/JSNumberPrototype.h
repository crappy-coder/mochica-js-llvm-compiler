#ifndef MOENJIN_RUNTIME_JSNUMBERPROTOTYPE_H
#define MOENJIN_RUNTIME_JSNUMBERPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSNumberInstance.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSNumberPrototype : public JSNumberInstance
	{
		public:
			JS_CLASS();

			static JSNumberPrototype* Create(JSGlobalObject* globalObject, JSValue prototype);

		private:
			JSNumberPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif