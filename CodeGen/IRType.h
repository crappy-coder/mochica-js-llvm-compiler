#ifndef MOENJIN_CODEGEN_IRTYPE_H
#define MOENJIN_CODEGEN_IRTYPE_H

#include "Platform/Configuration.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace MoEnjin
{
	enum IRType
	{
		// misc types
		IRVoidTy,
		IRLabelTy,
		IRMetadataTy,

		// floating point types
		IRHalfTy,
		IRHalfPtrTy,
		IRFloatTy,
		IRFloatPtrTy,
		IRDoubleTy,
		IRDoublePtrTy,
		IRFP128Ty,
		IRFP128PtrTy,

		// system specific floating point types
		IRFP128PPCTy,
		IRFP128PPCPtrTy,
		IRFP80x86Ty,
		IRFP80x86PtrTy,

		// integer types
		IRBitTy,
		IRBitPtrTy,
		IRByteTy,
		IRBytePtrTy,
		IRInt16Ty,
		IRInt16PtrTy,
		IRInt32Ty,
		IRInt32PtrTy,
		IRInt64Ty,
		IRInt64PtrTy,

		// builtin types
		IRJSStackFrameTy,
		IRJSValueTy,
		IRJSValuePtrTy,
		IRJSFunctionRetTy,
		IRJSFunctionRetPtrTy,
		IRJSObjectPtrTy,

		// stub return types
		IRStubRetVoidTy,
		IRStubRetVoidPtrTy,
		IRStubRetIntTy,
		IRStubRetValueTy,
		IRStubRetObjectTy
	};
}

#endif