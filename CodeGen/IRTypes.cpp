#include "CodeGen/IRTypes.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRTypeBuilder.h"

namespace MoEnjin
{
	#define RUNTIME_CALL_CONV llvm::CallingConv::C

	IRTypes::IRTypes(IREmitter* emitter)
		: mEmitter(emitter)
	{
		UTF8String moduleName = mEmitter->GetModuleName();
		UTF8String moduleStringTableName = UTF8String(IR_TYPE_NAME("StringTable_")) + moduleName;

		JSCallFrameTy = IRTypeBuilder::GetPointerTo(IRByteTy);
		ModuleStringTableTy = llvm::StructType::create(mEmitter->GetContext(), moduleStringTableName.GetChars());

		NativeFunctionBindingTy = llvm::StructType::create(IR_TYPE_NAME("NativeFunctionBinding"),
									IRTypeBuilder::GetPointerType(IRBytePtrTy),
									IRTypeBuilder::GetPointerType(IRBytePtrTy),
									IRTypeBuilder::GetType(IRInt32Ty),
									IRTypeBuilder::GetType(IRBitTy), mo_null);

		NativeStringCollectionTy = llvm::StructType::create(IR_TYPE_NAME("NativeStringCollection"),
									IRTypeBuilder::GetType(IRInt32Ty),
									IRTypeBuilder::GetPointerTo(IRBytePtrTy), mo_null);

		NativeExceptionDataTy = llvm::StructType::create(IR_TYPE_NAME("NativeExceptionData"), 
									IRTypeBuilder::GetArrayType(IRInt32Ty, IRConsts::SET_JMP_BUFFER_SIZE), 
									IRTypeBuilder::GetType(IRJSValueTy),
									IRTypeBuilder::GetPointerType(IRBytePtrTy), mo_null);

		NativeContextTy = llvm::StructType::create(IR_TYPE_NAME("NativeContext"), 
									IRTypeBuilder::GetPointerType(IRBytePtrTy),
									ModuleStringTableTy->getPointerTo(), mo_null);

		NativeErrorCodeTy = llvm::IntegerType::getInt32Ty(emitter->GetContext());

		EnterFunctionCallInitDataTy = llvm::StructType::create(IR_TYPE_NAME("EnterFunctionCallInitData"), 
									IRTypeBuilder::GetType(IRBitTy),				// isFunctionCode
									IRTypeBuilder::GetType(IRBitTy),				// isStrictMode
									IRTypeBuilder::GetType(IRInt32Ty),				// numParameters
									IRTypeBuilder::GetType(IRInt32Ty),				// numVariables
									IRTypeBuilder::GetType(IRInt32Ty),				// numFunctions
									IRTypeBuilder::GetPointerTo(IRBytePtrTy),		// parameters
									IRTypeBuilder::GetPointerTo(IRBytePtrTy),		// variables
									NativeFunctionBindingTy->getPointerTo()->getPointerTo(), // functions
									mo_null);
	}


	//-------------------------------------------------------------------------------------------------
	// Startup/Shutdown
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	llvm::Constant* IRTypes::GetInitializeFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_initialize", 
			IRTypeBuilder::GetFunctionType(IRVoidTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetInitializeEnvironmentFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_initialize_environment", 
			IRTypeBuilder::GetFunctionType(IRBytePtrTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetShutdownFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_shutdown", 
			IRTypeBuilder::GetFunctionType(IRVoidTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetInitializeStringsFn() const
	{
		return mEmitter->GetOrInsertFunction(IR_INTERNAL_FUNCTION_NAME("initialize_strings"), IRTypeBuilder::GetFunctionType(IRVoidTy));
	}


			
	//-------------------------------------------------------------------------------------------------
	// Exception Handling
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	llvm::Constant* IRTypes::GetExceptionThrowFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy), IRTypeBuilder::GetType(IRJSValueTy) };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_throw",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionTryEnterFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_try_enter",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionTryExitFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_try_exit",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionCatchEnterFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy), IRTypeBuilder::GetPointerTo(IRByteTy), IRTypeBuilder::GetType(IRJSValueTy) };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_catch_enter",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionCatchExitFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy) };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_catch_exit",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionExtractFn() const
	{
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRByteTy), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_extract",
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionSetGlobalHandlerFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo(), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_set_global_handler",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionReportFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo(), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_report",
			IRTypeBuilder::GetFunctionType(IRVoidTy, params));
	}

	llvm::Constant* IRTypes::GetExceptionErrorCodeFn() const
	{
		llvm::Type* params[] = { NativeContextTy->getPointerTo(), NativeExceptionDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_exception_error_code",
			IRTypeBuilder::GetFunctionType(IRInt32Ty, params));
	}

	llvm::Constant* IRTypes::GetSetJmpFn() const
	{
		// TODO: This is specifically the prototype for x86, need to 
		//       lookup the other platform signatures
		//
		llvm::Type* params[] = { IRTypeBuilder::GetPointerTo(IRInt32Ty) };

		IRConstant fnCon = mEmitter->GetOrInsertFunction(
			"_setjmp",
			IRTypeBuilder::GetFunctionType(IRInt32Ty, params));

		llvm::Function* fn = fnCon.GetAs<llvm::Function>();
		fn->setAttributes(llvm::AttributeSet::get(mEmitter->GetContext(), llvm::AttributeSet::FunctionIndex, llvm::Attribute::NonLazyBind));

		return fn;
	}


	//-------------------------------------------------------------------------------------------------
	// Debugging
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	llvm::Constant* IRTypes::GetDebugBreakFn() const
	{
		llvm::Type* params[] = { JSCallFrameTy, IRTypeBuilder::GetType(IRInt32Ty), IRTypeBuilder::GetType(IRInt32Ty) };

		return mEmitter->GetOrInsertFunction(
			"mo_debug_break", 
			IRTypeBuilder::GetFunctionType(IRVoidTy, params), RUNTIME_CALL_CONV);
	}


	//-------------------------------------------------------------------------------------------------
	// Stubs
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	llvm::Constant* IRTypes::GetRuntimeStubEnterCallFn() const
	{
		llvm::Type* params[] = { JSCallFrameTy, EnterFunctionCallInitDataTy->getPointerTo() };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_enter_call", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubEndCallFn() const
	{
		llvm::Type* params[] = { JSCallFrameTy };

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_end_call", 
			IRTypeBuilder::GetFunctionType(IRVoidTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubCallFuncFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy), 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRInt32Ty),
			IRTypeBuilder::GetType(IRJSValuePtrTy) 
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_call_func", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubCallFuncByNameFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy), 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRInt32Ty),
			IRTypeBuilder::GetType(IRJSValuePtrTy) 
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_call_func_by_name", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubConstructFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy), 
			IRTypeBuilder::GetType(IRInt32Ty),
			IRTypeBuilder::GetType(IRJSValuePtrTy) 
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_construct", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNewArrayFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetPointerTo(IRByteTy),
			IRTypeBuilder::GetType(IRInt32Ty),
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_new_array", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNewFuncFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetPointerTo(IRByteTy),
			IRTypeBuilder::GetType(IRInt32Ty),
			IRTypeBuilder::GetType(IRBitTy),
			IRTypeBuilder::GetPointerTo(IRByteTy) 
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_new_func", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNewPropIterFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_new_prop_iter", 
			IRTypeBuilder::GetFunctionType(IRBytePtrTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIncPropIterFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetPointerTo(IRByteTy),
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_inc_prop_iter", 
			IRTypeBuilder::GetFunctionType(IRBitTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubGetVarFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_get_var", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPutVarFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_put_var", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPutVarByNameFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetPointerTo(IRByteTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_put_var_by_name", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPutVarInThisFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_put_var_in_this", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubDeleteVarFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_delete_var", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubDeleteVarInThisFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_delete_var_in_this", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubThisFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_this", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubResolveFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_resolve", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubResolveBaseFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_resolve_base", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubTypeOfFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_typeof", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubToNumberFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_to_number", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubToBooleanFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_to_boolean", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNegateFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_negate", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPrefixIncFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_prefix_inc", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPrefixDecFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_prefix_dec", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPostfixIncFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_postfix_inc", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubPostfixDecFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_postfix_dec", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubBitOpFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRInt32Ty),
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_bit_op", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNotFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_not", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubAddFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_add", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubSubtractFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_subtract", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubMultiplyFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_multiply", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubDivideFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_divide", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubModulusFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_modulus", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubLeftShiftFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_lshift", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubRightShiftFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_rshift", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubUnsignedRightShiftFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_urshift", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsLessFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_less", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsLessOrEqualFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_less_or_equal", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsGreaterFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_greater", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsGreaterOrEqualFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_greater_or_equal", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsInstanceOfFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_instanceof", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubIsInFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_is_in", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubEqualFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_equal", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubEqualStrictFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_equal_strict", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNotEqualFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_not_equal", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubNotEqualStrictFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_not_equal_strict", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubLogicalANDFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_logical_and", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

	llvm::Constant* IRTypes::GetRuntimeStubLogicalORFn() const
	{
		llvm::Type* params[] = { 
			JSCallFrameTy, 
			IRTypeBuilder::GetType(IRJSValueTy),
			IRTypeBuilder::GetType(IRJSValueTy)
		};

		return mEmitter->GetOrInsertFunction(
			"mo_runtime_stub_logical_or", 
			IRTypeBuilder::GetFunctionType(IRJSValueTy, params), RUNTIME_CALL_CONV);
	}

}