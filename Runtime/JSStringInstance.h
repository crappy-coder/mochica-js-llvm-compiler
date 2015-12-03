#ifndef MOENJIN_RUNTIME_JSSTRINGINSTANCE_H
#define MOENJIN_RUNTIME_JSSTRINGINSTANCE_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;
	class JSString;

	class JSStringInstance : public JSObject
	{
		public:
			JS_CLASS();

			static JSStringInstance* Create(JSStackFrame* exec);
			static JSStringInstance* Create(JSStackFrame* exec, JSString* str);

		public:
			JSString* GetValue() const { return mValue; }

		protected:
			JSStringInstance(JSGlobalObject* globalObject, JSString* str, JSValue prototype);
			JSStringInstance(JSGlobalObject* globalObject, JSString* str, JSValue prototype, const JSClassInfo* classInfo);

		private:
			JSString* mValue;
	};
}

#endif