#ifndef MOENJIN_RUNTIME_JSSTRINGCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSSTRINGCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStringPrototype;
	class JSStackFrame;

	class JSStringConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSStringConstructor* Create(JSStackFrame* exec);

		private:
			JSStringConstructor(JSGlobalObject* globalObject);
	};
}

#endif