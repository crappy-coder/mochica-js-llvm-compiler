#ifndef MOENJIN_RUNTIME_JSGETTERSETTER_H
#define MOENJIN_RUNTIME_JSGETTERSETTER_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	class JSGetterSetter : public JSObject
	{
		public:
			JS_CLASS();

		public:
			JSGetterSetter(JSGlobalObject* globalObject);

			JSObject* GetGetter() const { return mGetter; }
			JSObject* GetSetter() const { return mSetter; }

			void SetGetter(JSObject* getter) { mGetter = getter; }
			void SetSetter(JSObject* setter) { mSetter = setter; }

		private:
			JSObject* mGetter;
			JSObject* mSetter;
	};
}

#endif