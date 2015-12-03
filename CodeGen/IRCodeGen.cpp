#include "CodeGen/IRCodeGen.h"

#include "AST/AST.h"
#include "AST/ASTContext.h"
#include "AST/OpCodes.h"
#include "CodeGen/IRCodeGenAnnotationWriter.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IREmitter.h"
#include "CodeGen/IRFunctionScope.h"
#include "CodeGen/IRTypeBuilder.h"
#include "CodeGen/IRTypes.h"
#include "CodeGen/IRValue.h"
#include "CodeGen/StubCall.h"
#include "CodeGen/StubNames.h"
#include "Core/StringTable.h"
#include "Platform/LLVM.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	llvm::Constant* IRCodeGen::JSValueEmptyConstant = IRTypeBuilder::GetConstInt64(JSValue::Encode(jsEmpty()));
	llvm::Constant* IRCodeGen::JSValueUndefinedConstant = IRTypeBuilder::GetConstInt64(JSValue::Encode(jsUndef()));
	llvm::Constant* IRCodeGen::JSValueNullConstant = IRTypeBuilder::GetConstInt64(JSValue::Encode(jsNull()));
	llvm::Constant* IRCodeGen::JSValueTrueConstant = IRTypeBuilder::GetConstInt64(JSValue::Encode(jsTrue()));
	llvm::Constant* IRCodeGen::JSValueFalseConstant = IRTypeBuilder::GetConstInt64(JSValue::Encode(jsFalse()));

	IRCodeGen::IRCodeGen() 
		: mMainModule(0)
		, mDataLayout(0)
		, mEmitter(0)
	{ 
		
	}

	llvm::LLVMContext& IRCodeGen::GetContext()
	{
		return llvm::getGlobalContext();
	}

	llvm::DataLayout* IRCodeGen::GetDataLayout()
	{
		return mDataLayout;
	}

	llvm::StructType* IRCodeGen::GetStructType(const UTF8String& name) const
	{
		return mMainModule->getTypeByName(name.GetChars());
	}

	void IRCodeGen::Save(const UTF8String& filePath)
	{
		if(!mMainModule)
			throw std::exception("Unable to write bytecode to disk. No module was generated.");
			
		mo_utf8string errInfo;
		llvm::raw_fd_ostream fs(filePath.GetChars(), errInfo, llvm::raw_fd_ostream::F_Binary);

		if(!errInfo.empty())
			throw std::exception(errInfo.c_str());

		// TODO : hookup comment sources for the annotation writer
		IRCodeGenAnnotationWriter w;
		mMainModule->print(fs, &w);
		fs.close();
	}

	void IRCodeGen::Generate(ProgramNode* program)
	{
		Initialize(program->GetSourceScriptName());

		// emit the program
		mEmitter->EmitProgram(program);
	}

	void IRCodeGen::Initialize(const UTF8String& moduleName)
	{
		mMainModule = new llvm::Module(moduleName.GetChars(), GetContext());
		mDataLayout = new llvm::DataLayout(mMainModule);
		mEmitter = new IREmitter(this);

		// define constant globals
		mEmitter->EmitGlobalVariable(IRConsts::JSValueEmptyName, IRTypeBuilder::GetType(IRJSValueTy), mo_true, JSValueEmptyConstant);
		mEmitter->EmitGlobalVariable(IRConsts::JSValueUndefinedName, IRTypeBuilder::GetType(IRJSValueTy), mo_true, JSValueUndefinedConstant);
		mEmitter->EmitGlobalVariable(IRConsts::JSValueNullName, IRTypeBuilder::GetType(IRJSValueTy), mo_true, JSValueNullConstant);
		mEmitter->EmitGlobalVariable(IRConsts::JSValueFalseName, IRTypeBuilder::GetType(IRJSValueTy), mo_true, JSValueFalseConstant);
		mEmitter->EmitGlobalVariable(IRConsts::JSValueTrueName, IRTypeBuilder::GetType(IRJSValueTy), mo_true, JSValueTrueConstant);

		// define internal helper methods
		GenerateEncodeJSValueMethod();
	}

	void IRCodeGen::DeclareStructType(const UTF8String& name, llvm::StructType* typeModel)
	{
		// first check if the type has already been declared
		llvm::StructType* type = mMainModule->getTypeByName(name.GetChars());

		// type declaration has not yet been created
		// so create the initial type
		if(!type)
			type = llvm::StructType::create(GetContext(), name.GetChars());
			
			
			
		// construct the type internals when opaque, we do this check
		// in case the type has been declared beforehand somewhere else
		// as an opaque type
		if(type->isOpaque())
		{
			std::vector<llvm::Type*> elementTypes;

			for(unsigned i = 0; i < typeModel->getNumElements(); ++i)
				elementTypes.push_back(typeModel->getElementType(i));

			type->setBody(elementTypes);	
		}
	}

	void IRCodeGen::GenerateEncodeJSValueMethod()
	{
		IRFunction func = mo_null;
		IRValue payloadArg = mo_null;
		IRValue tagArg = mo_null;
		IRValue value = mo_null;
		IRValue valueBits = mo_null;
		IRValue payloadPtr = mo_null;
		IRValue tagPtr = mo_null;
		std::vector<llvm::Type*> paramTypes(2, IRTypeBuilder::GetType(IRInt32Ty));

		func = mEmitter->BeginFunction(kIRFunctionTypeBuiltin, IRConsts::JSEncodeValueFunctionName, IRTypeBuilder::GetFunctionType(IRInt64Ty, paramTypes), llvm::GlobalValue::PrivateLinkage);
		func->addFnAttr(llvm::Attribute::AlwaysInline);
		payloadArg = mEmitter->ValueWithName(IRValue(func->arg_begin()), IRConsts::JSEncodeValuePayloadName);
		tagArg = mEmitter->ValueWithName(IRValue(func->arg_begin()->getNextNode()), IRConsts::JSEncodeValueTagName);

		value = mEmitter->GetFunctionScope()->GetReturnValue();
		valueBits = mEmitter->EmitBitCast(value, IRInt32PtrTy, IRConsts::JSEncodedValueBitsName);
		payloadPtr = mEmitter->EmitGEP(valueBits, 0, 1, IRConsts::JSEncodedValuePayloadName);
		tagPtr = mEmitter->EmitGEP(valueBits, 1, 1, IRConsts::JSEncodedValueTagName);

		mEmitter->EmitStore(payloadPtr, payloadArg);
		mEmitter->EmitStore(tagPtr, tagArg);
		mEmitter->EndFunction();
	}
}