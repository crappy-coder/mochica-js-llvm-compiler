#ifndef MOENJIN_RUNTIME_JSWRAPPEDOBJECT_H
#define MOENJIN_RUNTIME_JSWRAPPEDOBJECT_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSWrappedObject : public JSObject
	{
		public:
			JSValue GetValue() const;
			void SetValue(JSValue value);

		protected:
			JSWrappedObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, JSValue initialValue);
			JSWrappedObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, const JSClassInfo* classInfo, JSValue initialValue);

		private:
			JSValue mValue;
	};
}

#endif