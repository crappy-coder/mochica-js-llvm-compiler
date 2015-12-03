#ifndef MOENJIN_RUNTIME_JSCONVERT_H
#define MOENJIN_RUNTIME_JSCONVERT_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSConstructorFunction;
	class JSFunction;
	class JSGetterSetter;
	class JSObject;
	class JSConvert
	{
		MO_NON_COPYABLE_CLASS(JSConvert);

		public:
			static const mo_double ToNumber(const UTF8String& str);
			static const mo_int32 ToInt32(const mo_double value);
			static const mo_uint32 ToUInt32(const mo_double value);
			

			static JSConstructorFunction* ToConstructorFunction(JSValue value);
			static JSFunction* ToFunction(JSValue value);
			static JSGetterSetter* ToGetterSetter(JSValue value);
			static JSObject* ToNativeObject(JSValue value);

			template <typename To, typename From>
			static mo_inline To Cast(From* from)
			{
				MO_ASSERT(!from || from->JSObject::IsSubClassOf(&RemovePtr<To>::T::ClassInfo));

				return mo_cast<To>(from);
			}

			template <typename To>
			static mo_inline To Cast(JSValue from)
			{
				MO_ASSERT(from.IsNativeObject() && from.GetAsNativeObject()->JSObject::IsSubClassOf(&RemovePtr<To>::T::ClassInfo));

				return mo_cast<To>(from.GetAsNativeObject());
			}

			template <typename To, typename From>
			static mo_inline To NullableCast(From* from)
			{
				return (from->IsSubClassOf(&RemovePtr<To>::T::ClassInfo) ? mo_cast<To>(from) : mo_null);
			}

			template <typename To>
			static mo_inline To NullableCast(JSValue from)
			{
				return (from.IsNativeObject() && from.GetAsNativeObject()->IsSubClassOf(&RemovePtr<To>::T::ClassInfo) ? mo_cast<To>(from.GetAsNativeObject()) : mo_null);
			}

		private:
			JSConvert() { }
	};
}

#endif