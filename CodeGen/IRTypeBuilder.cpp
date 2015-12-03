#include "CodeGen/IRTypeBuilder.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRTypes.h"
#include "Platform/LLVM.h"

namespace MoEnjin
{
	///
	/// Built-In Primitive Types
	///
	llvm::Type* IRTypeBuilder::GetType(IRType type) { 
		return GetType(type, llvm::getGlobalContext()); 
	}

	llvm::Type* IRTypeBuilder::GetType(IRType type, llvm::LLVMContext& context) {
		switch(type)
		{
			case IRVoidTy:
			case IRStubRetVoidTy:
				return llvm::Type::getVoidTy(context);
			case IRLabelTy:
				return llvm::Type::getLabelTy(context);
			case IRMetadataTy:
				return llvm::Type::getMetadataTy(context);
			case IRHalfTy:
				return llvm::Type::getHalfTy(context);
			case IRHalfPtrTy:
				return llvm::Type::getHalfPtrTy(context);
			case IRFloatTy:
				return llvm::Type::getFloatTy(context);
			case IRFloatPtrTy:
				return llvm::Type::getFloatPtrTy(context);
			case IRDoubleTy:
				return llvm::Type::getDoubleTy(context);
			case IRDoublePtrTy:
				return llvm::Type::getDoublePtrTy(context);
			case IRFP128Ty:
				return llvm::Type::getFP128Ty(context);
			case IRFP128PtrTy:
				return llvm::Type::getFP128PtrTy(context);
			case IRFP128PPCTy:
				return llvm::Type::getPPC_FP128Ty(context);
			case IRFP128PPCPtrTy:
				return llvm::Type::getPPC_FP128PtrTy(context);
			case IRFP80x86Ty:
				return llvm::Type::getX86_FP80Ty(context);
			case IRFP80x86PtrTy:
				return llvm::Type::getX86_FP80PtrTy(context);
			case IRBitTy:
				return llvm::Type::getInt1Ty(context);
			case IRBitPtrTy:
				return llvm::Type::getInt1PtrTy(context);
			case IRByteTy:
				return llvm::Type::getInt8Ty(context);
			case IRBytePtrTy:
				return llvm::Type::getInt8PtrTy(context);
			case IRInt16Ty:
				return llvm::Type::getInt16Ty(context);
			case IRInt16PtrTy:
				return llvm::Type::getInt16PtrTy(context);
			case IRInt32Ty:
			case IRStubRetIntTy:
				return llvm::Type::getInt32Ty(context);
			case IRInt32PtrTy:
			case IRJSObjectPtrTy:
			case IRJSStackFrameTy:
			case IRStubRetVoidPtrTy:
			case IRStubRetObjectTy:
				return llvm::Type::getInt32PtrTy(context);
			case IRInt64Ty:
			case IRJSValueTy:
			case IRJSFunctionRetTy:
			case IRStubRetValueTy:
				return llvm::Type::getInt64Ty(context);
			case IRInt64PtrTy:
			case IRJSValuePtrTy:
			case IRJSFunctionRetPtrTy:
				return llvm::Type::getInt64PtrTy(context);
		}

		return mo_null;
	}

	llvm::PointerType* IRTypeBuilder::GetPointerType(IRType type) {
		return llvm::cast<llvm::PointerType>(GetType(type));
	}

	llvm::PointerType* IRTypeBuilder::GetPointerType(IRType type, llvm::LLVMContext& context) {
		return llvm::cast<llvm::PointerType>(GetType(type, context));
	}

	llvm::PointerType* IRTypeBuilder::GetPointerTo(IRType type, mo_unsigned addrSpace) {
		return GetType(type)->getPointerTo(addrSpace);
	}

	llvm::PointerType* IRTypeBuilder::GetPointerTo(IRType type, llvm::LLVMContext& context, mo_unsigned addrSpace) {
		return GetType(type, context)->getPointerTo(addrSpace);
	}

	///
	/// N-Sized Integer Types
	///
	llvm::Type* IRTypeBuilder::GetIntNType(mo_unsigned size) { 
		return GetIntNType(size, llvm::getGlobalContext()); 
	}

	llvm::Type* IRTypeBuilder::GetIntNType(mo_unsigned size, llvm::LLVMContext& context) { 
		return llvm::Type::getIntNTy(context, size); 
	}

	llvm::Type* IRTypeBuilder::GetIntNPtrType(mo_unsigned size) {
		return GetIntNPtrType(size, llvm::getGlobalContext()); 
	}

	llvm::Type* IRTypeBuilder::GetIntNPtrType(mo_unsigned size, llvm::LLVMContext& context) {
		return llvm::Type::getIntNPtrTy(context, size); 
	}


	
	///
	/// Array Types
	///
	llvm::ArrayType* IRTypeBuilder::GetArrayType(IRType type, mo_unsigned size) {
		return GetArrayType(type, size, llvm::getGlobalContext());
	}

	llvm::ArrayType* IRTypeBuilder::GetArrayType(IRType type, mo_unsigned size, llvm::LLVMContext& context) {
		return llvm::ArrayType::get(GetType(type, context), size);
	}



	///
	/// Function Types
	///
	llvm::FunctionType* IRTypeBuilder::GetFunctionType(IRType type) { 
		return GetFunctionType(type, llvm::getGlobalContext()); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionType(IRType type, llvm::LLVMContext& context) { 
		return llvm::FunctionType::get(GetType(type, context), mo_false); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionType(IRType type, llvm::ArrayRef<llvm::Type*> params) { 
		return GetFunctionType(type, params, llvm::getGlobalContext()); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionType(IRType type, llvm::ArrayRef<llvm::Type*> params, llvm::LLVMContext& context) { 
		return llvm::FunctionType::get(GetType(type, context), params, mo_false); 
	}


	///
	/// Function Types w/ Variable Arguments
	///
	llvm::FunctionType* IRTypeBuilder::GetFunctionVAType(IRType type) { 
		return GetFunctionVAType(type, llvm::getGlobalContext()); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionVAType(IRType type, llvm::LLVMContext& context) { 
		return llvm::FunctionType::get(GetType(type, context), mo_true); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionVAType(IRType type, llvm::ArrayRef<llvm::Type*> params) { 
		return GetFunctionVAType(type, params, llvm::getGlobalContext()); 
	}

	llvm::FunctionType* IRTypeBuilder::GetFunctionVAType(IRType type, llvm::ArrayRef<llvm::Type*> params, llvm::LLVMContext& context) { 
		return llvm::FunctionType::get(GetType(type, context), params, mo_true); 
	}


	///
	/// Cast -> IntToPtr
	///
	llvm::Constant* IRTypeBuilder::GetCastIntToPtr(llvm::ConstantInt* constInt) {
		return llvm::ConstantExpr::getIntToPtr(constInt, constInt->getType()->getPointerTo());
	}

	llvm::Constant* IRTypeBuilder::GetCastIntToPtr(llvm::ConstantInt* constInt, IRType ptrType) { 
		return GetCastIntToPtr(constInt, ptrType, llvm::getGlobalContext());
	}

	llvm::Constant* IRTypeBuilder::GetCastIntToPtr(llvm::ConstantInt* constInt, IRType ptrType, llvm::LLVMContext& context) {
		return llvm::ConstantExpr::getIntToPtr(constInt, GetPointerType(ptrType, context));
	}

	llvm::Constant* IRTypeBuilder::GetCastInt8ToPtr(mo_int8 value, IRType ptrTy) { 
		return GetCastInt8ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt8ToPtr(mo_int8 value, IRType ptrTy, llvm::LLVMContext& context) { 
		return GetCastIntToPtr(GetConstInt8(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt16ToPtr(mo_int16 value, IRType ptrTy) {
		return GetCastInt16ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt16ToPtr(mo_int16 value, IRType ptrTy, llvm::LLVMContext& context) {
		return GetCastIntToPtr(GetConstInt16(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt32ToPtr(mo_int32 value, IRType ptrTy) { 
		return GetCastInt32ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt32ToPtr(mo_int32 value, IRType ptrTy, llvm::LLVMContext& context) {
		return GetCastIntToPtr(GetConstInt32(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt64ToPtr(mo_int64 value, IRType ptrTy) { 
		return GetCastInt64ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastInt64ToPtr(mo_int64 value, IRType ptrTy, llvm::LLVMContext& context) {
		return GetCastIntToPtr(GetConstInt64(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt8ToPtr(mo_uint8 value, IRType ptrTy) { 
		return GetCastUInt8ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt8ToPtr(mo_uint8 value, IRType ptrTy, llvm::LLVMContext& context) {
		return GetCastIntToPtr(GetConstUInt8(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt16ToPtr(mo_uint16 value, IRType ptrTy) { 
		return GetCastUInt16ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt16ToPtr(mo_uint16 value, IRType ptrTy, llvm::LLVMContext& context) { 
		return GetCastIntToPtr(GetConstUInt16(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt32ToPtr(mo_uint32 value, IRType ptrTy) { 
		return GetCastUInt32ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt32ToPtr(mo_uint32 value, IRType ptrTy, llvm::LLVMContext& context) { 
		return GetCastIntToPtr(GetConstUInt32(value), ptrTy, context); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt64ToPtr(mo_uint64 value, IRType ptrTy) { 
		return GetCastUInt64ToPtr(value, ptrTy, llvm::getGlobalContext()); 
	}

	llvm::Constant* IRTypeBuilder::GetCastUInt64ToPtr(mo_uint64 value, IRType ptrTy, llvm::LLVMContext& context) { 
		return GetCastIntToPtr(GetConstUInt64(value), ptrTy, context); 
	}


	///
	/// Constant Types
	///
	llvm::Constant* IRTypeBuilder::GetConstNullPtr(IRType type) {
		return GetConstNullPtr(GetPointerType(type));
	}

	llvm::Constant* IRTypeBuilder::GetConstNullPtr(llvm::PointerType* type) {
		return llvm::ConstantPointerNull::get(type);
	}

	llvm::Constant* IRTypeBuilder::GetConstNullValue(IRType type) {
		return GetConstNullValue(GetType(type));
	}

	llvm::Constant* IRTypeBuilder::GetConstNullValue(llvm::Type* type) {
		return llvm::Constant::getNullValue(type);
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstIntPtr(void* ptr) {
		return GetConstInt32(mo_cast_type<mo_int32>(ptr));
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstInt(mo_int64 value, mo_unsigned numBits) {
		return llvm::ConstantInt::getSigned(llvm::IntegerType::get(llvm::getGlobalContext(), numBits), (int64_t)value);
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstUInt(mo_uint64 value, mo_unsigned numBits) {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(numBits, (uint64_t)value, mo_false));
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstInt8(mo_int8 value) { 
		return GetConstInt(value, 8); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstInt16(mo_int16 value) { 
		return GetConstInt(value, 16); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstInt32(mo_int32 value) { 
		return GetConstInt(value, 32); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstInt64(mo_int64 value) { 
		return GetConstInt(value, 64); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstUInt8(mo_uint8 value) { 
		return GetConstUInt(value, 8); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstUInt16(mo_uint16 value) { 
		return GetConstUInt(value, 16); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstUInt32(mo_uint32 value) { 
		return GetConstUInt(value, 32); 
	}

	llvm::ConstantInt* IRTypeBuilder::GetConstUInt64(mo_uint64 value) { 
		return GetConstUInt(value, 64); 
	}

	llvm::Constant* IRTypeBuilder::GetConstTrue(IRType type) {
		return GetConstTrue(llvm::getGlobalContext(), type);
	}

	llvm::Constant* IRTypeBuilder::GetConstTrue(llvm::LLVMContext& context, IRType type) {
		return llvm::ConstantInt::getTrue(GetType(type, context));
	}

	llvm::Constant* IRTypeBuilder::GetConstFalse(IRType type) {
		return GetConstFalse(llvm::getGlobalContext(), type);
	}

	llvm::Constant* IRTypeBuilder::GetConstFalse(llvm::LLVMContext& context, IRType type) {
		return llvm::ConstantInt::getFalse(GetType(type, context));
	}

	llvm::Constant* IRTypeBuilder::GetConstBool(mo_bool value, IRType type) {
		return GetConstBool(value, llvm::getGlobalContext(), type);
	}

	llvm::Constant* IRTypeBuilder::GetConstBool(mo_bool value, llvm::LLVMContext& context, IRType type) {
		return llvm::ConstantInt::get(GetType(type, context), (uint64_t)value);
	}

	llvm::Constant* IRTypeBuilder::GetConstDouble(mo_double value) {
		return llvm::ConstantFP::get(GetType(IRDoubleTy), value);
	}

	llvm::Constant* IRTypeBuilder::GetConstString(const UTF8String& str) { 
		return llvm::ConstantDataArray::getString(llvm::getGlobalContext(), str.GetChars()); 
	}

	

	///
	/// Get Element Pointer Constants
	///
	llvm::Constant* IRTypeBuilder::GetConstElementPtr(llvm::Constant* from, mo_int32 indices[], mo_int32 indicesSize) {
		std::vector<llvm::Constant*> constantIndices;

		for(mo_int32 i = 0; i < indicesSize; ++i)
			constantIndices.push_back(GetConstInt32(indices[i]));

		return GetConstElementPtr(from, constantIndices);
	}

	llvm::Constant* IRTypeBuilder::GetConstElementPtr(llvm::Constant* from, llvm::Constant* indices[], mo_int32 indicesSize) {
		std::vector<llvm::Constant*> constantIndices;

		for(mo_int32 i = 0; i < indicesSize; ++i)
			constantIndices.push_back(indices[i]);

		return GetConstElementPtr(from, constantIndices);
	}

	llvm::Constant* IRTypeBuilder::GetConstElementPtr(llvm::Constant* from, std::vector<llvm::Constant*> indices) {
		return llvm::ConstantExpr::getGetElementPtr(from, indices);
	}


	///
	/// Get String Pointer Constants
	///
	llvm::Constant* IRTypeBuilder::GetStringPtr(llvm::Constant* from) {
		std::vector<llvm::Constant*> indices(2, GetConstInt32(0));
		return GetConstElementPtr(from, indices);
	}

	llvm::Constant* IRTypeBuilder::GetStringPtr(llvm::Constant* from, llvm::Constant* index) {
		llvm::Constant* indices[] = {GetConstInt32(0), index};
		return GetConstElementPtr(from, indices, 2);
	}
}