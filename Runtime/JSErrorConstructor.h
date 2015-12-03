#ifndef MOENJIN_RUNTIME_JSERRORCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSERRORCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSErrorConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSErrorConstructor* Create(JSStackFrame* exec);

		private:
			JSErrorConstructor(JSGlobalObject* globalObject);
	};
}

#endif