#ifndef MOENJIN_CODEGEN_STUBNAMES_H
#define MOENJIN_CODEGEN_STUBNAMES_H

#include "Platform/Configuration.h"
#include "CodeGen/IRType.h"

namespace MoEnjin
{
	#define FOR_EACH_STUB_NAME(macro)								\
        macro(stub_enter_call,				6, IRStubRetValueTy)	\
		macro(stub_end_call,				0, IRStubRetVoidTy)		\
		macro(stub_call_func,				4, IRStubRetValueTy)	\
		macro(stub_call_func_by_name,		4, IRStubRetValueTy)	\
		macro(stub_construct,				3, IRStubRetValueTy)	\
		macro(stub_new_func,				4, IRStubRetValueTy)	\
		macro(stub_new_prop_iter,			1, IRStubRetValueTy)	\
		macro(stub_inc_prop_iter,			3, IRStubRetValueTy)	\
		macro(stub_get_var,					2, IRStubRetValueTy)	\
        macro(stub_put_var,					3, IRStubRetValueTy)	\
		macro(stub_put_var_by_name,			2, IRStubRetValueTy)	\
		macro(stub_put_var_in_this,			2, IRStubRetValueTy)	\
		macro(stub_delete_var,				2, IRStubRetValueTy)	\
		macro(stub_delete_var_in_this,		1, IRStubRetValueTy)	\
		macro(stub_this,					0, IRStubRetValueTy)	\
        macro(stub_resolve,					1, IRStubRetValueTy)	\
		macro(stub_resolve_base,			1, IRStubRetValueTy)	\
		macro(stub_typeof,					1, IRStubRetValueTy)	\
		macro(stub_to_number,				1, IRStubRetValueTy)	\
		macro(stub_to_boolean,				1, IRStubRetValueTy)	\
		macro(stub_negate,					1, IRStubRetValueTy)	\
		macro(stub_prefix_inc,				2, IRStubRetValueTy)	\
		macro(stub_prefix_dec,				2, IRStubRetValueTy)	\
		macro(stub_postfix_inc,				2, IRStubRetValueTy)	\
		macro(stub_postfix_dec,				2, IRStubRetValueTy)	\
		macro(stub_bit_op,					3, IRStubRetValueTy)	\
		macro(stub_not,						1, IRStubRetValueTy)	\
        macro(stub_add,						2, IRStubRetValueTy)	\
		macro(stub_subtract,				2, IRStubRetValueTy)	\
		macro(stub_multiply,				2, IRStubRetValueTy)	\
		macro(stub_divide,					2, IRStubRetValueTy)	\
		macro(stub_modulus,					2, IRStubRetValueTy)	\
		macro(stub_lshift,					2, IRStubRetValueTy)	\
		macro(stub_rshift,					2, IRStubRetValueTy)	\
		macro(stub_urshift,					2, IRStubRetValueTy)	\
		macro(stub_is_less,					2, IRStubRetValueTy)	\
		macro(stub_is_less_or_equal,		2, IRStubRetValueTy)	\
		macro(stub_is_greater,				2, IRStubRetValueTy)	\
		macro(stub_is_greater_or_equal,		2, IRStubRetValueTy)	\
		macro(stub_is_instanceof,			2, IRStubRetValueTy)	\
		macro(stub_is_in,					2, IRStubRetValueTy)	\
		macro(stub_equal,					2, IRStubRetValueTy)	\
		macro(stub_equal_strict,			2, IRStubRetValueTy)	\
		macro(stub_not_equal,				2, IRStubRetValueTy)	\
		macro(stub_not_equal_strict,		2, IRStubRetValueTy)	\
		macro(stub_logical_and,				2, IRStubRetValueTy)	\
		macro(stub_logical_or,				2, IRStubRetValueTy)

	#define FOR_EACH_EXCEPTION_FUNC_NAME(macro)	\
        macro(exception_throw)					\
		macro(exception_extract)				\
		macro(exception_try_enter)				\
		macro(exception_try_exit)				\
		macro(exception_catch_enter)			\
		macro(exception_catch_exit)				\
		macro(exception_set_global_handler)		\
		macro(exception_report)					\
		macro(exception_error_code)

	#define FOR_EACH_RUNTIME_FUNC_NAME(macro)	\
        macro(initialize)						\
		macro(initialize_environment)			\
		macro(shutdown)

	#define STUB_ID_ENUM(stub, size, rtype) stub,
		typedef enum
		{
			FOR_EACH_STUB_NAME(STUB_ID_ENUM)
		} StubID;
	#undef STUB_ID_ENUM

	#define STUB_NAME_SIZES(stub, size, rtype) const int stub##_size = size;
	FOR_EACH_STUB_NAME(STUB_NAME_SIZES);
	#undef STUB_NAME_SIZES

	#define STUB_LENGTH(stub) stub##_size


	#define EXCEPTION_FUNC_ID_ENUM(name) name,
		typedef enum
		{
			FOR_EACH_EXCEPTION_FUNC_NAME(EXCEPTION_FUNC_ID_ENUM)
		} ExceptionFunctionID;
	#undef EXCEPTION_FUNC_ID_ENUM

	#define RUNTIME_FUNC_ID_ENUM(name) name,
		typedef enum
		{
			FOR_EACH_RUNTIME_FUNC_NAME(RUNTIME_FUNC_ID_ENUM)
		} RuntimeFunctionID;
	#undef RUNTIME_FUNC_ID_ENUM

}

#endif