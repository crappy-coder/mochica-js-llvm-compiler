#ifndef MOENJIN_CODEGEN_IRTYPES_H
#define MOENJIN_CODEGEN_IRTYPES_H

#include "Platform/Configuration.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IRValue.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Support/NoFolder.h>

namespace MoEnjin
{
	typedef llvm::IRBuilder<true, llvm::NoFolder> IRBuilderDef;

	class IREmitter;
	class IRTypes
	{
		public:
			//-------------------------------------------------------------------------------------------------
			// Startup/Shutdown
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			llvm::StructType* NativeContextTy;
			llvm::StructType* ModuleStringTableTy;

			llvm::Constant* GetInitializeFn() const;
			llvm::Constant* GetInitializeEnvironmentFn() const;
			llvm::Constant* GetShutdownFn() const;
			llvm::Constant* GetInitializeStringsFn() const;
			
			//-------------------------------------------------------------------------------------------------
			// Exception Handling
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			llvm::StructType*	NativeExceptionDataTy;
			llvm::Type*			NativeErrorCodeTy;

			llvm::Constant* GetExceptionThrowFn() const;
			llvm::Constant* GetExceptionTryEnterFn() const;
			llvm::Constant* GetExceptionTryExitFn() const;
			llvm::Constant* GetExceptionCatchEnterFn() const;
			llvm::Constant* GetExceptionCatchExitFn() const;
			llvm::Constant* GetExceptionExtractFn() const;
			llvm::Constant* GetExceptionSetGlobalHandlerFn() const;
			llvm::Constant* GetExceptionReportFn() const;
			llvm::Constant* GetExceptionErrorCodeFn() const;
			llvm::Constant* GetSetJmpFn() const;


			//-------------------------------------------------------------------------------------------------
			// Debugging
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			llvm::Constant* GetDebugBreakFn() const;


			//-------------------------------------------------------------------------------------------------
			// Stubs
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			llvm::Type* JSCallFrameTy;

			llvm::StructType* EnterFunctionCallInitDataTy;
			llvm::StructType* NativeFunctionBindingTy;
			llvm::StructType* NativeStringCollectionTy;

			llvm::Constant* GetRuntimeStubEnterCallFn() const;
			llvm::Constant* GetRuntimeStubEndCallFn() const;
			llvm::Constant* GetRuntimeStubCallFuncFn() const;
			llvm::Constant* GetRuntimeStubCallFuncByNameFn() const;
			llvm::Constant* GetRuntimeStubConstructFn() const;
			llvm::Constant* GetRuntimeStubNewArrayFn() const;
			llvm::Constant* GetRuntimeStubNewFuncFn() const;
			llvm::Constant* GetRuntimeStubNewPropIterFn() const;
			llvm::Constant* GetRuntimeStubIncPropIterFn() const;
			llvm::Constant* GetRuntimeStubGetVarFn() const;
			llvm::Constant* GetRuntimeStubPutVarFn() const;
			llvm::Constant* GetRuntimeStubPutVarByNameFn() const;
			llvm::Constant* GetRuntimeStubPutVarInThisFn() const;
			llvm::Constant* GetRuntimeStubDeleteVarFn() const;
			llvm::Constant* GetRuntimeStubDeleteVarInThisFn() const;
			llvm::Constant* GetRuntimeStubThisFn() const;
			llvm::Constant* GetRuntimeStubResolveFn() const;
			llvm::Constant* GetRuntimeStubResolveBaseFn() const;
			llvm::Constant* GetRuntimeStubTypeOfFn() const;
			llvm::Constant* GetRuntimeStubToNumberFn() const;
			llvm::Constant* GetRuntimeStubToBooleanFn() const;
			llvm::Constant* GetRuntimeStubNegateFn() const;
			llvm::Constant* GetRuntimeStubPrefixIncFn() const;
			llvm::Constant* GetRuntimeStubPrefixDecFn() const;
			llvm::Constant* GetRuntimeStubPostfixIncFn() const;
			llvm::Constant* GetRuntimeStubPostfixDecFn() const;
			llvm::Constant* GetRuntimeStubBitOpFn() const;
			llvm::Constant* GetRuntimeStubNotFn() const;
			llvm::Constant* GetRuntimeStubAddFn() const;
			llvm::Constant* GetRuntimeStubSubtractFn() const;
			llvm::Constant* GetRuntimeStubMultiplyFn() const;
			llvm::Constant* GetRuntimeStubDivideFn() const;
			llvm::Constant* GetRuntimeStubModulusFn() const;
			llvm::Constant* GetRuntimeStubLeftShiftFn() const;
			llvm::Constant* GetRuntimeStubRightShiftFn() const;
			llvm::Constant* GetRuntimeStubUnsignedRightShiftFn() const;
			llvm::Constant* GetRuntimeStubIsLessFn() const;
			llvm::Constant* GetRuntimeStubIsLessOrEqualFn() const;
			llvm::Constant* GetRuntimeStubIsGreaterFn() const;
			llvm::Constant* GetRuntimeStubIsGreaterOrEqualFn() const;
			llvm::Constant* GetRuntimeStubIsInstanceOfFn() const;
			llvm::Constant* GetRuntimeStubIsInFn() const;
			llvm::Constant* GetRuntimeStubEqualFn() const;
			llvm::Constant* GetRuntimeStubEqualStrictFn() const;
			llvm::Constant* GetRuntimeStubNotEqualFn() const;
			llvm::Constant* GetRuntimeStubNotEqualStrictFn() const;
			llvm::Constant* GetRuntimeStubLogicalANDFn() const;
			llvm::Constant* GetRuntimeStubLogicalORFn() const;

		public:
			IRTypes(IREmitter* emitter);

		private:
			IREmitter* mEmitter;
	};

	mo_inline UTF8String IRCallCreateReturnValueName(const IRValue& func)
	{
		if(func.IsNull() || !func->hasName() || func.GetAs<llvm::Function>()->getReturnType()->isVoidTy())
			return "";

		UTF8String name = func->getName().str();
		name += IR_NAME_ENDI(IR_NAME_NEXT("value"));

		return name;
	}

	struct IRNativeFunctionBinding
	{
		const UTF8String name;
		IRFunction function;
		mo_uint32 length;
		mo_bool inStrictMode;

		IRNativeFunctionBinding(const UTF8String& _name, const IRFunction& _func, mo_uint32 _length, mo_bool _inStrictMode)
			: name(_name), function(_func), length(_length), inStrictMode(_inStrictMode) { }
	};

	struct IRNativeStringCollection
	{
		mo_uint32 length;
		const UTF8StringCollection values;

		IRNativeStringCollection(const UTF8StringCollection& _values, mo_uint32 _length)
			: values(_values), length(_length) { }
	};

}

#endif