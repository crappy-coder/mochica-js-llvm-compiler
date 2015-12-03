#ifndef MOENJIN_RUNTIME_JSNUMBERCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSNUMBERCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSNumberPrototype;
	class JSStackFrame;

	class JSNumberConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSNumberConstructor* Create(JSStackFrame* exec);

		private:
			JSNumberConstructor(JSGlobalObject* globalObject);
	};
}

#endif