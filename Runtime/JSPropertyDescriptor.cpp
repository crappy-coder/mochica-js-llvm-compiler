#include "Runtime/JSPropertyDescriptor.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSGetterSetter.h"
#include "Runtime/JSObject.h"
#include "Runtime/JSPropertyAttributes.h"

namespace MoEnjin
{
	const mo_unsigned JSPropertyDescriptor::DEFAULT_ATTRIBUTES = (JSPropertyAttributeDontDelete << 1) - 1;

	JSPropertyDescriptor JSPropertyDescriptor::Make(JSValue value, mo_unsigned attributes)
	{
		JSPropertyDescriptor desc;
		desc.Set(value, attributes);
		return desc;
	}

	JSPropertyDescriptor::JSPropertyDescriptor()
		: mAttributes(DEFAULT_ATTRIBUTES)
		, mAttributesSet(0) { }

	mo_unsigned JSPropertyDescriptor::GetOverridingAttributes(const JSPropertyDescriptor& current) const
	{
		mo_unsigned attrs = current.mAttributes;
		mo_unsigned overrideMask = 0;

		if(IsData() && current.IsAccessor())
			attrs |= JSPropertyAttributeReadOnly;

		if(IsWritableSet())
			overrideMask |= JSPropertyAttributeReadOnly;

		if(IsEnumerableSet())
			overrideMask |= JSPropertyAttributeDontEnum;

		if(IsConfigurableSet())
			overrideMask |= JSPropertyAttributeDontDelete;

		if(IsAccessor())
			overrideMask |= JSPropertyAttributeAccessor;

		return (mAttributes & overrideMask) | (attrs & ~overrideMask);
	}

	mo_bool JSPropertyDescriptor::IsEqualTo(const JSPropertyDescriptor& other) const
	{
		if(!other.mValue == mValue || !other.mGetter == mGetter || !other.mSetter == mSetter)
			return mo_false;

		return  (!mValue  || JSValue::AreSameValue(other.mValue, mValue)) && 
				(!mGetter || JSValue::AreStrictEqual(other.mGetter, mGetter)) &&
				(!mSetter || JSValue::AreStrictEqual(other.mSetter, mSetter)) && HasEqualAttributes(other);
	}

	mo_bool JSPropertyDescriptor::HasEqualAttributes(const JSPropertyDescriptor& other) const
	{
		mo_unsigned mismatch = other.mAttributes ^ mAttributes;
		mo_unsigned sharedSeen = other.mAttributesSet & mAttributesSet;

		if(sharedSeen & WritableSet && mismatch & JSPropertyAttributeReadOnly)
			return mo_false;

		if(sharedSeen & ConfigurableSet && mismatch & JSPropertyAttributeDontDelete)
			return mo_false;

		if(sharedSeen & EnumerableSet && mismatch & JSPropertyAttributeDontEnum)
			return mo_false;

		return true;
	}

	mo_bool JSPropertyDescriptor::IsWritable() const
	{
		MO_ASSERT(!IsAccessor());

		return !(mAttributes & JSPropertyAttributeReadOnly);
	}

	mo_bool JSPropertyDescriptor::IsEnumerable() const
	{
		return !(mAttributes & JSPropertyAttributeDontEnum);
	}

	mo_bool JSPropertyDescriptor::IsConfigurable() const
	{
		return !(mAttributes & JSPropertyAttributeDontDelete);
	}

	mo_bool JSPropertyDescriptor::IsData() const
	{
		return (mValue || (mAttributesSet & WritableSet));
	}

	mo_bool JSPropertyDescriptor::IsAccessor() const
	{
		return (mGetter || mSetter);
	}

	mo_bool JSPropertyDescriptor::IsGeneric() const
	{
		return (!IsAccessor() && !IsData());
	}

	JSObject* JSPropertyDescriptor::GetGetter() const
	{
		MO_ASSERT(IsAccessor() && HasGetter());

		return (mGetter.IsObject() ? JSConvert::ToNativeObject(mGetter) : mo_null);
	}

	JSValue JSPropertyDescriptor::GetGetterValue() const
	{
		MO_ASSERT(IsAccessor());

		return mGetter;
	}

	JSObject* JSPropertyDescriptor::GetSetter() const
	{
		MO_ASSERT(IsAccessor() && HasSetter());

		return (mSetter.IsObject() ? JSConvert::ToNativeObject(mSetter) : mo_null);
	}

	JSValue JSPropertyDescriptor::GetSetterValue() const	
	{
		MO_ASSERT(IsAccessor());

		return mSetter;
	}

	void JSPropertyDescriptor::SetUndefined()
	{
		mValue = JSValue::MakeUndefined();
		mAttributes = (JSPropertyAttributeReadOnly | JSPropertyAttributeDontDelete | JSPropertyAttributeDontEnum);
	}

	void JSPropertyDescriptor::Set(JSValue value, mo_unsigned attributes)
	{
		MO_ASSERT(value);
		MO_ASSERT(value.IsGetterOrSetter() == !!(attributes & JSPropertyAttributeAccessor));

		if(value.IsGetterOrSetter())
			SetAccessor(JSConvert::ToGetterSetter(value), attributes);
		else
		{
			mValue = value;
			mAttributes = attributes;
			mAttributesSet = (EnumerableSet | ConfigurableSet | WritableSet);
		}
	}

	void JSPropertyDescriptor::SetAccessor(JSGetterSetter* accessor, mo_unsigned attributes)
	{
		MO_ASSERT(attributes & JSPropertyAttributeAccessor);

		attributes &= ~JSPropertyAttributeReadOnly;

		mAttributes = attributes;
		mAttributesSet = (EnumerableSet | ConfigurableSet);

		mGetter = (accessor->GetGetter() ? accessor->GetGetter() : JSValue::MakeUndefined());
		mSetter = (accessor->GetSetter() ? accessor->GetSetter() : JSValue::MakeUndefined());
	}

	void JSPropertyDescriptor::SetGetterValue(JSValue getter)
	{
		mGetter = getter;
		mAttributes |=  JSPropertyAttributeAccessor;
		mAttributes &= ~JSPropertyAttributeReadOnly;
	}

	void JSPropertyDescriptor::SetSetterValue(JSValue setter)
	{
		mSetter = setter;
		mAttributes |=  JSPropertyAttributeAccessor;
		mAttributes &= ~JSPropertyAttributeReadOnly;
	}

	void JSPropertyDescriptor::SetWritable(mo_bool value)
	{
		mAttributes = value ? 
			(mAttributes & ~JSPropertyAttributeReadOnly) : 
			(mAttributes |  JSPropertyAttributeReadOnly);

		mAttributesSet |= WritableSet;
	}

	void JSPropertyDescriptor::SetEnumerable(mo_bool value)
	{
		mAttributes = value ? 
			(mAttributes & ~JSPropertyAttributeDontEnum) : 
			(mAttributes |  JSPropertyAttributeDontEnum);

		mAttributesSet |= EnumerableSet;
	}

	void JSPropertyDescriptor::SetConfigurable(mo_bool value)
	{
		mAttributes = value ? 
			(mAttributes & ~JSPropertyAttributeDontDelete) : 
			(mAttributes |  JSPropertyAttributeDontDelete);

		mAttributesSet |= ConfigurableSet;
	}
}