#ifndef MOENJIN_RUNTIME_JSNUMBERINSTANCE_H
#define MOENJIN_RUNTIME_JSNUMBERINSTANCE_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSNumberInstance : public JSObject
	{
		public:
			JS_CLASS();

			static JSNumberInstance* Create(JSStackFrame* exec);
			static JSNumberInstance* Create(JSStackFrame* exec, JSValue value);

		public:
			JSValue GetValue() const { return mValue; }

		protected:
			JSNumberInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype);
			JSNumberInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype, const JSClassInfo* classInfo);

		private:
			JSValue mValue;
	};
}

#endif