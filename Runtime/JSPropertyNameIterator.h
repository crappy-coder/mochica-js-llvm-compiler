#ifndef MOENJIN_RUNTIME_JSPROPERTYNAMEITERATOR_H
#define MOENJIN_RUNTIME_JSPROPERTYNAMEITERATOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSObject.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSPropertyNameList;
	class JSStackFrame;
	class JSString;
	class JSPropertyNameIterator : public JSObject
	{
		public:
			JS_CLASS();

		public:
			static JSPropertyNameIterator* Create(JSStackFrame* exec, JSObject* sourceObj);

		public:
			mo_bool IsDone() const { return (mCurrentIndex == mNamesLen); }

			JSObject* GetObject() const { return mObject; }

			mo_uint32 GetCurrentIndex() const { return mCurrentIndex; }
			mo_uint32 GetCount() const { return mNamesLen; }

			JSValue GetCurrent(JSStackFrame* exec) { return Get(exec, mCurrentIndex); }
			JSValue Get(JSStackFrame* exec, mo_uint32 i);

			void MoveNext();
			void Reset() { mCurrentIndex = 0; }

		private:
			JSPropertyNameIterator(JSStackFrame* exec, JSObject* sourceObj);

			void InitializePropertyNames(JSStackFrame* exec);

		private:
			JSObject* mObject;
			JSString** mNames;
			mo_uint32 mNamesLen;
			mo_uint32 mCurrentIndex;
	};
}

#endif