#ifndef MOENJIN_RUNTIME_JSCONSTRUCTORFUNCTION_H
#define MOENJIN_RUNTIME_JSCONSTRUCTORFUNCTION_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	class JSStackFrame;

	class JSConstructorFunction : public JSObject
	{
		public:
			JS_CLASS();

			static mo_bool IsCallableImpl();
			static mo_bool ImplementsHasInstanceImpl();
			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);

		public:
			const UTF8String& GetName(JSStackFrame* exec) const;
			const UTF8String& GetDisplayName(JSStackFrame* exec) const;
			const UTF8String GetDebuggerName(JSStackFrame* exec) const;

		protected:
			JSConstructorFunction(JSGlobalObject* globalObject, JSValue prototype, const UTF8String& name);
			JSConstructorFunction(JSGlobalObject* globalObject, JSValue prototype, const JSClassInfo* classInfo, const UTF8String& name);

		private:
			UTF8String mName;
	};
}

#endif