#ifndef MOENJIN_RUNTIME_JSPROPERTYSLOT_H
#define MOENJIN_RUNTIME_JSPROPERTYSLOT_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSStackFrame;
	class JSObject;

	typedef JSValue (*GetValueFunction)(JSStackFrame*, JSValue, const UTF8String&);
	typedef JSValue (*GetIndexValueFunction)(JSStackFrame*, JSValue, mo_unsigned);

	class JSPropertySlot
	{
		enum SlotType { IsValue, IsGetValueFunc, IsGetIndexValueFunc, IsGetter };

		public:
			JSPropertySlot();
			explicit JSPropertySlot(const JSValue base);

			JSValue GetBase() const { return mBase; }
			mo_unsigned GetIndex() const { return mData.index; }
			GetValueFunction GetGetterFunc() const { return mGetValueFunc; }
			GetIndexValueFunction GetIndexGetterFunc() const { return mGetIndexValueFunc; }

			JSValue GetValue(JSStackFrame* exec, const UTF8String& propertyName) const;
			JSValue GetValue(JSStackFrame* exec, mo_unsigned propertyIndex) const;

			void SetValue(JSValue value);
			void SetValue(JSValue base, JSValue value);
			void SetValueFunc(JSValue base, GetValueFunction getValueFunc);
			void SetIndexValueFunc(JSValue base, GetIndexValueFunction getIndexValueFunc, mo_unsigned index);

			void SetBase(JSValue base);
			void SetGetter(JSObject* getterFunc);
			void SetUndefined();

		private:
			JSValue CallGetter(JSStackFrame* exec) const;

			SlotType mType;
			GetValueFunction mGetValueFunc;
			GetIndexValueFunction mGetIndexValueFunc;
			JSValue mBase;
			JSValue mValue;
			JSValue mThisValue;

			union {
				JSObject* getter;
				mo_unsigned index;
			} mData;
	};
}

#endif