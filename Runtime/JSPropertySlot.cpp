#include "Runtime/JSPropertySlot.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSStackFrame.h"

namespace MoEnjin
{
	JSPropertySlot::JSPropertySlot() 
		: mGetIndexValueFunc(0)
		, mGetValueFunc(0)
		, mType(IsValue) { }

	JSPropertySlot::JSPropertySlot(const JSValue base)
		: mBase(base)
		, mGetIndexValueFunc(0)
		, mGetValueFunc(0)
		, mType(IsValue) { }

	JSValue JSPropertySlot::GetValue(JSStackFrame* exec, const UTF8String& propertyName) const
	{
		if(mType == IsValue)
			return mValue;
		if(mType == IsGetIndexValueFunc)
			return mGetIndexValueFunc(exec, mBase, mData.index);
		if(mType == IsGetter)
			return CallGetter(exec);

		return mGetValueFunc(exec, mBase, propertyName);
	}

	JSValue JSPropertySlot::GetValue(JSStackFrame* exec, mo_unsigned propertyIndex) const
	{
		if(mType == IsValue)
			return mValue;
		if(mType == IsGetIndexValueFunc)
			return mGetIndexValueFunc(exec, mBase, mData.index);
		if(mType == IsGetter)
			return CallGetter(exec);

		return mGetValueFunc(exec, mBase, UTF8String::FromNumber(propertyIndex));
	}

	void JSPropertySlot::SetValue(JSValue value)
	{
		MO_ASSERT(value);

		mType = IsValue;
		mValue = value;
	}

	void JSPropertySlot::SetValue(JSValue base, JSValue value)
	{
		MO_ASSERT(value);

		mType = IsValue;
		mValue = value;
		mBase = base;
	}

	void JSPropertySlot::SetValueFunc(JSValue base, GetValueFunction getValueFunc)
	{
		MO_ASSERT(base);
		MO_ASSERT(getValueFunc);

		mType = IsGetValueFunc;
		mGetValueFunc = getValueFunc;
		mGetIndexValueFunc = mo_null;
		mBase = base;
	}

	void JSPropertySlot::SetIndexValueFunc(JSValue base, GetIndexValueFunction getIndexValueFunc, mo_unsigned index)
	{
		MO_ASSERT(base);
		MO_ASSERT(getIndexValueFunc);

		mType = IsGetIndexValueFunc;
		mGetIndexValueFunc = getIndexValueFunc;
		mBase = base;
		mData.index = index;
	}

	void JSPropertySlot::SetBase(JSValue base)
	{
		MO_ASSERT(mBase);
		MO_ASSERT(base);

		mBase = base;
	}

	void JSPropertySlot::SetGetter(JSObject* getterFunc)
	{
		MO_ASSERT(getterFunc);

		mType = IsGetter;
		mThisValue = mBase;
		mData.getter = getterFunc;
	}

	void JSPropertySlot::SetUndefined()
	{
		SetValue(JSValue::MakeUndefined());
	}

	JSValue JSPropertySlot::CallGetter(JSStackFrame* exec) const
	{
		// REFACTOR
		//NativeFunction func = mData.getter->GetCallFunction();
		//return JSStackFrame::CallFunc(exec, JSConvert::ToObject(mData.getter), func, mThisValue.IsObject() ? mThisValue.ToThisObject() : mThisValue, mo_null);
		return JSValue::MakeUndefined();
	}
}