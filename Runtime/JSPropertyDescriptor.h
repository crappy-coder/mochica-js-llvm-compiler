#ifndef MOENJIN_RUNTIME_JSPROPERTYDESCRIPTOR_H
#define MOENJIN_RUNTIME_JSPROPERTYDESCRIPTOR_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSGetterSetter;
	class JSObject;

	class JSPropertyDescriptor
	{
		enum { WritableSet = 1, EnumerableSet = 2, ConfigurableSet = 4 };

		static const mo_unsigned DEFAULT_ATTRIBUTES;

		public:
			static JSPropertyDescriptor Make(JSValue value, mo_unsigned attributes);

		public:
			JSPropertyDescriptor();

			mo_unsigned GetOverridingAttributes(const JSPropertyDescriptor& current) const;

			mo_bool IsEqualTo(const JSPropertyDescriptor& other) const;
			mo_bool HasEqualAttributes(const JSPropertyDescriptor& other) const;

			mo_bool IsWritable() const;
			mo_bool IsWritableSet() const { return ((mAttributesSet & WritableSet) != 0); }
			
			mo_bool IsEnumerable() const;
			mo_bool IsEnumerableSet() const { return ((mAttributesSet & EnumerableSet) != 0); }
			
			mo_bool IsConfigurable() const;
			mo_bool IsConfigurableSet() const { return ((mAttributesSet & ConfigurableSet) != 0); }

			mo_bool IsData() const;
			mo_bool IsAccessor() const;
			mo_bool IsGeneric() const;

			mo_bool IsEmpty() const { return !(mValue || mGetter || mSetter || mAttributesSet); }

			mo_bool HasAttributes() const { return (mAttributesSet != 0); }
			mo_bool HasValue() const { return !!mValue; }
			mo_bool HasGetter() const { return !!mGetter; }
			mo_bool HasSetter() const { return !!mSetter; }

			mo_unsigned GetAttributes() const { return mAttributes; }
			JSValue GetValue() const { return mValue; }

			JSObject* GetGetter() const;
			JSValue GetGetterValue() const;

			JSObject* GetSetter() const;
			JSValue GetSetterValue() const;
			
			void SetUndefined();
			void Set(JSValue value, mo_unsigned attributes);
			void SetAccessor(JSGetterSetter* accessor, mo_unsigned attributes);
			void SetGetterValue(JSValue getter);
			void SetSetterValue(JSValue setter);
			void SetValue(JSValue value) { mValue = value; }
			void SetWritable(mo_bool value);
			void SetEnumerable(mo_bool value);
			void SetConfigurable(mo_bool value);

		private:
			mo_bool operator==(const JSPropertyDescriptor&) { return mo_false; }

			JSValue mValue;
			JSValue mGetter;
			JSValue mSetter;
			mo_unsigned mAttributes;
			mo_unsigned mAttributesSet;
	};
}

#endif