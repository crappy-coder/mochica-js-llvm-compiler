#ifndef MOENJIN_RUNTIME_JSNATIVEERRORCONSTRUCTOR_H
#define MOENJIN_RUNTIME_JSNATIVEERRORCONSTRUCTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSConstructorFunction.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSNativeErrorConstructor : public JSConstructorFunction
	{
		public:
			JS_CLASS();

			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSValueImpl Construct(JSStackFrame*);
			static JSValueImpl Call(JSStackFrame*);

			static JSNativeErrorConstructor* Create(JSStackFrame* exec, const UTF8String& name);

		public:
			JSValue GetErrorPrototype(JSStackFrame* exec) const;

		private:
			JSNativeErrorConstructor(JSGlobalObject* globalObject, const UTF8String& name);
	};
}

#endif