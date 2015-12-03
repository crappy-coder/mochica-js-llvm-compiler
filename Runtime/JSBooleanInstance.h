#ifndef MOENJIN_RUNTIME_JSBOOLEANINSTANCE_H
#define MOENJIN_RUNTIME_JSBOOLEANINSTANCE_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSBooleanInstance : public JSObject
	{
		public:
			JS_CLASS();

			static JSBooleanInstance* Create(JSStackFrame* exec);
			static JSBooleanInstance* Create(JSStackFrame* exec, JSValue value);

		public:
			JSValue GetValue() const { return mValue; }

		protected:
			JSBooleanInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype);
			JSBooleanInstance(JSGlobalObject* globalObject, JSValue value, JSValue prototype, const JSClassInfo* classInfo);

		private:
			JSValue mValue;
	};
}

#endif