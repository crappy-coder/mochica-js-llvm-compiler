#ifndef MOENJIN_CODEGEN_IRTYPEBUILDER_H
#define MOENJIN_CODEGEN_IRTYPEBUILDER_H

#include "Platform/Configuration.h"
#include "Platform/LLVMDecls.h"
#include "CodeGen/IRType.h"
#include "CodeGen/IRTypes.h"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/Twine.h>

namespace MoEnjin
{
	class IREmitter;
	class IRTypeBuilder
	{
		MO_STATIC_CLASS(IRTypeBuilder);

		public:

			///
			/// Built-In Primitive Types
			///
			static llvm::Type* GetType(IRType type); 
			static llvm::Type* GetType(IRType type, llvm::LLVMContext& context);

			static llvm::PointerType* GetPointerType(IRType type); 
			static llvm::PointerType* GetPointerType(IRType type, llvm::LLVMContext& context);

			static llvm::PointerType* GetPointerTo(IRType type, mo_unsigned addrSpace = 0);
			static llvm::PointerType* GetPointerTo(IRType type, llvm::LLVMContext& context, mo_unsigned addrSpace = 0);


			///
			/// N-Sized Integer Types
			///
			static llvm::Type* GetIntNType(mo_unsigned size); 
			static llvm::Type* GetIntNType(mo_unsigned size, llvm::LLVMContext& context); 
			static llvm::Type* GetIntNPtrType(mo_unsigned size);
			static llvm::Type* GetIntNPtrType(mo_unsigned size, llvm::LLVMContext& context);


			///
			/// Array Types
			///
			static llvm::ArrayType* GetArrayType(IRType type, mo_unsigned size);
			static llvm::ArrayType* GetArrayType(IRType type, mo_unsigned size, llvm::LLVMContext& context);


			///
			/// Function Types
			///
			static llvm::FunctionType* GetFunctionType(IRType type); 
			static llvm::FunctionType* GetFunctionType(IRType type, llvm::LLVMContext& context); 
			static llvm::FunctionType* GetFunctionType(IRType type, llvm::ArrayRef<llvm::Type*> params); 
			static llvm::FunctionType* GetFunctionType(IRType type, llvm::ArrayRef<llvm::Type*> params, llvm::LLVMContext& context); 
			


			///
			/// Function Types w/ Variable Arguments
			///
			static llvm::FunctionType* GetFunctionVAType(IRType type); 
			static llvm::FunctionType* GetFunctionVAType(IRType type, llvm::LLVMContext& context); 

			static llvm::FunctionType* GetFunctionVAType(IRType type, llvm::ArrayRef<llvm::Type*> params); 
			static llvm::FunctionType* GetFunctionVAType(IRType type, llvm::ArrayRef<llvm::Type*> params, llvm::LLVMContext& context); 


			///
			/// Cast -> IntToPtr
			///
			static llvm::Constant* GetCastIntToPtr(llvm::ConstantInt* constInt);

			static llvm::Constant* GetCastIntToPtr(llvm::ConstantInt* constInt, IRType ptrType);
			static llvm::Constant* GetCastIntToPtr(llvm::ConstantInt* constInt, IRType ptrType, llvm::LLVMContext& context);

			static llvm::Constant* GetCastInt8ToPtr(mo_int8 value, IRType ptrTy);
			static llvm::Constant* GetCastInt8ToPtr(mo_int8 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastInt16ToPtr(mo_int16 value, IRType ptrTy);
			static llvm::Constant* GetCastInt16ToPtr(mo_int16 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastInt32ToPtr(mo_int32 value, IRType ptrTy);
			static llvm::Constant* GetCastInt32ToPtr(mo_int32 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastInt64ToPtr(mo_int64 value, IRType ptrTy);
			static llvm::Constant* GetCastInt64ToPtr(mo_int64 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastUInt8ToPtr(mo_uint8 value, IRType ptrTy);
			static llvm::Constant* GetCastUInt8ToPtr(mo_uint8 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastUInt16ToPtr(mo_uint16 value, IRType ptrTy);
			static llvm::Constant* GetCastUInt16ToPtr(mo_uint16 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastUInt32ToPtr(mo_uint32 value, IRType ptrTy);
			static llvm::Constant* GetCastUInt32ToPtr(mo_uint32 value, IRType ptrTy, llvm::LLVMContext& context);

			static llvm::Constant* GetCastUInt64ToPtr(mo_uint64 value, IRType ptrTy);
			static llvm::Constant* GetCastUInt64ToPtr(mo_uint64 value, IRType ptrTy, llvm::LLVMContext& context);


			///
			/// Constant Types
			///
			static llvm::Constant* GetConstNullPtr(IRType type);
			static llvm::Constant* GetConstNullPtr(llvm::PointerType* type);

			static llvm::Constant* GetConstNullValue(IRType type);
			static llvm::Constant* GetConstNullValue(llvm::Type* type);

			static llvm::ConstantInt* GetConstIntPtr(void* ptr);

			static llvm::ConstantInt* GetConstInt(mo_int64 value, mo_unsigned numBits);
			static llvm::ConstantInt* GetConstInt8(mo_int8 value = 0); 
			static llvm::ConstantInt* GetConstInt16(mo_int16 value = 0); 
			static llvm::ConstantInt* GetConstInt32(mo_int32 value = 0); 
			static llvm::ConstantInt* GetConstInt64(mo_int64 value = 0); 

			static llvm::ConstantInt* GetConstUInt(mo_uint64 value, mo_unsigned numBits);
			static llvm::ConstantInt* GetConstUInt8(mo_uint8 value = 0); 
			static llvm::ConstantInt* GetConstUInt16(mo_uint16 value = 0); 
			static llvm::ConstantInt* GetConstUInt32(mo_uint32 value = 0); 
			static llvm::ConstantInt* GetConstUInt64(mo_uint64 value = 0); 

			static llvm::Constant* GetConstTrue(IRType type = IRBitTy);
			static llvm::Constant* GetConstTrue(llvm::LLVMContext& context, IRType type = IRBitTy);

			static llvm::Constant* GetConstFalse(IRType type = IRBitTy);
			static llvm::Constant* GetConstFalse(llvm::LLVMContext& context, IRType type = IRBitTy);

			static llvm::Constant* GetConstBool(mo_bool value, IRType type = IRBitTy);
			static llvm::Constant* GetConstBool(mo_bool value, llvm::LLVMContext& context, IRType type = IRBitTy);

			static llvm::Constant* GetConstDouble(mo_double value);

			static llvm::Constant* GetConstString(const UTF8String& str); 

			///
			/// Get Element Pointer Constants
			///
			static llvm::Constant* GetConstElementPtr(llvm::Constant* from, mo_int32 indices[], mo_int32 indicesSize);
			static llvm::Constant* GetConstElementPtr(llvm::Constant* from, llvm::Constant* indices[], mo_int32 indicesSize);
			static llvm::Constant* GetConstElementPtr(llvm::Constant* from, std::vector<llvm::Constant*> indices);



			///
			/// Get String Pointer Constants
			///
			static llvm::Constant* GetStringPtr(llvm::Constant* from);
			static llvm::Constant* GetStringPtr(llvm::Constant* from, llvm::Constant* index);
	};
}

#endif