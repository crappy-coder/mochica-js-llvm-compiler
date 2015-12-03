#ifndef MOENJIN_RUNTIME_JSOBJECTCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSOBJECTCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSObjectPrototype;
	class JSStackFrame;

	class JSObjectConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSObjectConstructor* Create(JSStackFrame* exec);

		private:
			JSObjectConstructor(JSGlobalObject* globalObject);
	};
}

#endif