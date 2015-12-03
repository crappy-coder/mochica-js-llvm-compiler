#ifndef MOENJIN_RUNTIME_JSFUNCTIONCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSFUNCTIONCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSFunctionPrototype;
	class JSGlobalObject;
	class JSStackFrame;

	class JSFunctionConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSFunctionConstructor* Create(JSStackFrame* exec);

		private:
			JSFunctionConstructor(JSGlobalObject* globalObject);
	};
}

#endif