#include "Runtime/JSPropertyNameIterator.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSPropertyNameList.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSStrings.h"

namespace MoEnjin
{
	JS_DEFINE_AS_EMPTY_CLASS(JSPropertyNameIterator, PropertyNameIterator);

	JSPropertyNameIterator::JSPropertyNameIterator(JSStackFrame* exec, JSObject* sourceObj)
		: JSObject(exec->GetGlobalObject(), JSObjectTypeObject, jsNull(), &ClassInfo) 
		, mObject(sourceObj)
		, mCurrentIndex(0)
		, mNamesLen(0)
		, mNames(mo_null) 
	{
		InitializePropertyNames(exec);
	}

	void JSPropertyNameIterator::InitializePropertyNames(JSStackFrame* exec)
	{
		JSPropertyNameList namesList;
		mObject->GetPropertyNames(exec, namesList, mo_false);

		if(namesList.GetCount() > 0)
		{
			mNamesLen = namesList.GetCount();
			mNames = new JSString*[mNamesLen];

			for(mo_int32 i = 0; i < mNamesLen; ++i)
				mNames[i] = jsString(exec, namesList[i]);
		}
	}

	JSValue JSPropertyNameIterator::Get(JSStackFrame* exec, mo_uint32 i)
	{
		JSValue name = mNames[i];

		// make sure the property still exists on the object
		// before we send it back
		if(mObject->HasProperty(exec, name.ToString(exec)->GetValue()))
			return name;

		return jsEmpty();
	}

	void JSPropertyNameIterator::MoveNext()
	{
		if(++mCurrentIndex < mNamesLen)
			return;

		mCurrentIndex = mNamesLen;
	}

	JSPropertyNameIterator* JSPropertyNameIterator::Create(JSStackFrame* exec, JSObject* sourceObj)
	{
		return new JSPropertyNameIterator(exec, sourceObj);
	}
}