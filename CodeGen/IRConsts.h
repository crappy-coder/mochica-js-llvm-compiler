#ifndef MOENJIN_CODEGEN_IRCONSTS_H
#define MOENJIN_CODEGEN_IRCONSTS_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	#define IR_NAME_SEPARATOR							"_"								// _
	#define IR_NAME_BEGIN(name)							"__mo" IR_NAME_SEPARATOR name	// __mo_name
	#define IR_NAME_END(name)							name							// name
	#define IR_NAME_ENDI(name)							name IR_NAME_SEPARATOR "0"		// name_0
	#define IR_NAME_NEXT(name)							IR_NAME_SEPARATOR name			// _name

	// creates a name that ends with a zero (i.e. __mo_name1_name2_nameN_0), use
	// for names that auto generate a unique number
	#define IR_CREATE_NAMEI(...)							MO_MACRO_DISPATCHER(IR_CREATE_NAMEI, __VA_ARGS__)(__VA_ARGS__)
	#define IR_CREATE_NAMEI1(n1)							IR_NAME_BEGIN(IR_NAME_ENDI(n1))
	#define IR_CREATE_NAMEI2(n1, n2)						IR_NAME_BEGIN(n1) IR_NAME_NEXT(IR_NAME_ENDI(n2))
	#define IR_CREATE_NAMEI3(n1, n2, n3)					IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(IR_NAME_ENDI(n3))
	#define IR_CREATE_NAMEI4(n1, n2, n3, n4)				IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(IR_NAME_ENDI(n4))
	#define IR_CREATE_NAMEI5(n1, n2, n3, n4, n5)			IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(n4) IR_NAME_NEXT(IR_NAME_ENDI(n5))
	#define IR_CREATE_NAMEI6(n1, n2, n3, n4, n5, n6)		IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(n4) IR_NAME_NEXT(n5) IR_NAME_NEXT(IR_NAME_ENDI(n6))

	// creates a name (i.e. __mo_name1_name2_nameN)
	#define IR_CREATE_NAME(...)							MO_MACRO_DISPATCHER(IR_CREATE_NAME, __VA_ARGS__)(__VA_ARGS__)
	#define IR_CREATE_NAME1(n1)							IR_NAME_BEGIN(IR_NAME_END(n1))
	#define IR_CREATE_NAME2(n1, n2)						IR_NAME_BEGIN(n1) IR_NAME_NEXT(IR_NAME_END(n2))
	#define IR_CREATE_NAME3(n1, n2, n3)					IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(IR_NAME_END(n3))
	#define IR_CREATE_NAME4(n1, n2, n3, n4)				IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(IR_NAME_END(n4))
	#define IR_CREATE_NAME5(n1, n2, n3, n4, n5)			IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(n4) IR_NAME_NEXT(IR_NAME_END(n5))
	#define IR_CREATE_NAME6(n1, n2, n3, n4, n5, n6)		IR_NAME_BEGIN(n1) IR_NAME_NEXT(n2) IR_NAME_NEXT(n3) IR_NAME_NEXT(n4) IR_NAME_NEXT(n5) IR_NAME_NEXT(IR_NAME_END(n6))

	#define IR_INTERNAL_GLOBAL_NAME(name)				IR_CREATE_NAME("sys", "global", name)
	#define IR_INTERNAL_VAR_NAME(name)					IR_CREATE_NAMEI("sys", "var", name)
	#define IR_INTERNAL_FUNCTION_NAME(name)				IR_NAME_BEGIN("sys") IR_NAME_NEXT("function") IR_NAME_NEXT(name)
	#define IR_VAR_NAME(name)							IR_CREATE_NAMEI("var", name)			// use for local and globals since they begin with a % and @ already
	#define IR_BLOCK_NAME(name)							IR_NAME_ENDI(name) 
	#define IR_ERROR_NAME(name)							IR_CREATE_NAMEI("ERROR", name)
	#define IR_CALL_RETVAL_NAME(name)					name IR_NAME_ENDI(IR_NAME_NEXT("value"))
	#define IR_TYPE_NAME(name)							IR_CREATE_NAME("type", name)

	#define MO_IR_JS_FUNCTION_PREFIX "__js_function_"
	#define MO_IR_CONST_VALUE_STRING(v) "__mo_" #v

	#define MO_IR_LOCAL_VAR_STRING(name) MO_IR_CONST_VALUE_STRING(var) "_" #name

	#define MO_IR_GLOBAL_VAR_STRING(name) MO_IR_CONST_VALUE_STRING(global_var) "_" #name

	#define MO_IR_STRING_NAME(name) (MO_IR_CONST_VALUE_STRING(strings) "_" + ##name)

	#define MO_IR_ENCODED_STRING_NAME(name) (MO_IR_CONST_VALUE_STRING(encoded_strings) "_" + ##name)

	#define MO_IR_JS_FUNCTION_VAR_NAME(name) (MO_IR_JS_FUNCTION_PREFIX "_" + ##name)

	#define MO_IR_RUNTIME_NAME(name) (MO_IR_CONST_VALUE_STRING(runtime) "_" + ##name)
	#define MO_IR_RUNTIME_STRING(name) MO_IR_CONST_VALUE_STRING(runtime) "_" #name

	#define MO_IR_CPP_TYPE_STRING(className) MO_IR_CONST_VALUE_STRING(type_cpp) "_" #className

	#define MO_IR_IR_FUNCTION_STRING(name) MO_IR_CONST_VALUE_STRING(function_ir) "_" #name

	class IRConsts
	{
		MO_STATIC_CLASS(IRConsts);

		public:
			static const mo_uint64 SET_JMP_BUFFER_SIZE = 16;

			static mo_utf8stringptr JSValueNullName;
			static mo_utf8stringptr JSValueUndefinedName;
			static mo_utf8stringptr JSValueTrueName;
			static mo_utf8stringptr JSValueFalseName;
			static mo_utf8stringptr JSValueEmptyName;

			static mo_utf8stringptr JSEncodeValuePayloadName;
			static mo_utf8stringptr JSEncodeValueTagName;

			static mo_utf8stringptr JSEncodedValueName;
			static mo_utf8stringptr JSEncodedValuePtrName;
			static mo_utf8stringptr JSEncodedValueBitsName;
			static mo_utf8stringptr JSEncodedValuePayloadName;
			static mo_utf8stringptr JSEncodedValueTagName;

			static mo_utf8stringptr JSStackFrameCalleeName;
			static mo_utf8stringptr JSStackFrameCalleePtrName;
			static mo_utf8stringptr JSStackFrameCallerName;
			static mo_utf8stringptr JSStackFrameCallerPtrName;

			static mo_utf8stringptr NativeCallFrameName;
			static mo_utf8stringptr NativeCallFrameArgsName;
			static mo_utf8stringptr NativeCallFrameStackFrameName;

			static mo_utf8stringptr NativeFunctionBinding;
			static mo_utf8stringptr NativeFunctionBinding_Name;
			static mo_utf8stringptr NativeFunctionBinding_Function;
			static mo_utf8stringptr NativeFunctionBinding_ParameterCount;
			static mo_utf8stringptr NativeFunctionBinding_Strict;

			static mo_utf8stringptr ParameterNameCollection;
			static mo_utf8stringptr ParameterNameCollection_Values;
			static mo_utf8stringptr ParameterNameCollection_Length;

			static mo_utf8stringptr VariableNameCollection;
			static mo_utf8stringptr VariableNameCollection_Values;
			static mo_utf8stringptr VariableNameCollection_Length;

			static mo_utf8stringptr NativeFunctionBindingTypeName;
			static mo_utf8stringptr NativeStringCollectionTypeName;
			static mo_utf8stringptr NativeExceptionDataTypeName;
			static mo_utf8stringptr NativeContextTypeName;
			static mo_utf8stringptr NativeStringTableTypeName;
			static mo_utf8stringptr EnterFunctionCallInitDataTypeName;

			static mo_utf8stringptr NativeCallFrameFunctionName;
			static mo_utf8stringptr InitializeStringsFunctionName;
			static mo_utf8stringptr JSEncodeValueFunctionName;

			static mo_utf8stringptr BuiltinCallsSection;
			static mo_utf8stringptr StubCallsSection;
	};
}

#endif