#ifndef MOENJIN_RUNTIME_JSSTUBDEFS_H
#define MOENJIN_RUNTIME_JSSTUBDEFS_H

#include "Runtime/BitOps.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
//#if defined(MO_CPU_X86)
//	#if defined(MO_COMPILER_MSVC)
//	#define NATIVE_STUB_CALL __fastcall
//	#elif defined(MO_COMPILER_GCC)
//	#define NATIVE_STUB_CALL __attribute__ ((fastcall))
//	#else
//	#error "Stub function calls require fastcall calling conventions for x86, need to add directive for compiler!"
//	#endif
//#else
//	#define NATIVE_STUB_CALL
//#endif

	#define RUNTIME_API mo_extern mo_api

	#define RUNTIME_CALL
	#define RUNTIME_CALL_EH 

	#define RUNTIME_FUNCTION(name, rtype) RUNTIME_API rtype RUNTIME_CALL name
	#define RUNTIME_FUNCTION_EH(name, rtype) RUNTIME_API rtype RUNTIME_CALL_EH name

	class JSFunction;
	class JSGlobalObject;
	class JSObject;
	class JSStackFrame;
	class JSNativeExceptionHandler;
	class JSPropertyNameIterator;

	struct NativeFunctionBinding
	{
		const mo_utf8stringptr name;
		NativeFunctionPtr function;
		mo_uint32 parameterCount;
		mo_bool inStrictMode;
	};

	struct ModuleStringTableA
	{
		mo_utf8stringptr data;
	};

	struct ModuleStringTable
	{
		mo_int32 length;
		ModuleStringTableA value;
	};

	struct NativeContext
	{
		JSGlobalObject* global;
		ModuleStringTable* strings;
	};

	struct EnterFunctionCallInitData
	{
		mo_bool isFunctionCode;
		mo_bool isStrictMode;
		mo_uint32 numParameters;
		mo_uint32 numVariables;
		mo_uint32 numFunctions;
		const mo_utf8stringptr* parameters;
		const mo_utf8stringptr* variables;
		const NativeFunctionBinding** functions;
		
	};

	union JSVal
	{
		JSValueImpl asValue;
		JSValueData asValueData;

		mo_int32  GetPayload() const;
		mo_int32& GetPayload();
		mo_int32  GetTag() const;
		mo_int32& GetTag();

		JSValue ToValue() const;
		JSObject* ToObject() const;
		JSFunction* ToFunction() const;

		JSValue operator *() const { return JSValue::Decode(asValue); }
		JSValue operator->() const { return JSValue::Decode(asValue); }
			
		operator JSValue () { return JSValue::Decode(asValue); }
		operator JSValue () const { return JSValue::Decode(asValue); }

		JSVal& operator=(const JSVal& other);
		JSVal& operator=(JSValue other);
	};

	mo_inline JSVal& JSVal::operator=(const JSVal& other)
	{
		asValue = other.asValue;
		return *this;
	}

	mo_inline JSVal& JSVal::operator=(JSValue other)
	{
		asValue = JSValue::Encode(other);
		return *this;
	}

	mo_inline mo_int32 JSVal::GetPayload() const
	{
		return asValueData.asBits.payload;
	}

	mo_inline mo_int32& JSVal::GetPayload()
	{
		return asValueData.asBits.payload;
	}

	mo_inline mo_int32 JSVal::GetTag() const
	{
		return asValueData.asBits.tag;
	}

	mo_inline mo_int32& JSVal::GetTag()
	{
		return asValueData.asBits.tag;
	}

	mo_inline JSValue JSVal::ToValue() const
	{
		return JSValue::Decode(asValue);
	}

	mo_inline JSObject* JSVal::ToObject() const
	{
		return JSValue::Decode(asValue).ToNativeObject();
	}

	mo_inline JSFunction* JSVal::ToFunction() const
	{
		return mo_cast_type<JSFunction*>(JSValue::Decode(asValue).ToNativeObject());
	}
}

#endif