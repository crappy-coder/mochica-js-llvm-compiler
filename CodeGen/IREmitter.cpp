#include "CodeGen/IREmitter.h"
#include "CodeGen/IRCodeGen.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IRFunctionScope.h"
#include "CodeGen/IRStubEmitter.h"
#include "CodeGen/IRType.h"
#include "CodeGen/StubCall.h"
#include "AST/AST.h"
#include "AST/ASTContext.h"
#include "AST/OpCodes.h"
#include "Platform/LLVM.h"
#include "Runtime/JSStubs.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	#define EMIT_CALL_RETURN_VALUE_NAME(f) IRCallCreateReturnValueName(f).GetChars()

	IREmitter::IREmitter(IRCodeGen* generator)
		: mGenerator(generator)
		, mTypes(this)
		, mBuilder(generator->GetContext())
		, mCleanupDestID(1)
	{
		mStubEmitter = new IRStubEmitter(this);
	}

	llvm::Module* IREmitter::GetModule()
	{
		return mGenerator->GetModule();
	}

	UTF8String IREmitter::GetModuleName() const
	{
		return mGenerator->GetModule()->getModuleIdentifier();
	}

	llvm::LLVMContext& IREmitter::GetContext()
	{
		return mGenerator->GetContext();
	}

	llvm::DataLayout* IREmitter::GetDataLayout()
	{
		return mGenerator->GetDataLayout();
	}

	void IREmitter::EmitDefaultSetup()
	{
		// emit an opaque string table to use as a reference during
		// code generation, once we're done emitting code and have all
		// the strings we can finalize the string table
		mStringTableValue = EmitGlobalVariable(IR_INTERNAL_GLOBAL_NAME("GlobalStringTable"), mTypes.ModuleStringTableTy, mo_true, llvm::GlobalValue::InternalLinkage, mo_null);		

		std::vector<llvm::Constant*> vals;
		vals.push_back(IRTypeBuilder::GetConstNullPtr(IRBytePtrTy));
		vals.push_back(mStringTableValue);

		mNativeContext = EmitGlobalVariable(IR_INTERNAL_GLOBAL_NAME("context"), mTypes.NativeContextTy, mo_true, llvm::GlobalValue::InternalLinkage, llvm::ConstantStruct::get(mTypes.NativeContextTy, vals));
	}

	void IREmitter::EmitProgram(ProgramNode* program)
	{
		EmitDefaultSetup();

		// emit the core program function, this will execute the "global" code
		IRFunction execProgramFunc = EmitProgramFunction(program);

		// begin the main function, we use this to essentially wrap a try/catch around so we can 
		// report errors, generate a return code and perform a graceful exit
		BeginFunction(kIRFunctionTypeMain, "main", IRTypeBuilder::GetFunctionType(IRInt32Ty), llvm::GlobalValue::ExternalLinkage);
		
		IRFunctionScope* scope = GetFunctionScope();
		IRFunction func = scope->GetFunction();
		IRBasicBlock runBlock = NewBasicBlock("ExecuteProgram");
		IRBasicBlock handleExceptionBlock = NewBasicBlock("HandleException");
		IRBasicBlock exitBlock = NewBasicBlock("Exit");
		IRValue exceptionDataValue = EmitAllocStruct(mTypes.NativeExceptionDataTy, IR_INTERNAL_VAR_NAME("exception_data"));
		IRValue errorCodeValue = EmitAlloc(mTypes.NativeErrorCodeTy, IR_INTERNAL_VAR_NAME("error_code"));
		IRValue jmpValue;
		IRValue haveExceptionValue;
		IRValue exceptionDataBuf;
		
		// initialize constant strings
		EmitCall(mTypes.GetInitializeStringsFn());

		// initialize system, setup global exception handler and set initial 
		// error code (0=SUCCESS)
		EmitCallToInitialize();
		EmitCallToExceptionSetGlobalHandler(exceptionDataValue);
		EmitStore(errorCodeValue, IRTypeBuilder::GetConstInt32(0));

		// get pointer to the buf member field and set a jump point to this position that will
		// get executed if an unhandled js exception is thrown
		exceptionDataBuf = EmitStructGEP(exceptionDataValue, 0, 2, IR_INTERNAL_VAR_NAME("exception_data_buf"));
		jmpValue = EmitCallWithAttrs(mTypes.GetSetJmpFn(), exceptionDataBuf.Get(), kIRFunctionAttributeNoUnwind | kIRFunctionAttributeReturnsTwice, IR_CALL_RETVAL_NAME("setjmp"));
		haveExceptionValue = mBuilder.CreateIsNotNull(jmpValue.Get(), IR_INTERNAL_VAR_NAME("have_exception"));

		EmitBranch(haveExceptionValue, handleExceptionBlock, runBlock);

		// emit execution block, this runs the top level script code, then jump to
		// the exit block if no exceptions were thrown
		EmitBasicBlock(runBlock);
		EmitCall(execProgramFunc);
		EmitBranch(exitBlock);

		// emit unhandled exception handler block, this will report the exception
		// and then lookup a suitable error code that can be used as the functions
		// exit code
		EmitBasicBlock(handleExceptionBlock);
		EmitCallToExceptionReport(exceptionDataValue);
		EmitStore(errorCodeValue, EmitCallToExceptionErrorCode(exceptionDataValue));

		// emit exit block, this will teardown the js execution environment, release
		// resources, etc.. and then return the exit code
		EmitBasicBlock(exitBlock, mo_true);
		EmitCallToShutdown();
		EmitStore(GetFunctionScope()->GetReturnValue(), EmitLoad(errorCodeValue));

		// finish function
		EndFunction();

		// move the main function to the top so we have faster visual access to it, this
		// is simply a convenience so when we view the IR we don't have to scroll past
		// all the stubs...
		MoveFunctionToFront(func);

		// generate the function that initializes the encoded string values, we
		// do this at the end when we know all the strings that have been used and
		// the function get called before any other code so multiple blocks and 
		// functions can use them safely knowing they have already been initialized
		EmitInitializeStringsFunction();

		EmitStringTable();
	}

	void IREmitter::EmitProgramPrologue(ProgramNode* program)
	{
		//IRValue nativeContextValue = EmitLoad(mNativeContext, IR_INTERNAL_VAR_NAME("native_context"));

		// create and initialize the first js callframe
		GetFunctionScope()->SetExecutionContext(
			ValueWithName(EmitCallToInitializeEnvironment(), IRConsts::JSStackFrameCalleeName));

		// emit the standard function prologue, which sets up the environment and scope
		EmitFunctionPrologue(program);
	}

	void IREmitter::EmitProgramEpilogue(ProgramNode* program)
	{
		// TODO - Return a program exit code
		//        ---------------------------------------------------
		//        need to return a variable that holds the result of
		//        the program execution, i.e. a non-zero value means
		//        an error occurred
		//
		// create the program return value
		//mBuilder.CreateRet(IRTypeBuilder::GetConstInt32(0));

		//EmitStore(GetFunctionScope()->GetReturnValue(), IRTypeBuilder::GetConstInt32(0));
	}

	IRFunction IREmitter::EmitProgramFunction(ProgramNode* program)
	{
		// begin the program function, this will contain the contents of the script 
		// file at the global level
		IRFunction func = BeginFunction(kIRFunctionTypeProgram, IR_INTERNAL_FUNCTION_NAME("execute_program"), IRTypeBuilder::GetFunctionType(IRVoidTy), llvm::GlobalValue::ExternalLinkage);

		// emit the programs prologue
		EmitProgramPrologue(program);

		// emit the programs statements
		EmitStatements(program->GetStatements());

		// finalize program
		EmitProgramEpilogue(program);

		EndFunction();

		MoveFunctionToFront(func);

		return func;
	}

	void IREmitter::EmitFunctionPrologue(ScopedStatement* node)
	{
		// need the parameter names from the function node
		FunctionNode* functionNode = node->GetType() == NODE_FUNCTION ? mo_cast<FunctionNode*>(node) : mo_null;

		// if this is not global code (i.e. the main function) then the 
		// signature of the function will always have the first argument
		// as the callee JSStackFrame, not the caller. Stack frame setup 
		// is done prior to actually calling the function. If the current
		// function has this argument then use it as the scopes execution
		// context.
		IRFunctionScope* scope = GetFunctionScope();

		if(scope->GetType() == kIRFunctionTypeJS)
			scope->SetExecutionContext(IRArgument(scope->GetFunction()->arg_begin()));


		// setup the environment by initialing it with the parameter names (if a function),
		// the variable names and function bindings
		IRNativeStringCollection parameterNames = functionNode 
			? IRNativeStringCollection(functionNode->GetParameters(), functionNode->GetParameterCount()) 
			: IRNativeStringCollection(UTF8StringCollection(), 0);

		IRNativeStringCollection variableNames = IRNativeStringCollection(node->GetVariables().GetNames(), node->GetVariables().GetLength());
		const FunctionList& functions = node->GetFunctions();
		std::vector<IRNativeFunctionBinding> functionBindings;

		if(functions.GetLength() > 0)
		{
			for(mo_int32 i = 0, len = functions.GetLength(); i < len; ++i)
			{
				FunctionNode* func = functions.GetFunction(i);
				IRFunction nativeFunc = EmitFunctionSignature(func);

				// ECMA 5.1, §10.5, §13
				if(func->GetOwnerType() == NODE_FUNCTION_DECL)
					functionBindings.push_back(IRNativeFunctionBinding(func->GetNameOrInferredName(), nativeFunc, func->GetParameterCount(), func->IsStrictMode()));
			}
		}

		mStubEmitter->EmitEnterCall(
			functionBindings.data(), functionBindings.size(), parameterNames, variableNames, node->IsStrictMode(), (scope->GetType() == kIRFunctionTypeJS));
	}

	void IREmitter::EmitFunctionEpilogue()
	{
		mStubEmitter->EmitEndCall();
	}

	void IREmitter::EmitStringTable()
	{
		MO_ASSERT(mTypes.ModuleStringTableTy->isOpaque());

		std::vector<llvm::Constant*> stringTableValues;
		std::vector<llvm::Type*> stringTableTypes;

		for(SymbolTable::const_iterator i = mStringTable_OLD.Begin(); i != mStringTable_OLD.End(); ++i)
		{
			const IRValue& value = i->getValue();

			stringTableValues.push_back(llvm::ConstantExpr::getBitCast(value.GetAs<llvm::Constant>(), IRTypeBuilder::GetPointerTo(IRByteTy)));
		}

		llvm::ArrayType* stringTableDataType = llvm::ArrayType::get(IRTypeBuilder::GetPointerTo(IRByteTy), mStringTable_OLD.Size());

		stringTableTypes.push_back(IRTypeBuilder::GetType(IRInt32Ty));
		stringTableTypes.push_back(stringTableDataType);

		// update the module string table type to its final representation
		mTypes.ModuleStringTableTy->setBody(stringTableTypes);
		
		// update the global variables initializer
		mStringTableValue.GetAs<llvm::GlobalVariable>()->setInitializer(
			llvm::ConstantStruct::get(
				mTypes.ModuleStringTableTy, 
				IRTypeBuilder::GetConstInt32(stringTableValues.size()),
				llvm::ConstantArray::get(stringTableDataType, stringTableValues), mo_null));
	}

	void IREmitter::EmitInitializeStringsFunction()
	{
		IRConstant fn = mTypes.GetInitializeStringsFn();
		BeginFunction(kIRFunctionTypeBuiltin, fn.GetAs<llvm::Function>());

		for(SymbolTable::const_iterator i = mEncodedStringTable.Begin(); i != mEncodedStringTable.End(); ++i)
		{
			UTF8String str = UTF8String(i->getKey().data());

			EmitStore(i->getValue().Get(), EmitEncodedValue(EmitPtrCast(IRTypeBuilder::GetStringPtr(GetString(str)), IRInt32Ty), IRTypeBuilder::GetConstInt32(JSValueDataStringTag)));
		}

		EndFunction();
	}

	IRInstruction IREmitter::EmitBranch(const IRBasicBlock& toBlock)
	{
		IRBasicBlock currentBlock = GetInsertBasicBlock();
		IRInstruction ret = IRInstruction::Null;

		if(!currentBlock.IsNull() && !currentBlock->getTerminator())
			ret = EmitBranchTo(toBlock);

		mBuilder.ClearInsertionPoint();
		return ret;
	}

	IRInstruction IREmitter::EmitBranch(const IRValue& condition, const IRBasicBlock& trueBlock, const IRBasicBlock& falseBlock)
	{
		return mBuilder.CreateCondBr(condition.Get(), trueBlock.Get(), falseBlock.Get());
	}

	IRInstruction IREmitter::EmitBranchTo(const IRBasicBlock& toBlock)
	{
		return mBuilder.CreateBr(toBlock.Get());
	}

	//-------------------------------------------------------------------------------------------------
	// Terminate the current block by emitting a branch which might leave the current cleanup-protected
	// scope. The target scope may not yet be known, in which case this will require a fixup.
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	
	void IREmitter::EmitBranchThroughCleanup(const IRJumpDestination& jumpDest)
	{
		MO_ASSERT(jumpDest.GetCleanup().Encloses(mCleanupStack.begin()) && "jump destination is no longer valid");

		if(!InBasicBlock())
			return;

		// create the branch
		IRInstruction brInst = EmitBranchTo(jumpDest.GetBlock());

		// get the innermost active normal cleanup
		IRCleanupRef topCleanup = mCleanupStack.GetInnermostCleanup();

		// if we're not in an active normal cleanup, or if the destination 
		// cleanup is within the innermost active normal cleanup, then we
		// dont need to worry about fixups.
		if(topCleanup == mCleanupStack.end() || topCleanup.Encloses(jumpDest.GetCleanup()))
		{
			mBuilder.ClearInsertionPoint();
			return;
		}

		// if we can't resolve the destination cleanup, just add this to
		// the current cleanup as a branch fixup.
		if(!jumpDest.GetCleanup().IsValid())
		{
			MO_ASSERT(false && "branch fixups are not yet supported!");

			mBuilder.ClearInsertionPoint();
			return;
		}

		// otherwise, thread through all the normal cleanups.

		// store the index at the start
		llvm::ConstantInt* index = IRTypeBuilder::GetConstInt32(jumpDest.GetIndex());
		EmitStore(GetNormalCleanupDestSlot().Get(), index, mo_false, brInst);

		// adjust the branch instruction to point to the first cleanup block
		brInst.GetAs<llvm::BranchInst>()->setSuccessor(0, EnsureCleanupHasBasicBlock(*topCleanup));

		// add this destination to all the cleanups involved
		IRCleanupRef nextCleanup = topCleanup;
		IRCleanupRef destCleanup = jumpDest.GetCleanup();

		if(destCleanup.StrictlyEncloses(nextCleanup))
		{
			while(mo_true)
			{
				IRCleanup& cleanup = *nextCleanup;

				nextCleanup = cleanup.GetEnclosingCleanup();

				// if this is the last cleanup we're propagating through, tell it
				// that theres a resolved jump moving through it
				if(!destCleanup.StrictlyEncloses(nextCleanup))
				{
					cleanup.AddBranchAfter(index, jumpDest.GetBlock().Get());
					break;
				}

				// otherwise, tell the cleanup that theres a jump propagating through
				// it. If this isn't new information, all the rest of the work has 
				// been done before.
				if(!cleanup.AddBranchThrough(jumpDest.GetBlock().Get()))
					break;
			}
		}

		mBuilder.ClearInsertionPoint();
	}

	IRGlobalVariable IREmitter::EmitGlobalVariable(const UTF8String& name, llvm::Type* type)
	{
		return EmitGlobalVariable(name, type, mo_true, mo_null);	
	}

	IRGlobalVariable IREmitter::EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant)
	{
		return EmitGlobalVariable(name, type, isConstant, mo_null);
	}

	IRGlobalVariable IREmitter::EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant, llvm::Constant* initializer)
	{
		return EmitGlobalVariable(name, type, isConstant, llvm::GlobalValue::InternalLinkage, initializer);
	}

	IRGlobalVariable IREmitter::EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant, llvm::GlobalValue::LinkageTypes linkage, llvm::Constant* initializer)
	{
		return new llvm::GlobalVariable(*GetModule(), type, isConstant, linkage, initializer, name.GetChars());
	}

	IRValue IREmitter::EmitBitCast(IRValue value, IRType type, const UTF8String& name)
	{
		return EmitBitCast(value, IRTypeBuilder::GetType(type), name);
	}

	IRValue IREmitter::EmitBitCast(IRValue value, llvm::Type* type, const UTF8String& name)
	{
		return mBuilder.CreateBitCast(value.Get(), type, name.Get());
	}

	IRValue IREmitter::EmitPtrCast(IRValue value, IRType type, const UTF8String& name)
	{
		return EmitPtrCast(value, IRTypeBuilder::GetType(type), name);
	}

	IRValue IREmitter::EmitPtrCast(IRValue value, llvm::Type* type, const UTF8String& name)
	{
		return mBuilder.CreatePointerCast(value, type, name.GetChars());
	}

	IRValue IREmitter::EmitIntCast(IRValue value, IRType type, mo_bool isSigned, const UTF8String& name)
	{
		return EmitIntCast(value, IRTypeBuilder::GetType(type), isSigned, name);
	}

	IRValue IREmitter::EmitIntCast(IRValue value, llvm::Type* type, mo_bool isSigned, const UTF8String& name)
	{
		return mBuilder.CreateIntCast(value, type, isSigned, name.GetChars());
	}

	IRValue IREmitter::EmitIntToPtr(IRValue value, IRType type, const UTF8String& name)
	{
		return EmitIntToPtr(value, IRTypeBuilder::GetPointerType(type), name);
	}

	IRValue IREmitter::EmitIntToPtr(IRValue value, llvm::Type* type, const UTF8String& name)
	{
		return mBuilder.CreateIntToPtr(value, type, name.GetChars());
	}

	llvm::Value* IREmitter::EmitStubCall(const UTF8String& name, llvm::ArrayRef<llvm::Value*> args, const UTF8String& returnName)
	{
		// grab the arg array as a vector so we can insert the callframe
		std::vector<llvm::Value*> stubArgs = args.vec();

		// get the current call frame (if any)
		IRValue jsFrame = GetFunctionScope()->HasExecutionContext() ? GetFunctionScope()->GetExecutionContext() : mo_null;

		// assign a null pointer if there is no frame
		if(!jsFrame)
			jsFrame = IRTypeBuilder::GetConstNullPtr(IRJSStackFrameTy);

		// insert the js frame as the first argument
		stubArgs.insert(stubArgs.begin(), jsFrame);

		// now emit the actual function call
		return EmitCall(name, stubArgs, returnName);
	}

	llvm::Value* IREmitter::EmitCall(const UTF8String& name, llvm::Value* arg, const UTF8String& returnName)
	{
		return EmitCall1(GetFunction(name), arg, returnName);
	}

	llvm::Value* IREmitter::EmitCall(llvm::Value* func, const UTF8String& returnName)
	{
		return mBuilder.CreateCall(func, returnName.IsNull() ? EMIT_CALL_RETURN_VALUE_NAME(func) : returnName.GetChars());
	}

	llvm::Value* IREmitter::EmitCall(llvm::Value* func, llvm::ArrayRef<llvm::Value*> args, const UTF8String& returnName)
	{
		return mBuilder.CreateCall(func, args, returnName.IsNull() ? EMIT_CALL_RETURN_VALUE_NAME(func) : returnName.GetChars());
	}

	llvm::Value* IREmitter::EmitCall1(llvm::Value* func, llvm::Value* arg1, const UTF8String& returnName)
	{
		std::vector<llvm::Value*> args(1, arg1);
		return EmitCall(func, args, returnName);
	}

	llvm::Value* IREmitter::EmitCall2(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, const UTF8String& returnName)
	{
		std::vector<llvm::Value*> args;
		args.push_back(arg1); 
		args.push_back(arg2);
		
		return EmitCall(func, args, returnName);
	}

	llvm::Value* IREmitter::EmitCall3(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, const UTF8String& returnName)
	{
		std::vector<llvm::Value*> args;
		args.push_back(arg1); 
		args.push_back(arg2);
		args.push_back(arg3);

		return EmitCall(func, args, returnName);
	}

	llvm::Value* IREmitter::EmitCall4(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, const UTF8String& returnName)
	{
		std::vector<llvm::Value*> args;
		args.push_back(arg1); 
		args.push_back(arg2);
		args.push_back(arg3);
		args.push_back(arg4);

		return EmitCall(func, args, returnName);
	}

	llvm::Value* IREmitter::EmitCall5(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, llvm::Value* arg5, const UTF8String& returnName)
	{
		std::vector<llvm::Value*> args;
		args.push_back(arg1); 
		args.push_back(arg2);
		args.push_back(arg3);
		args.push_back(arg4);
		args.push_back(arg5);

		return EmitCall(func, args, returnName);
	}

	IRCallInst IREmitter::EmitCallWithAttrs(llvm::Value* func, mo_int32 attrs, const UTF8String& returnName)
	{
		return EmitCallWithAttrs(func, llvm::ArrayRef<llvm::Value*>(), attrs, returnName);
	}

	IRCallInst IREmitter::EmitCallWithAttrs(llvm::Value* func, llvm::ArrayRef<llvm::Value*> args, mo_int32 attrs, const UTF8String& returnName)
	{
		llvm::CallInst* call = mBuilder.CreateCall(func, args, returnName.IsNull() ? EMIT_CALL_RETURN_VALUE_NAME(func) : returnName.GetChars());

		if((attrs & kIRFunctionAttributeNoUnwind) != 0)
			call->setDoesNotThrow();

		if((attrs & kIRFunctionAttributeNoReturn) != 0)
			call->setDoesNotReturn();

		if((attrs & kIRFunctionAttributeReturnsTwice) != 0)
			call->setCanReturnTwice();

		return call;
	}

	IRValue IREmitter::EmitStringArray(const UTF8StringCollection& strings, const UTF8String& name)
	{
		IRValue arrStrings = IRValue::Null;

		if(!strings.empty())
		{
			std::vector<llvm::Value*> stringValues;

			for(mo_int32 i = 0, len = strings.size(); i < len; ++i)
			{
				IRConstant str = GetString(strings[i]);
				
				stringValues.push_back(EmitPtrCast(str, IRBytePtrTy, (str->hasName() ? (str->getName() + "_ptr").str() : "")).Get());
			}

			arrStrings = EmitArray(IRBytePtrTy, stringValues, name);
		}

		return arrStrings;
	}

	llvm::Value* IREmitter::EmitArray(IRType type, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name)
	{
		return EmitArray(IRTypeBuilder::GetType(type), values, name);
	}

	llvm::Value* IREmitter::EmitArray(llvm::Type* type, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name)
	{
		return EmitArray(EmitAlloc(type, values.size(), name), values, name);
	}

	llvm::Value* IREmitter::EmitArray(llvm::Value* arr, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name)
	{
		UTF8String arrName = (name.IsEmpty() ? "array" : name);
		mo_uint32 arrSize = values.size();

		for(mo_int32 i = 0; i < arrSize; ++i)
			EmitArrayStore(arr, values[i], i, mo_false, arrName + "_index_" + UTF8String::FromNumber(i));
			
		return arr;
	}


	//-------------------------------------------------------------------------------------------------
	// Allocation
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	IRValue IREmitter::EmitAlloc(IRType type, const UTF8String& name)
	{
		return EmitAlloc(type, -1, name);
	}

	IRValue IREmitter::EmitAlloc(llvm::Type* type, const UTF8String& name)
	{
		return EmitAlloc(type, -1, name);
	}

	IRValue IREmitter::EmitAlloc(IRType type, mo_int32 arraySize, const UTF8String& name)
	{
		return EmitAlloc(IRTypeBuilder::GetType(type), arraySize, name);
	}

	IRValue IREmitter::EmitAlloc(llvm::Type* type, mo_int32 arraySize, const UTF8String& name)
	{
		// assume array with a non-negative size
		if(arraySize >= 0)
			return new llvm::AllocaInst(llvm::ArrayType::get(type, arraySize), 0, name.Get(), GetFunctionScope()->GetAllocationInsertPoint());

		return new llvm::AllocaInst(type, 0, name.Get(), GetFunctionScope()->GetAllocationInsertPoint());
	}

	IRValue IREmitter::EmitAllocStruct(llvm::StructType* type, const UTF8String& name)
	{
		IRValue var = EmitAlloc(type, name);
		IRValue dataPtr = var;
		llvm::PointerType* dataPtrType =llvm::cast<llvm::PointerType>(dataPtr->getType());
		
		if(!dataPtrType->getElementType()->isIntegerTy(8))
			dataPtr = EmitBitCast(dataPtr, mBuilder.getInt8PtrTy(dataPtrType->getAddressSpace()), type->hasName() ? UTF8String(type->getName().str()) + "Pointer" : "");

		EmitMemSet(dataPtr, IRTypeBuilder::GetConstInt8(0), GetStructSize(type));

		return var;
	}

	IRValue IREmitter::EmitAllocWithValue(IRType type, const IRValue& value, const UTF8String& name)
	{
		return EmitAllocWithValue(IRTypeBuilder::GetType(type), value, name);
	}

	IRValue IREmitter::EmitAllocWithValue(llvm::Type* type, const IRValue& value, const UTF8String& name)
	{
		return EmitStore(EmitAlloc(type, name), value).GetAs<llvm::StoreInst>()->getPointerOperand();
	}



	//-------------------------------------------------------------------------------------------------
	// Store
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	IRValue IREmitter::EmitStore(const IRValue& ptr, const IRValue& value, mo_bool isVolatile, const IRInstruction& insertBeforeInst)
	{
		if(!insertBeforeInst.IsNull())
			return new llvm::StoreInst(value.Get(), ptr.Get(), insertBeforeInst.Get());

		return mBuilder.CreateStore(value.Get(), ptr.Get(), isVolatile);
	}

	IRValue IREmitter::EmitStoreNull(const IRValue& ptr, IRType type, mo_bool isVolatile)
	{
		return EmitStore(ptr, IRTypeBuilder::GetConstNullPtr(type), isVolatile);
	}

	IRValue IREmitter::EmitArrayStore(const IRValue& arrayPtr, const IRValue& value, mo_int32 index, mo_bool isVolatile, const UTF8String& elementName)
	{
		return EmitStore(EmitGEP2(arrayPtr.Get(), 0, index, 1, elementName), value, isVolatile);
	}



	llvm::LoadInst* IREmitter::EmitLoad(const IRValue& ptr, const UTF8String& name)
	{
		return mBuilder.CreateLoad(ptr.Get(), mo_false, name.GetChars());
	}

	llvm::LoadInst* IREmitter::EmitVolatileLoad(const IRValue& ptr, const UTF8String& name)
	{
		return mBuilder.CreateLoad(ptr.Get(), mo_true, name.GetChars());
	}

	llvm::Value* IREmitter::EmitGEP(llvm::Value* ptr, mo_int32 index, mo_int32 count, const UTF8String& name)
	{
		std::vector<llvm::Value*> indices(count, IRTypeBuilder::GetConstInt32(index));
		
		return mBuilder.CreateGEP(ptr, indices, name.GetChars());
	}

	llvm::Value* IREmitter::EmitGEP2(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 count, const UTF8String& name)
	{
		std::vector<llvm::Value*> indices(count, IRTypeBuilder::GetConstInt32(index2));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(index1));

		return mBuilder.CreateGEP(ptr, indices, name.GetChars());
	}

	llvm::Value* IREmitter::EmitGEP3(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 index3, mo_int32 count, const UTF8String& name)
	{
		std::vector<llvm::Value*> indices(count, IRTypeBuilder::GetConstInt32(index3));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(index2));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(index1));

		return mBuilder.CreateGEP(ptr, indices, name.GetChars());
	}

	llvm::Value* IREmitter::EmitStructGEP(llvm::Value* ptr, mo_int32 index, mo_int32 count, const UTF8String& name)
	{
		return EmitGEP2(ptr, 0, index, count, name);
	}

	llvm::Value* IREmitter::EmitStructGEP2(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 count, const UTF8String& name)
	{
		return EmitGEP3(ptr, 0, index1, index2, count, name);
	}

	llvm::Value* IREmitter::EmitStructGEP3(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 index3, mo_int32 count, const UTF8String& name)
	{
		std::vector<llvm::Value*> indices(count, IRTypeBuilder::GetConstInt32(index3));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(index2));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(index1));
		indices.insert(indices.begin(), IRTypeBuilder::GetConstInt32(0));

		return mBuilder.CreateGEP(ptr, indices, name.GetChars());
	}

	void IREmitter::EmitMemSet(const IRValue& ptr, const IRValue& value, mo_uint64 size, mo_uint32 align, mo_bool isVolatile)
	{
		EmitMemSet(ptr, value, IRTypeBuilder::GetConstUInt64(size), align, isVolatile);
	}

	void IREmitter::EmitMemSet(const IRValue& ptr, const IRValue& value, const IRValue& size, mo_uint32 align, mo_bool isVolatile)
	{
		mBuilder.CreateMemSet(ptr.Get(), value.Get(), size.Get(), align, isVolatile);
	}

	IRValue IREmitter::EmitDecodedTagValue(IRValue encodedValue, const UTF8String& name)
	{
		IRValue ptr = EmitAllocWithValue(IRInt64Ty, encodedValue);
		IRValue valueBits = mBuilder.CreateBitCast(ptr, IRTypeBuilder::GetPointerType(IRInt32PtrTy), "decoded_value_tag_bits");
		IRValue tagBits = EmitGEP(valueBits, 1, 1, "decoded_value_tag_ptr");

		return EmitLoad(tagBits, "decoded_value_tag");
	}

	IRValue IREmitter::EmitDecodedPayloadValue(IRValue encodedValue, const UTF8String& name)
	{
		IRValue ptr = EmitAllocWithValue(IRInt64Ty, encodedValue);
		IRValue valueBits = mBuilder.CreateBitCast(ptr, IRTypeBuilder::GetPointerType(IRInt32PtrTy), "decoded_value_payload_bits");
		IRValue payloadBits = EmitGEP(valueBits, 0, 1, "decoded_value_payload_ptr");

		return EmitLoad(payloadBits, "decoded_value_payload");
	}

	IRValue IREmitter::EmitEncodedValue(IRValue payload, IRValue tag, const UTF8String& name)
	{
		return EmitCall2(IRConsts::JSEncodeValueFunctionName, payload, tag, name);
	}

	IRValue IREmitter::EmitEncodedStringValue(const UTF8String& str, const UTF8String& name)
	{
		IRConstant strConst = GetString(str);
		UTF8String strConstName = (name.IsNull() ? UTF8String(strConst->getName().str()) : name);

		return EmitEncodedValue(EmitPtrCast(IRTypeBuilder::GetStringPtr(strConst), IRInt32Ty), IRTypeBuilder::GetConstInt32(JSValueDataStringTag), IRConsts::JSEncodedValueName + strConstName);
	}

	llvm::Value* IREmitter::LoadEmptyValue(const UTF8String& name)
	{
		return mBuilder.CreateLoad(FindGlobal(IRConsts::JSValueEmptyName), name.GetChars());
	}

	llvm::Value* IREmitter::LoadUndefinedValue(const UTF8String& name)
	{
		return mBuilder.CreateLoad(FindGlobal(IRConsts::JSValueUndefinedName), name.GetChars());
	}

	llvm::Value* IREmitter::LoadNullValue(const UTF8String& name)
	{
		return mBuilder.CreateLoad(FindGlobal(IRConsts::JSValueNullName), name.GetChars());
	}

	llvm::Value* IREmitter::LoadTrueValue(const UTF8String& name)
	{
		return mBuilder.CreateLoad(FindGlobal(IRConsts::JSValueTrueName), name.GetChars());
	}

	llvm::Value* IREmitter::LoadFalseValue(const UTF8String& name)
	{
		return mBuilder.CreateLoad(FindGlobal(IRConsts::JSValueFalseName), name.GetChars());
	}

	IRValue IREmitter::ValueWithName(const IRValue& value, const UTF8String& name)
	{
		if(!value.IsNull())
			value->setName(name.GetChars());

		return value;
	}

	const llvm::StructLayout* IREmitter::GetStructLayout(llvm::StructType* type)
	{
		return GetDataLayout()->getStructLayout(type);
	}

	mo_uint64 IREmitter::GetStructSize(llvm::StructType* type)
	{
		return GetStructLayout(type)->getSizeInBytes();
	}

	IRConstant IREmitter::GetString(const UTF8String& str)
	{
		IRConstant stringValue = mStringTable.Get(str);
		IRConstant stringValue_OLD = mStringTable_OLD.Get(str);
		
		if(!stringValue_OLD.IsNull())
			return stringValue_OLD;
			
		stringValue = IRTypeBuilder::GetConstString(str);
		stringValue_OLD = IRTypeBuilder::GetConstString(str);

		mStringTable.Set(str, stringValue);

		return mStringTable_OLD.Set(str, EmitGlobalVariable(MO_IR_STRING_NAME(UTF8String::FromNumber(mStringTableID.GetNextId())), stringValue_OLD->getType(), mo_true, llvm::GlobalValue::PrivateLinkage, stringValue_OLD));
	}

	IRValue IREmitter::GetEncodedString(const UTF8String& str)
	{
		UTF8String encStringName;
		IRValue stringValue = mEncodedStringTable.Get(str);
		
		if(!stringValue.IsNull())
		{
			encStringName = UTF8String(stringValue->getName().data()) + "_value";
			return EmitLoad(stringValue, encStringName);
		}

		encStringName = MO_IR_ENCODED_STRING_NAME(UTF8String::FromNumber(mEncodedStringTableID.GetNextId()));
		IRConstant encGlobalVar = EmitGlobalVariable(encStringName, IRTypeBuilder::GetType(IRJSValueTy), mo_false, llvm::GlobalValue::PrivateLinkage, mGenerator->JSValueUndefinedConstant);

		return EmitLoad(mEncodedStringTable.Set(str, encGlobalVar), encStringName + "_value");
	}

	llvm::GlobalVariable* IREmitter::FindGlobal(const UTF8String& name)
	{
		return GetModule()->getNamedGlobal(name.GetChars());
	}

	llvm::Value* IREmitter::FindSymbol(const UTF8String& name)
	{
		// first see if the variable is in our local scope
		llvm::ValueSymbolTable* symTable = GetInsertBasicBlock()->getValueSymbolTable();
		llvm::Value* variable = symTable->lookup(name.GetChars());

		// when it does not exist in the local scope then look in
		// our global scope for it there
		if(variable == mo_null)
			variable = GetModule()->getNamedGlobal(name.GetChars());

		// TODO - implement variables that have not yet been created
		if(variable == mo_null)
		{
			mo_debug_print("unable to find symbol with the name: %s\n", name.GetChars());
			return mo_null;
		}

		return variable;
	}

	IRBasicBlock IREmitter::GetUnreachableBlock()
	{
		IRBasicBlock block = GetFunctionScope()->GetUnreachableBlock();

		if(block.IsNull())
		{
			block = NewBasicBlock(IR_BLOCK_NAME("Unreachable"));
			new llvm::UnreachableInst(GetContext(), block.Get());

			GetFunctionScope()->SetUnreachableBlock(block);
		}

		return block;
	}

	IRInstruction IREmitter::EmitUnreachable()
	{
		return mBuilder.CreateUnreachable();
	}


	//-------------------------------------------------------------------------------------------------
	// Exception Handling / Cleanup
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	IRCleanupRef IREmitter::GetCleanupStackDepth() const
	{
		return mCleanupStack.begin();
	}

	IRCleanupRef IREmitter::GetInnermostCleanup() const
	{
		return mCleanupStack.GetInnermostCleanup();
	}

	static void DestroyOptimisticNormalEntry(IREmitter* emitter, IRCleanup& cleanup)
	{
		IRBasicBlock entryBlock = cleanup.GetEntryBlock();

		if(entryBlock.IsNull())
			return;

		IRBasicBlock unreachableBlock = emitter->GetUnreachableBlock();

		for(llvm::BasicBlock::use_iterator i = entryBlock->use_begin(), e = entryBlock->use_end(); i != e;)
		{
			llvm::Use& use = i.getUse();
			++i;

			use.set(unreachableBlock.Get());

			llvm::SwitchInst* si = llvm::cast<llvm::SwitchInst>(use.getUser());

			if(si->getNumCases() == 1 && si->getDefaultDest() == unreachableBlock.Get())
			{
				llvm::BranchInst::Create(si->case_begin().getCaseSuccessor(), si);
				llvm::LoadInst* condition = llvm::cast<llvm::LoadInst>(si->getCondition());

				si->eraseFromParent();

				MO_ASSERT(condition->getOperand(0) == emitter->GetFunctionScope()->GetNormalCleanupDestSlot().Get());
				MO_ASSERT(condition->use_empty());

				condition->eraseFromParent();
			}
		}

		MO_ASSERT(entryBlock->use_empty());
		entryBlock.Delete();
	}

	void IREmitter::PopCleanupBlocks(IRCleanupRef& endDepth)
	{
		MO_ASSERT(endDepth.IsValid());

		while(mCleanupStack.begin() != endDepth)
		{
			IRCleanup& cleaup = *mCleanupStack.begin();
			mo_bool isBranchThough = endDepth.StrictlyEncloses(cleaup.GetEnclosingCleanup());

			PopCleanupBlock(isBranchThough);
		}
	}

	void IREmitter::PopCleanupBlock(mo_bool fallthroughIsBranchThrough)
	{
		MO_ASSERT(!mCleanupStack.IsEmpty() && "cleanup stack is empty.");

		IRCleanup& cleanup = *mCleanupStack.begin();

		// check the two conditions which might require a normal cleanup:

		// - whether there are branch-throughs or branch-afters
		mo_bool hasExistingBranches = cleanup.HasBranches();

		// - whether theres a fallthrough
		IRBasicBlock sourceFallthroughBlock = GetInsertBasicBlock();
		mo_bool hasFallthrough = !sourceFallthroughBlock.IsNull();

		// branch-through fall-throughs leave the insertion point set to the
		// end of the last cleanup, which points to the current scope. The rest
		// of IR gen doesn't need to worry about this, it only happens during
		// the execution of PopCleanupBlocks()
		mo_bool hasPrebranchedFallthrough = (!sourceFallthroughBlock.IsNull() && sourceFallthroughBlock->getTerminator() != 0);

		// if this is a normal cleanup, then having a prebranched fallthrough
		// implies that the fallthrough source unconditionally jumps here.
		MO_ASSERT(
			!hasPrebranchedFallthrough || (
			!cleanup.GetEntryBlock().IsNull() && sourceFallthroughBlock->getTerminator()->getSuccessor(0) == cleanup.GetEntryBlock().Get()));

		mo_bool requiresNormalCleanup = mo_false;

		if(hasExistingBranches || hasFallthrough)
			requiresNormalCleanup = mo_true;

		// if we don't need the cleanup at all, we're done
		if(!requiresNormalCleanup)
		{
			DestroyOptimisticNormalEntry(this, cleanup);
			mCleanupStack.Pop();
			return;
		}

		IRCleanupTask* cleanupTask = cleanup.GetTask();
			
		// if there is just a fallthough and no other need for this cleanup, then we
		// can just emit it directly.
		//
		// fallthrough = yes
		// prebranched-fallthrough = no
		// existing-branches = no
		if(hasFallthrough && !hasPrebranchedFallthrough && !hasExistingBranches)
		{
			DestroyOptimisticNormalEntry(this, cleanup);
			mCleanupStack.Pop();

			EmitCleanup(cleanupTask);
			return;
		}

		// otherwise, the best approach is to thread everything through the cleanup
		// block and then try to cleanup after ourself.


		// make sure the cleanup has an entry block
		IRBasicBlock entryBlock = EnsureCleanupHasBasicBlock(cleanup);

		// if there is no fallthrough then there also should not be a fallthrough block
		if(!hasFallthrough)
			MO_ASSERT(sourceFallthroughBlock.IsNull() && "source without fallthrough");
		else
		{
			// otherwise, if we do have a fallthrough, we need to save the cleanup destination.
			// this is always zero for a fallthrough
			if(!hasPrebranchedFallthrough)
				mBuilder.CreateStore(mBuilder.getInt32(0), GetNormalCleanupDestSlot().Get());
		} 

		// emit the entry block, this will implicity branch to it if there is a fallthrough and
		// existing branches should already be branched to it.
		EmitBasicBlock(entryBlock);

		// figure out where we are going and create the cleanup epilogue
		mo_bool hasEnclosingCleanups = (cleanup.GetEnclosingCleanup() != mCleanupStack.end());

		
		// compute the branch-through destination if we need it:
		//  - if there are branch-throughs threaded through the scope
		//  - if fall-through is a branch-through
		//  - if there are fixups that will be optimistically forwarded
		//    to the enclosing cleanup
		//
		IRBasicBlock branchThroughDestBlock = 0;

		if(cleanup.HasBranchThoughs() || (!sourceFallthroughBlock.IsNull() && fallthroughIsBranchThrough))
		{
			MO_ASSERT(hasEnclosingCleanups);

			IRCleanupRef s = cleanup.GetEnclosingCleanup();
			branchThroughDestBlock = EnsureCleanupHasBasicBlock(*s);
		}

		IRBasicBlock fallthroughDestBlock = 0;
		llvm::SmallVector<llvm::Instruction*, 2> instrsToAppend;

		// if there is exactly one branch-after and no other threads, we can 
		// route it without a switch
		if(!cleanup.HasBranchThoughs() && !hasFallthrough && cleanup.GetBranchAfterCount() == 1)
		{
			MO_ASSERT(branchThroughDestBlock.IsNull());

			// FIXME: cleanup the dead stores to the cleanup dest slot
			IRBasicBlock branchAfterBlock = cleanup.GetBranchAfterBlock(0);
			instrsToAppend.push_back(llvm::BranchInst::Create(branchAfterBlock.Get()));
		}

		// build a switch-out if we need it:
		//   - if there are branch-afters thread through the scope
		//   - if fall-through is a branch-after
		else if(cleanup.GetBranchAfterCount() > 0 || (hasFallthrough && !fallthroughIsBranchThrough))
		{
			IRBasicBlock defaultBlock = (branchThroughDestBlock.IsNull() ? GetUnreachableBlock() : branchThroughDestBlock);

			// FIXME: need to base this on the actual number of branch-afters..
			const mo_uint32 switchCapacity = 10;

			llvm::LoadInst* load = new llvm::LoadInst(GetNormalCleanupDestSlot().Get(), IR_INTERNAL_VAR_NAME("cleanup_dest"));
			llvm::SwitchInst* sw = llvm::SwitchInst::Create(load, defaultBlock.Get(), switchCapacity);

			instrsToAppend.push_back(load);
			instrsToAppend.push_back(sw);

			// branch-after fallthrough
			if(!sourceFallthroughBlock.IsNull() && !fallthroughIsBranchThrough)
			{
				fallthroughDestBlock = NewBasicBlock(IR_BLOCK_NAME("CleanupContinue"));

				if(hasFallthrough)
					sw->addCase(mBuilder.getInt32(0), fallthroughDestBlock.Get());
			}

			for(mo_uint32 i = 0, e = cleanup.GetBranchAfterCount(); i != e; ++i)
				sw->addCase(cleanup.GetBranchAfterIndex(i), cleanup.GetBranchAfterBlock(i));
		}
		else
		{
			MO_ASSERT(!branchThroughDestBlock.IsNull());
			instrsToAppend.push_back(llvm::BranchInst::Create(branchThroughDestBlock.Get()));
		}

		// pop the cleanup and emit it
		mCleanupStack.Pop();
		MO_ASSERT(mCleanupStack.IsEmpty() != hasEnclosingCleanups);

		EmitCleanup(cleanupTask);

		// append the prepared cleanup prologue from above
		IRBasicBlock normalExitBlock = GetInsertBasicBlock();

		for(mo_uint32 i = 0, e = instrsToAppend.size(); i != e; ++i)
			normalExitBlock->getInstList().push_back(instrsToAppend[i]);

		// setup the fallthrough edge

		MO_ASSERT(hasFallthrough == !sourceFallthroughBlock.IsNull());
		
		// a fallthrough source exists and should branch to the cleanup, but were
		// not supposed to branch through to the next cleanup
		if(hasFallthrough && !fallthroughDestBlock.IsNull())
		{
			MO_ASSERT(!fallthroughIsBranchThrough);
			EmitBasicBlock(fallthroughDestBlock);
		}
		else
		{
			// no fallthrough source exists
			if(!hasFallthrough)
				mBuilder.ClearInsertionPoint();

			// otherwise, a fallthrough source exists and should branch to the
			// cleanup and then through to the next, everything should already
			// be setup for this.
		}

		IRBasicBlock newNormalEntryBlock = SimplifyCleanupEntry(entryBlock);

		if(newNormalEntryBlock != entryBlock && entryBlock == normalExitBlock)
		{
			MO_ASSERT(false && "branch fixups will be needed!");
		}
	}

	IRJumpDestination IREmitter::GetJumpDestination(const IRBasicBlock& targetBlock)
	{
		return IRJumpDestination(targetBlock, mCleanupStack.GetInnermostCleanup(), mCleanupDestID.GetNextId());
	}

	IRJumpDestination IREmitter::GetJumpDestination(const UTF8String& name)
	{
		return GetJumpDestination(NewBasicBlock(name));
	}

	void IREmitter::EmitCleanup(IRCleanupTask* task)
	{
		task->Emit(this);
		MO_ASSERT(InBasicBlock() && "cleanup ended with no insertion point?");
	}

	IRBasicBlock IREmitter::EnsureCleanupHasBasicBlock(IRCleanup& cleanup)
	{
		IRBasicBlock entryBlock = cleanup.GetEntryBlock();

		if(entryBlock.IsNull())
		{
			entryBlock = NewBasicBlock(cleanup.GetTask() ? cleanup.GetTask()->GetEntryBlockName() : IR_BLOCK_NAME("Cleanup"));
			cleanup.SetEntryBlock(entryBlock);
		}

		return entryBlock;
	}

	IRBasicBlock IREmitter::SimplifyCleanupEntry(const IRBasicBlock& entryBlock)
	{
		IRBasicBlock predBlock = entryBlock->getSinglePredecessor();

		if(predBlock.IsNull())
			return entryBlock;

		llvm::BranchInst* br = llvm::dyn_cast<llvm::BranchInst>(predBlock->getTerminator());

		if(!br || br->isConditional())
			return entryBlock;

		MO_ASSERT(br->getSuccessor(0) == entryBlock.Get());

		mo_bool wasInsertBlock = (GetInsertBasicBlock() == entryBlock);

		MO_ASSERT(!wasInsertBlock || mBuilder.GetInsertPoint() == entryBlock->end());

		br->eraseFromParent();

		entryBlock->replaceAllUsesWith(predBlock.Get());

		predBlock->getInstList().splice(predBlock->end(), entryBlock->getInstList());

		entryBlock->eraseFromParent();

		if(wasInsertBlock)
			mBuilder.SetInsertPoint(predBlock.Get());

		return predBlock;
	}

	mo_bool IREmitter::CanBranchWithoutCleanups(const IRJumpDestination& dest)
	{
		MO_ASSERT(dest.GetCleanup().Encloses(mCleanupStack.begin()) && "jump destination no longer exists");

		IRCleanupRef topCleanup = mCleanupStack.GetInnermostCleanup();

		if(topCleanup == mCleanupStack.end() || topCleanup.Encloses(dest.GetCleanup()))
			return mo_true;

		return mo_false;
	}

	void IREmitter::EmitCallToExceptionThrow(const IRValue& exceptionValue)
	{
		std::vector<llvm::Value*> args;
		IRFunctionScope* scope = GetFunctionScope();

		args.push_back(scope->GetExecutionContext().Get());
		args.push_back(exceptionValue.Get());

		EmitCallWithAttrs(mTypes.GetExceptionThrowFn(), args, kIRFunctionAttributeNoUnwind)->setDoesNotReturn();
	}

	IRValue IREmitter::EmitCallToExceptionExtract(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;
		IRFunctionScope* scope = GetFunctionScope();

		args.push_back(scope->GetExecutionContext().Get());
		args.push_back(exceptionData.Get());

		return EmitCallWithAttrs(mTypes.GetExceptionExtractFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionTryEnter(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;
		IRFunctionScope* scope = GetFunctionScope();

		args.push_back(scope->GetExecutionContext().Get());
		args.push_back(exceptionData.Get());

		EmitCallWithAttrs(mTypes.GetExceptionTryEnterFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionTryExit(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;
		IRFunctionScope* scope = GetFunctionScope();

		args.push_back(scope->GetExecutionContext().Get());
		args.push_back(exceptionData.Get());

		EmitCallWithAttrs(mTypes.GetExceptionTryExitFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionCatchEnter()
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRValue catchVarNameValue = IRTypeBuilder::GetStringPtr(GetString(scope->GetExceptionCatchVariableName()));
		IRValue exceptionValue = scope->GetExceptionValue();

		std::vector<llvm::Value*> args;

		args.push_back(scope->GetExecutionContext().Get());
		args.push_back(catchVarNameValue.Get());
		args.push_back(exceptionValue);

		EmitCallWithAttrs(mTypes.GetExceptionCatchEnterFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionCatchExit()
	{
		IRFunctionScope* scope = GetFunctionScope();

		EmitCallWithAttrs(mTypes.GetExceptionCatchExitFn(), scope->GetExecutionContext().Get(), kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionSetGlobalHandler(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;

		args.push_back(mNativeContext.Get());
		args.push_back(exceptionData.Get());

		EmitCallWithAttrs(mTypes.GetExceptionSetGlobalHandlerFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToExceptionReport(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;

		args.push_back(mNativeContext.Get());
		args.push_back(exceptionData.Get());

		EmitCallWithAttrs(mTypes.GetExceptionReportFn(), args, kIRFunctionAttributeNoUnwind);
	}

	IRValue IREmitter::EmitCallToExceptionErrorCode(const IRValue& exceptionData)
	{
		std::vector<llvm::Value*> args;

		args.push_back(mNativeContext.Get());
		args.push_back(exceptionData.Get());

		return EmitCallWithAttrs(mTypes.GetExceptionErrorCodeFn(), args, kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToInitialize()
	{
		EmitCallWithAttrs(mTypes.GetInitializeFn(), mNativeContext.Get(), kIRFunctionAttributeNoUnwind);
	}

	IRValue IREmitter::EmitCallToInitializeEnvironment()
	{
		return EmitCallWithAttrs(mTypes.GetInitializeEnvironmentFn(), mNativeContext.Get(), kIRFunctionAttributeNoUnwind);
	}

	void IREmitter::EmitCallToShutdown()
	{
		EmitCallWithAttrs(mTypes.GetShutdownFn(), mNativeContext.Get(), kIRFunctionAttributeNoUnwind);
	}

	//-------------------------------------------------------------------------------------------------
	// Functions
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	IRFunction IREmitter::EmitFunction(const UTF8String& name, llvm::FunctionType* type, llvm::GlobalValue::LinkageTypes linkage)
	{
		return llvm::Function::Create(type, linkage, name.GetChars(), GetModule());
	}

	void IREmitter::EmitReturnBlock(IRJumpDestination& returnDest)
	{
		// to stay organized, we try to avoid emitting the return block for
		// simple cases
		IRBasicBlock currentBlock = GetInsertBasicBlock();

		if(!currentBlock.IsNull())
		{
			MO_ASSERT(!currentBlock->getTerminator() && "Unexpected terminated block.");

			// we have a valid insert point, reuse it if it is empty or there
			// are no explicit jumps to the return block
			if(currentBlock->empty() || returnDest.GetBlock()->use_empty())
			{
				returnDest.GetBlock()->replaceAllUsesWith(currentBlock);
				returnDest.GetBlock().Delete();
				return;
			}

			EmitBasicBlock(returnDest.GetBlock());
			return;
		}

		// otherwise, if the return block is the target of a single direct
		// branch then we can just put the code in that block instead. this
		// cleans up functions which started with a unified return block.
		if(returnDest.GetBlock()->hasOneUse())
		{
			llvm::BranchInst* BI = llvm::dyn_cast<llvm::BranchInst>(*returnDest.GetBlock()->use_begin());

			if(BI && BI->isUnconditional() && BI->getSuccessor(0) == returnDest.GetBlock().Get())
			{
				mBuilder.SetInsertPoint(BI->getParent());
				BI->eraseFromParent();

				returnDest.GetBlock().Delete();
				return;
			}
		}

		EmitBasicBlock(returnDest.GetBlock(), mo_true);
	}

	IRFunction IREmitter::BeginFunction(IRFunctionType type, const UTF8String& name, llvm::FunctionType* signature, llvm::GlobalValue::LinkageTypes linkage)
	{
		return BeginFunction(type, EmitFunction(name, signature, linkage));
	}

	IRFunction IREmitter::BeginFunction(IRFunctionType type, const IRFunction& function)
	{
		// TODO : Setup an alloca marker that we can use to insert other
		//        alloca's at while emitting code (i.e. the InitializeStrings
		//        function can go away)
		//
		// See CodeGenFunction.cpp:519

		// create a new function scope with initial entry and return blocks
		IRBasicBlock entryBlock = NewEntryBasicBlock(function);
		IRFunctionScope* scope = new IRFunctionScope(type, function, entryBlock, GetJumpDestination(IR_BLOCK_NAME("Return")));

		// save the current insert point, this will allow us to restore it when
		// this scope is finished
		scope->SetRestorePoint(mBuilder.saveIP());

		// push the new scope into our stack, this now becomes the currently
		// emitting function
		mFunctionScopeStack.push_back(scope);
		

		// begin emitting with the entry block
		BeginBasicBlock(entryBlock);

		// return the function, this is more useful than
		// returning the new scope as the function is used
		// more often right after this call
		return function;
	}

	IRFunction IREmitter::EndFunction()
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRFunction func = scope->GetFunction();
		IRValue returnValue = scope->GetReturnValue();

		MO_ASSERT(mCleanupStack.IsEmpty());

		// emit the return block and return value to close out
		// this function
		EmitReturnBlock(scope->GetReturnDestination());

		if(returnValue.IsNull())
			mBuilder.CreateRetVoid();
		else
			mBuilder.CreateRet(mBuilder.CreateLoad(returnValue));
			
		IRInstruction allocPoint = scope->GetAllocationInsertPoint();
		allocPoint->removeFromParent();

		EmitBasicBlockIfUsed(scope->GetUnreachableBlock());

		mBuilder.restoreIP(scope->GetRestorePoint());

		// remove scope
		mFunctionScopeStack.pop_back();

		// TODO: might be good to do a consistency check before deleting
		//       the function scope, i.e. have break/continue scopes all
		//       been finished, switches, etc...?
		delete scope;

		// TODO: don't return the function back, currently I don't think
		//       this is used anywhere so there is no need for it.
		//
		// return the function that was ended in case
		// the caller needs it

		return func;	
	}

	IRFunctionScope* IREmitter::GetFunctionScope() 
	{ 
		MO_ASSERT(!mFunctionScopeStack.empty() && "GetFunctionScope: No functions currently in scope.");

		return mFunctionScopeStack.back();
	}

	IRFunction IREmitter::GetFunction() 
	{ 
		return GetFunctionScope()->GetFunction();
	}

	IRFunction IREmitter::GetFunction(const UTF8String& name)
	{
		return GetModule()->getFunction(name.GetChars());
	}

	IRConstant IREmitter::GetOrInsertFunction(const UTF8String& name, llvm::Type* type, llvm::CallingConv::ID callConv)
	{
		llvm::GlobalValue* entry = GetModule()->getNamedValue(name.GetChars());

		if(entry)
		{
			if(!type || entry->getType()->getElementType() == type)
				return llvm::dyn_cast<llvm::Function>(entry);

			return llvm::ConstantExpr::getBitCast(entry, type->getPointerTo());
		}
		
		mo_bool isIncompleteFunction = mo_false;
		llvm::FunctionType* fnTy = 0;

		if(type && llvm::isa<llvm::FunctionType>(type))
			fnTy = llvm::cast<llvm::FunctionType>(type);
		else
		{
			fnTy = llvm::FunctionType::get(IRTypeBuilder::GetType(IRVoidTy), mo_false);
			isIncompleteFunction = mo_true;
		}

		llvm::Function* fn = llvm::Function::Create(fnTy, llvm::GlobalValue::ExternalLinkage, name.GetChars(), GetModule());
		
		if(!isIncompleteFunction)
		{
			MO_ASSERT(fn->getType()->getElementType() == type);

			fn->setCallingConv(callConv);
			return fn;
		}

		llvm::Type* ptrTy = llvm::PointerType::getUnqual(type);
		return llvm::ConstantExpr::getBitCast(fn, ptrTy);
	}

	IRValue IREmitter::GetNormalCleanupDestSlot()
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRValue slot = scope->GetNormalCleanupDestSlot();

		if(slot.IsNull())
		{
			slot = EmitAlloc(IRTypeBuilder::GetType(IRInt32Ty), IR_INTERNAL_VAR_NAME("cleanup_dest_slot"));
			scope->SetNormalCleanupDestSlot(slot);
		}

		return slot;
	}

	mo_bool IREmitter::IsJSFunction(const IRFunction& function)
	{
		return function->getName().startswith(MO_IR_JS_FUNCTION_PREFIX);
	}

	void IREmitter::MoveFunctionToFront(const IRFunction& function)
	{
		llvm::Module::FunctionListType& funcList = GetModule()->getFunctionList();

		function->removeFromParent();
		funcList.push_front(function.Get());
	}


	//-------------------------------------------------------------------------------------------------
	// Basic Blocks
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	IRBasicBlock IREmitter::NewEntryBasicBlock(const IRFunction parent)
	{
		return NewBasicBlock("Entry", parent);
	}

	IRBasicBlock IREmitter::NewBasicBlock(const UTF8String& name, const IRFunction parent, const IRBasicBlock& beforeBlock)
	{
		return llvm::BasicBlock::Create(GetContext(), name.GetChars(), parent.Get(), beforeBlock.Get());
	}

	IRBasicBlock IREmitter::EmitBasicBlock(IRBasicBlock& block, mo_bool isFinished)
	{
		IRBasicBlock currentBlock = GetInsertBasicBlock();

		EmitBranch(block);

		if(isFinished && block->use_empty())
		{
			block.Delete();
			return IRBasicBlock::Null;
		}

		if(currentBlock && currentBlock->getParent())
			GetFunction()->getBasicBlockList().insertAfter(currentBlock.Get(), block.Get());
		else
			GetFunction()->getBasicBlockList().push_back(block.Get());

		return BeginBasicBlock(block);
	}

	IRBasicBlock IREmitter::EmitBasicBlockBefore(const IRBasicBlock& insertBeforeBlock, const UTF8String& name)
	{
		MO_ASSERT(!insertBeforeBlock.IsNull());

		return llvm::BasicBlock::Create(GetContext(), name.GetChars(), insertBeforeBlock->getParent(), insertBeforeBlock.Get());
	}

	void IREmitter::EmitBasicBlockIfUsed(IRBasicBlock& block)
	{
		if(block.IsNull())
			return;

		if(!block->use_empty())
		{
			GetFunction()->getBasicBlockList().push_back(block.Get());
			return;
		}

		block.Delete();
	}

	IRBasicBlock IREmitter::EnsureBasicBlock(const UTF8String& name)
	{
		if(!InBasicBlock())
			return EmitBasicBlock(NewBasicBlock(name));

		return GetInsertBasicBlock();
	}

	IRBasicBlock IREmitter::BeginBasicBlock(const IRBasicBlock& block)
	{
		mBuilder.SetInsertPoint(block.Get());
		return block;
	}

	IRBasicBlock IREmitter::EndBasicBlock()
	{
		IRBasicBlock currentBlock = GetInsertBasicBlock();
		mBuilder.ClearInsertionPoint();

		return currentBlock;
	}

	IRBasicBlock IREmitter::GetInsertBasicBlock()
	{
		return mBuilder.GetInsertBlock();
	}

	mo_bool IREmitter::InBasicBlock()
	{
		return !GetInsertBasicBlock().IsNull();
	}

	mo_bool IREmitter::HasPredecessors(const IRBasicBlock& block) const
	{
		llvm::pred_iterator prIter = llvm::pred_begin(block.Get());
		llvm::pred_iterator prEnd = llvm::pred_end(block.Get());

		return (prIter != prEnd);
	}

	mo_bool IREmitter::ReduceForwardingBlocks(const IRBasicBlock& block)
	{
		llvm::BranchInst* BI = llvm::dyn_cast<llvm::BranchInst>(block->getTerminator());

		// not worth the effort to try and reduce this block if we have
		// any cleanups, just leave it be
		if(!mCleanupStack.IsEmpty())
			return mo_false;

		// only direct branches and empty blocks can be reduced
		if(!BI || !BI->isUnconditional() || BI != block->begin())
			return mo_false;

		block->replaceAllUsesWith(BI->getSuccessor(0));
		BI->eraseFromParent();
		block->eraseFromParent();
		return mo_true;
	}
}