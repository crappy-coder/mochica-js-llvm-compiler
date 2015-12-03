#ifndef MOENJIN_RUNTIME_JSBOOLEANCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSBOOLEANCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSBooleanPrototype;
	class JSStackFrame;

	class JSBooleanConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSBooleanConstructor* Create(JSStackFrame* exec);

		private:
			JSBooleanConstructor(JSGlobalObject* globalObject);
	};
}

#endif