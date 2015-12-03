#include "CodeGen/IRConsts.h"

namespace MoEnjin
{
	//
	// Internal IR identifiers should always begin with '__mo', followed by a dot
	// and then the format defined below for each identifier type. 
	// 
	// The placeholder <mo-prefix> denotes the prefix value '__mo'
	//
	// For example:
	//     '<mo-prefix>.var.jsStringName' would be expanded to the identifier of
	//     '__mo.var.jsStringName', which means it is a local variable with a name
	//     as a js string type.
	//

	// Section Names
	//     <mo-prefix>.calls.<name>
	//
	mo_utf8stringptr IRConsts::BuiltinCallsSection = MO_IR_CONST_VALUE_STRING(calls_builtin);
	mo_utf8stringptr IRConsts::StubCallsSection = MO_IR_CONST_VALUE_STRING(calls_stub);


	// 
	// Global Variables / Constants
	//     <mo-prefix>.global.var.<name>
	//
	mo_utf8stringptr IRConsts::JSValueNullName = MO_IR_GLOBAL_VAR_STRING(js_null);
	mo_utf8stringptr IRConsts::JSValueUndefinedName = MO_IR_GLOBAL_VAR_STRING(js_undefined);
	mo_utf8stringptr IRConsts::JSValueTrueName = MO_IR_GLOBAL_VAR_STRING(js_true);
	mo_utf8stringptr IRConsts::JSValueFalseName = MO_IR_GLOBAL_VAR_STRING(js_false);
	mo_utf8stringptr IRConsts::JSValueEmptyName = MO_IR_GLOBAL_VAR_STRING(js_empty);

	//
	// Local Variables
	//     <mo-prefix>.var.<name>
	//
	mo_utf8stringptr IRConsts::JSStackFrameCalleeName = MO_IR_LOCAL_VAR_STRING(jsCalleeStackFrame);
	mo_utf8stringptr IRConsts::JSStackFrameCalleePtrName = MO_IR_LOCAL_VAR_STRING(jsCalleeStackFramePtr);
	mo_utf8stringptr IRConsts::JSStackFrameCallerName = MO_IR_LOCAL_VAR_STRING(jsCallerStackFrame);
	mo_utf8stringptr IRConsts::JSStackFrameCallerPtrName = MO_IR_LOCAL_VAR_STRING(jsCallerStackFramePtr);

	mo_utf8stringptr IRConsts::JSEncodeValuePayloadName = MO_IR_LOCAL_VAR_STRING(payload);
	mo_utf8stringptr IRConsts::JSEncodeValueTagName = MO_IR_LOCAL_VAR_STRING(tag);

	mo_utf8stringptr IRConsts::JSEncodedValueName = MO_IR_LOCAL_VAR_STRING(encoded_value);
	mo_utf8stringptr IRConsts::JSEncodedValuePtrName = MO_IR_LOCAL_VAR_STRING(encoded_value_ptr);
	mo_utf8stringptr IRConsts::JSEncodedValueBitsName = MO_IR_LOCAL_VAR_STRING(encoded_value_bits);
	mo_utf8stringptr IRConsts::JSEncodedValuePayloadName = MO_IR_LOCAL_VAR_STRING(encoded_value_payload);
	mo_utf8stringptr IRConsts::JSEncodedValueTagName = MO_IR_LOCAL_VAR_STRING(encoded_value_tag);

	mo_utf8stringptr IRConsts::NativeCallFrameName = MO_IR_LOCAL_VAR_STRING(nativeStubCallFrame);
	mo_utf8stringptr IRConsts::NativeCallFrameArgsName = MO_IR_LOCAL_VAR_STRING(nativeStubCallFrame_args);
	mo_utf8stringptr IRConsts::NativeCallFrameStackFrameName = MO_IR_LOCAL_VAR_STRING(nativeStubCallFrame_stack_frame);

	mo_utf8stringptr IRConsts::NativeFunctionBinding = MO_IR_LOCAL_VAR_STRING(nativeFunctionBinding);
	mo_utf8stringptr IRConsts::NativeFunctionBinding_Name = MO_IR_LOCAL_VAR_STRING(nativeFunctionBinding_name);
	mo_utf8stringptr IRConsts::NativeFunctionBinding_Function = MO_IR_LOCAL_VAR_STRING(nativeFunctionBinding_function);
	mo_utf8stringptr IRConsts::NativeFunctionBinding_ParameterCount = MO_IR_LOCAL_VAR_STRING(nativeFunctionBinding_parameter_count);
	mo_utf8stringptr IRConsts::NativeFunctionBinding_Strict = MO_IR_LOCAL_VAR_STRING(nativeFunctionBinding_strict);

	mo_utf8stringptr IRConsts::ParameterNameCollection = MO_IR_LOCAL_VAR_STRING(parameter_name_collection);
	mo_utf8stringptr IRConsts::ParameterNameCollection_Values = MO_IR_LOCAL_VAR_STRING(parameter_name_collection_values);
	mo_utf8stringptr IRConsts::ParameterNameCollection_Length = MO_IR_LOCAL_VAR_STRING(parameter_name_collection_length);

	mo_utf8stringptr IRConsts::VariableNameCollection = MO_IR_LOCAL_VAR_STRING(variable_name_collection);
	mo_utf8stringptr IRConsts::VariableNameCollection_Values = MO_IR_LOCAL_VAR_STRING(variable_name_collection_values);
	mo_utf8stringptr IRConsts::VariableNameCollection_Length = MO_IR_LOCAL_VAR_STRING(variable_name_collection_length);

	//
	// Types
	//     <mo-prefix>.type.<lang>.<name|ClassName>
	//
	// where <lang> is either 'ir' if it is used only in IR or 'cpp' if it is passed into
	// or returned from native stub calls and represents a native type.
	//
	// <name|ClassName> is either the name of the 'ir' type, or if 'cpp' then the actual
	// name of the cpp class.
	//
	mo_utf8stringptr IRConsts::NativeFunctionBindingTypeName = MO_IR_CPP_TYPE_STRING(NativeFunctionBinding);
	mo_utf8stringptr IRConsts::NativeStringCollectionTypeName = MO_IR_CPP_TYPE_STRING(NativeStringCollection);
	mo_utf8stringptr IRConsts::NativeExceptionDataTypeName = MO_IR_CPP_TYPE_STRING(NativeExceptionData);
	mo_utf8stringptr IRConsts::NativeContextTypeName = MO_IR_CPP_TYPE_STRING(NativeContext);
	mo_utf8stringptr IRConsts::NativeStringTableTypeName = MO_IR_CPP_TYPE_STRING(NativeStringTable);
	mo_utf8stringptr IRConsts::EnterFunctionCallInitDataTypeName = MO_IR_CPP_TYPE_STRING(EnterFunctionCallInitData);

	//
	// Generated/Builtin Functions
	//     <mo-prefix>.function.<lang>.<name>
	//
	// where <lang> is either 'ir' if it is used only in IR or 'cpp' if it is defined in external
	// cpp code.
	//
	// <name> is either the name of the 'ir' function, or if 'cpp' then the actual
	// name of the cpp function.
	//
	mo_utf8stringptr IRConsts::NativeCallFrameFunctionName = MO_IR_IR_FUNCTION_STRING(InitializeNativeCallFrame);
	mo_utf8stringptr IRConsts::InitializeStringsFunctionName = MO_IR_IR_FUNCTION_STRING(InitializeStrings);
	mo_utf8stringptr IRConsts::JSEncodeValueFunctionName = MO_IR_IR_FUNCTION_STRING(EncodeJSValue);
}