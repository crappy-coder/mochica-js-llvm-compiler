#ifndef MOENJIN_RUNTIME_JSFUNCTIONPROTOTYPE_H
#define MOENJIN_RUNTIME_JSFUNCTIONPROTOTYPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSFunctionPrototype : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSFunctionPrototype* Create(JSGlobalObject* globalObject, JSValue prototype);

		private:
			JSFunctionPrototype(JSGlobalObject* globalObject, JSValue prototype);
	};
}

#endif