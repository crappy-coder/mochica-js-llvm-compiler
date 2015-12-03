#ifndef MOENJIN_RUNTIME_JSVALUEWRAPPER_H
#define MOENJIN_RUNTIME_JSVALUEWRAPPER_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSObject;

	typedef enum { } JSAnyValue;
	enum IsNullableTag { IsNullable };

	template<class T> class JSBaseValueWrapper;
	template<> class JSBaseValueWrapper<JSValue>;

	template<typename T> 
	class JSBaseValueWrapper
	{
		public:
			typedef T* (JSBaseValueWrapper::*TBoolType);

		public:
			void Reset() { mBase = mo_null; }

			void Set(T* value) { MO_ASSERT(value); SetNullable(value); }
			void SetNullable(T* value) { mBase = mo_cast_type<JSObject*>(value); }

			T* Get() const { return mo_cast_type<T*>(mBase); }

			T* operator	 *() const { MO_ASSERT(mBase); return mo_cast<T*>(mBase); }
			T* operator	->() const { MO_ASSERT(mBase); return mo_cast<T*>(mBase); }

			operator TBoolType*() const { return (mBase ? mo_cast_type<TBoolType*>(1) : 0); }
			mo_bool operator !() const { return !mBase; }

		private:
			JSObject* mBase;
	};

	template <> class 
	JSBaseValueWrapper<JSAnyValue>
	{
		public:
			typedef JSValue (JSBaseValueWrapper::*TBoolType);

		public:
			mo_bool IsNumber() const { return Get().IsNumber(); }
			mo_bool IsObject() const { return Get().IsObject(); }
			mo_bool IsGetterSetter() const { return Get().IsGetterOrSetter(); }
			mo_bool IsNull() const { return Get().IsNull(); }
			mo_bool IsUndefined() const { return Get().IsUndefined(); }

			void Reset() { mValue = JSValue::Encode(JSValue()); }

			void Set(JSValue value) { mValue = JSValue::Encode(value); }
			void SetUndefined() { mValue = JSValue::MakeUndefined(); }

			JSValue Get() const { return JSValue::Decode(mValue); }

			operator TBoolType*() const { return (Get() ? mo_cast_type<TBoolType*>(1) : 0); }
			mo_bool operator !() const { return !Get(); }

		private:
			JSValueImpl mValue;
	};

	template <typename T>
	class JSValueWrapper : public JSBaseValueWrapper<T>
	{
		public:
			JSValueWrapper() { SetNullable(mo_null); }
			JSValueWrapper(T* value) { Set(value); }
			JSValueWrapper(T* value, IsNullableTag) { SetNullable(value); }
	};

	template <> class JSValueWrapper<JSAnyValue> : public JSBaseValueWrapper<JSAnyValue>
	{
		public:
			JSValueWrapper() { Set(JSValue()); }
			JSValueWrapper(JSValue value) { Set(value); }
	};

	template <typename A, typename B> mo_inline bool operator == (const JSBaseValueWrapper<A> &lhs, const JSBaseValueWrapper<B> &rhs)
	{
		return (lhs.Get() == rhs.Get());
	}
}

#endif