#ifndef MOENJIN_RUNTIME_JSERRORINSTANCE_H
#define MOENJIN_RUNTIME_JSERRORINSTANCE_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSStackFrame;

	class JSErrorInstance : public JSObject
	{
		public:
			JS_CLASS();

			static JSErrorInstance* Create(JSStackFrame* exec, JSValue message);
			static JSErrorInstance* Create(JSStackFrame* exec, JSValue prototype, JSValue message);

			static JSErrorInstance* Create(JSStackFrame* exec, const UTF8String& message);
			static JSErrorInstance* Create(JSStackFrame* exec, JSValue prototype, const UTF8String& message);

		public:
			mo_bool GetShowSourceInMessage() const { return mShowSourceInMessage; }
			void SetShowSourceInMessage(mo_bool value) { mShowSourceInMessage = value; }

		protected:
			JSErrorInstance(JSGlobalObject* globalObject, JSValue prototype);
			JSErrorInstance(JSGlobalObject* globalObject, JSValue prototype, const JSClassInfo* classInfo);

		private:
			mo_bool mShowSourceInMessage;
	};
}

#endif