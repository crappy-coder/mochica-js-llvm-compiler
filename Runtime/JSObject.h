#ifndef MOENJIN_RUNTIME_JSOBJECT_H
#define MOENJIN_RUNTIME_JSOBJECT_H

#include "Platform/Configuration.h"
#include "Runtime/JSClassInfo.h"
#include "Runtime/JSObjectType.h"
#include "Runtime/JSTypePreference.h"

namespace MoEnjin
{
	class JSGlobalObject;
	class JSPropertyMap;
	class JSPropertyNameList;
	class JSStackFrame;
	class JSString;

	#define JS_OBJECT_OVERRIDES_FUNCTION(obj, funcName) (obj->GetClassTable()->funcName != JSObject::funcName)

	class JSObject
	{
		friend class JSValue;

		public:
			JS_CLASS();

		public:
			static UTF8String GetClassNameImpl(const JSObject* obj);
			static mo_bool IsExtensibleImpl(const JSObject* obj);
			static mo_bool IsCallableImpl();
			static mo_bool ImplementsHasInstanceImpl();
			static void GetOwnPropertyNamesImpl(JSStackFrame* exec, JSObject* obj, JSPropertyNameList& namesList, mo_bool includeDontEnumProps);
			static void GetPropertyNamesImpl(JSStackFrame* exec, JSObject* obj, JSPropertyNameList& namesList, mo_bool includeDontEnumProps);
			static mo_bool GetOwnPropertyDescriptorImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			static mo_bool GetOwnPropertySlotImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertySlot& slot);
			static mo_bool GetOwnPropertySlotByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSPropertySlot& slot);
			static void PutImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_unsigned attributes);
			static void PutPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot);
			static void PutPropertyByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, JSValue value, mo_bool throwException);
			static mo_bool DeletePropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, mo_bool throwException, mo_bool force = mo_false);
			static mo_bool DeletePropertyByIndexImpl(JSStackFrame* exec, JSObject* obj, mo_unsigned propertyName, mo_bool throwException, mo_bool force = mo_false);
			static mo_bool DefineOwnPropertyImpl(JSStackFrame* exec, JSObject* obj, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException);
			static JSValue GetDefaultValueImpl(JSStackFrame* exec, const JSObject* obj, JSTypePreference typeHint);
			static JSObject* ToThisObjectImpl(JSStackFrame* exec, JSObject* obj);
			static mo_bool HasInstanceImpl(JSStackFrame* exec, JSObject* obj, JSValue value, JSValue prototype);
			static JSCallInfo GetCallInfoImpl(JSStackFrame* exec, JSObject* obj);
			static JSCallInfo GetConstructInfoImpl(JSStackFrame* exec, JSObject* obj);

			static JSObject* Create(JSStackFrame* exec);
			static JSObject* Create(JSStackFrame* exec, JSValue prototype);

		public:
			JSObject(JSGlobalObject* globalObject);
			JSObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype);
			JSObject(JSGlobalObject* globalObject, JSObjectType type, JSValue prototype, const JSClassInfo* classInfo);

			const JSObjectType GetType() const { return mType; }
			const JSClassInfo* GetClassInfo() const { return mClassInfo; }
			const JSClassFunctionTable* GetClassTable() const;

			UTF8String GetClassName() const;
			JSPropertyMap* GetProperties() const { return mProperties; }
			JSValue GetPrototype() const { return mPrototype; }
			JSGlobalObject* GetGlobalObject() const { MO_ASSERT(!IsGlobalObject() || ((JSObject*)mGlobalObject == this)); return mGlobalObject; }

			void SetIsExtensible(mo_bool value) { mIsExtensible = value; }
			void SetPrototype(JSValue prototype) { mPrototype = prototype; }
			void SetGlobalObject(JSGlobalObject* globalObject) { mGlobalObject = globalObject; }

			mo_bool HasPrototype() const { return !mPrototype.IsEmpty() && !mPrototype.IsNull(); }

			mo_bool IsExtensible() const { return mIsExtensible; }
			mo_bool IsEnvironmentObject() const;
			mo_bool IsGlobalObject() const;
			mo_bool IsString() const;
			mo_bool IsObject() const;
			mo_bool IsFunction() const;
			mo_bool IsGetterOrSetter() const;

			mo_bool IsCallable() const;
			mo_bool ImplementsHasInstance() const;

			mo_bool IsSubClassOf(const JSClassInfo* info) const;
			mo_bool IsPropertyEnumerable(const UTF8String& propertyName) const;

			UTF8String ResolveFunctionName(JSStackFrame* exec);

			mo_bool DefineOwnProperty(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor, mo_bool throwException);

			JSCallInfo GetCallInfo(JSStackFrame* exec);
			JSCallInfo GetConstructInfo(JSStackFrame* exec);

			JSValue GetDefaultValue(JSStackFrame* exec, JSTypePreference typeHint);

			JSValue GetValue(const UTF8String& propertyName) const;

			JSValue Get(JSStackFrame* exec, const UTF8String& propertyName) const;
			JSValue Get(JSStackFrame* exec, mo_unsigned propertyName) const;

			void GetOwnPropertyNames(JSStackFrame* exec, JSPropertyNameList& namesList, mo_bool includeDontEnumProps);
			void GetPropertyNames(JSStackFrame* exec, JSPropertyNameList& namesList, mo_bool includeDontEnumProps);

			mo_bool GetPropertySlot(JSStackFrame* exec, const UTF8String& propertyName, JSPropertySlot& slot);
			mo_bool GetPropertySlot(JSStackFrame* exec, mo_unsigned propertyName, JSPropertySlot& slot);
			mo_bool GetPropertyDescriptor(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);

			mo_bool GetOwnPropertyDescriptor(JSStackFrame* exec, const UTF8String& propertyName, JSPropertyDescriptor& descriptor);
			mo_bool GetOwnPropertySlot(JSStackFrame* exec, const UTF8String& propertyName, JSPropertySlot& slot);
			mo_bool GetOwnPropertySlotByIndex(JSStackFrame* exec, mo_unsigned propertyName, JSPropertySlot& slot);

			void PutProperty(const UTF8String& propertyName, JSValue value, mo_unsigned attributes = 0);
			void PutProperty(const UTF8String& propertyName, JSValue value, JSPutPropertySlot& slot);
			void PutPropertyAccessor(const UTF8String& propertyName, JSValue value, mo_unsigned attributes = 0);

			void Put(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_unsigned attributes);
			void PutProperty(JSStackFrame* exec, const UTF8String& propertyName, JSValue value, mo_bool throwException, JSPutPropertySlot& slot);
			void PutPropertyByIndex(JSStackFrame* exec, mo_unsigned propertyName, JSValue value, mo_bool throwException);

			mo_bool DeleteProperty(JSStackFrame* exec, const UTF8String& propertyName, mo_bool throwException, mo_bool force = mo_false);
			mo_bool DeletePropertyByIndex(JSStackFrame* exec, mo_unsigned propertyName, mo_bool throwException, mo_bool force = mo_false);

			mo_bool HasProperty(JSStackFrame* exec, const UTF8String& propertyName) const;
			mo_bool HasProperty(JSStackFrame* exec, mo_unsigned propertyName) const;
			mo_bool HasOwnProperty(JSStackFrame* exec, const UTF8String& propertyName) const;

			mo_bool HasInstance(JSStackFrame* exec, JSValue value, JSValue prototype);

			mo_bool ToBoolean(JSStackFrame* exec) const;
			mo_double ToNumber(JSStackFrame* exec) const;
			JSString* ToString(JSStackFrame* exec) const;
			JSObject* ToThisObject(JSStackFrame* exec);
			JSObject* ToObject(JSStackFrame* exec);

			JSValue ToPrimitive(JSStackFrame* exec, JSTypePreference preference = JSTypeNoPreference) const;
			JSValue ToPrimitiveNumber(JSStackFrame* exec, mo_double& number) const;

		private:
			mo_bool DeleteProperty(const UTF8String& propertyName);
			mo_bool DeleteProperty(mo_unsigned propertyName);
			mo_bool PutProperty(const UTF8String& propertyName, JSValue value, mo_unsigned attributes, JSPutPropertySlot& slot, mo_bool honorAttributes = mo_true);
			
			void Initialize(JSObjectType type);

			const JSClassInfo* mClassInfo;
			JSObjectType mType;
			JSGlobalObject* mGlobalObject;
			JSPropertyMap* mProperties;
			JSValue mPrototype;
			mo_bool mIsExtensible;
	};

	typedef JSObject JSNativeObject;
}

#endif