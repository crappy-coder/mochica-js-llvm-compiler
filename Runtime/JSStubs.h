#ifndef MOENJIN_RUNTIME_JSSTUBS_H
#define MOENJIN_RUNTIME_JSSTUBS_H

#include "Platform/Configuration.h"
#include "Runtime/JSStubDefs.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	//-------------------------------------------------------------------------------------------------
	// SECTION: STARTUP/SHUTDOWN CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	RUNTIME_FUNCTION(mo_runtime_initialize, void) (NativeContext*);
	RUNTIME_FUNCTION(mo_runtime_shutdown, void) (NativeContext*);
	RUNTIME_FUNCTION(mo_runtime_initialize_environment, JSStackFrame*) (NativeContext*);
	


	//-------------------------------------------------------------------------------------------------
	// SECTION: EXCEPTION CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	RUNTIME_FUNCTION_EH(mo_runtime_exception_throw, void) (JSStackFrame*, JSValueImpl);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_extract, JSValueImpl) (JSStackFrame*, JSNativeExceptionHandler*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_try_enter, void) (JSStackFrame*, JSNativeExceptionHandler*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_try_exit, void) (JSStackFrame*, JSNativeExceptionHandler*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_catch_enter, void) (JSStackFrame*, const char*, JSValueImpl);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_catch_exit, void) (JSStackFrame*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_set_global_handler, void) (NativeContext*, JSNativeExceptionHandler*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_report, void) (NativeContext*, JSNativeExceptionHandler*);
	RUNTIME_FUNCTION_EH(mo_runtime_exception_error_code, mo_int32) (NativeContext*, JSNativeExceptionHandler*);


	//-------------------------------------------------------------------------------------------------
	// SECTION: DEBUGGING CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	RUNTIME_FUNCTION_EH(mo_debug_break, void) (JSStackFrame*, mo_int32, mo_int32);


	//-------------------------------------------------------------------------------------------------
	// SECTION: OP CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	RUNTIME_FUNCTION(mo_runtime_stub_enter_call, JSValueImpl) (JSStackFrame*, EnterFunctionCallInitData*);
	RUNTIME_FUNCTION(mo_runtime_stub_end_call, void) (JSStackFrame*);
	RUNTIME_FUNCTION(mo_runtime_stub_call_func, JSValueImpl) (JSStackFrame*, JSValue, JSValue, mo_uint32, JSValue*);
	RUNTIME_FUNCTION(mo_runtime_stub_call_func_by_name, JSValueImpl) (JSStackFrame*, JSValue, JSValue, mo_uint32, JSValue*);
	RUNTIME_FUNCTION(mo_runtime_stub_construct, JSValueImpl) (JSStackFrame*, JSValue, mo_uint32, JSValue*);
	RUNTIME_FUNCTION(mo_runtime_stub_new_array, JSValueImpl) (JSStackFrame*, JSValue*, mo_uint32);
	RUNTIME_FUNCTION(mo_runtime_stub_new_func, JSValueImpl) (JSStackFrame*, const mo_utf8stringptr, mo_uint32, mo_bool, NativeFunctionPtr);
	RUNTIME_FUNCTION(mo_runtime_stub_new_prop_iter, JSPropertyNameIterator*) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_inc_prop_iter, mo_bool) (JSStackFrame*, JSPropertyNameIterator*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_get_var, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_put_var, JSValueImpl) (JSStackFrame*, JSValue, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_put_var_by_name, JSValueImpl) (JSStackFrame*, const mo_utf8stringptr, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_put_var_in_this, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_delete_var, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_delete_var_in_this, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_this, JSValueImpl) (JSStackFrame*);
	RUNTIME_FUNCTION(mo_runtime_stub_resolve, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_resolve_base, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_typeof, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_to_number, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_to_boolean, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_negate, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_prefix_inc, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_prefix_dec, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_postfix_inc, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_postfix_dec, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_bit_op, JSValueImpl) (JSStackFrame*, BitOp, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_not, JSValueImpl) (JSStackFrame*, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_add, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_subtract, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_multiply, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_divide, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_modulus, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_lshift, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_rshift, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_urshift, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_less, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_less_or_equal, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_greater, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_greater_or_equal, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_instanceof, JSValueImpl) (JSStackFrame*, JSValue value, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_is_in, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_equal, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_equal_strict, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_not_equal, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_not_equal_strict, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_logical_and, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
	RUNTIME_FUNCTION(mo_runtime_stub_logical_or, JSValueImpl) (JSStackFrame*, JSValue, JSValue);
}

#endif