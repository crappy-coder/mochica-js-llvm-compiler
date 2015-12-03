#ifndef MOENJIN_CODEGEN_IREMITTER_H
#define MOENJIN_CODEGEN_IREMITTER_H

#include "Platform/Configuration.h"
#include "Platform/LLVMDecls.h"
#include "AST/ASTDecls.h"
#include "AST/Operators.h"
#include "CodeGen/IRCleanup.h"
#include "CodeGen/IRCleanupStack.h"
#include "CodeGen/IRFunctionAttributes.h"
#include "CodeGen/IRFunctionType.h"
#include "CodeGen/IRJumpDestination.h"
#include "CodeGen/IRTypes.h"
#include "CodeGen/IRTypeBuilder.h"
#include "CodeGen/SymbolTable.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/Support/NoFolder.h>

namespace MoEnjin
{	

	// ----------------------------------------------------------------------------------
	// FIXME: constify the appropriate IREmitter Get* functions and any other functions
	//        that makes sense to do so (i.e. they don't make any modifications!)
	// ----------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------


	class IREmitter
	{
		friend class IRCodeGen;
		friend class IRExceptionHandlingHazards;
		friend class IRStubEmitter;
		friend class IRFunctionScope;
		friend class StubCall;

		typedef std::vector<IRBasicBlock> IRBasicBlockCollection;
		typedef std::vector<IRFunctionScope*> IRFunctionScopeCollection;

		public:
			IREmitter(IRCodeGen* generator);

			llvm::Module* GetModule();
			llvm::DataLayout* GetDataLayout();
			llvm::LLVMContext& GetContext();
			const IRTypes& GetTypes() const { return mTypes; }
			UTF8String GetModuleName() const;

			IRValue EmitExpression(Expression* node);
			void EmitStatements(StatementList* statements);
			void EmitStatement(Statement* statement);

			void EmitDefaultSetup();

			void EmitProgram(ProgramNode* program);
			void EmitProgramPrologue(ProgramNode* program);
			void EmitProgramEpilogue(ProgramNode* program);

			IRFunction EmitProgramFunction(ProgramNode* program);

			void EmitFunctionPrologue(ScopedStatement* node);
			void EmitFunctionEpilogue();

			void EmitInitializeStringsFunction();
			void EmitStringTable();

			//------------------------------------------------------------------------------
			// EmitBranch - Emit a branch from the current insert block to the specified 
			//              basic block. If there is no insert block or the current block
			//              is already terminated then no branch will be created. This is
			//              useful if the current block is a dummy placeholder or if it's
			//              not known whether the current block has been terminated.
			//
			// Remarks:
			//   • This function is safe to call even if there is no insert point.
			//   • This function clears the current insert point.
			//------------------------------------------------------------------------------
			IRInstruction EmitBranch(const IRBasicBlock& toBlock);


			//------------------------------------------------------------------------------
			// EmitBranch - Emits a conditional branch from the current insert block to the
			//              specified blocks.
			//------------------------------------------------------------------------------
			IRInstruction EmitBranch(const IRValue& condition, const IRBasicBlock& trueBlock, const IRBasicBlock& falseBlock);


			//------------------------------------------------------------------------------
			// EmitBranchTo - Emits a branch from the current insert block to the specified
			//                basic block. Unlike EmitBranch(toBlock), this function simply
			//                just creates a fallthrough branch will no validations.
			//
			// Remarks:
			//   • There must be a current insert point.
			//   • The current branch must not already be terminated.
			//------------------------------------------------------------------------------
			IRInstruction EmitBranchTo(const IRBasicBlock& toBlock);


			void EmitBranchThroughCleanup(const IRJumpDestination& jumpDest);

			IRGlobalVariable EmitGlobalVariable(const UTF8String& name, llvm::Type* type);
			IRGlobalVariable EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant);
			IRGlobalVariable EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant, llvm::Constant* initializer);
			IRGlobalVariable EmitGlobalVariable(const UTF8String& name, llvm::Type* type, mo_bool isConstant, llvm::GlobalValue::LinkageTypes linkage, llvm::Constant* initializer);

			IRValue EmitBitCast(IRValue value, IRType type, const UTF8String& name = "");
			IRValue EmitBitCast(IRValue value, llvm::Type* type, const UTF8String& name = "");

			IRValue EmitPtrCast(IRValue value, IRType type, const UTF8String& name = "");
			IRValue EmitPtrCast(IRValue value, llvm::Type* type, const UTF8String& name = "");

			IRValue EmitIntCast(IRValue value, IRType type, mo_bool isSigned = mo_true, const UTF8String& name = "");
			IRValue EmitIntCast(IRValue value, llvm::Type* type, mo_bool isSigned = mo_true, const UTF8String& name = "");

			IRValue EmitIntToPtr(IRValue value, IRType type, const UTF8String& name = "");
			IRValue EmitIntToPtr(IRValue value, llvm::Type* type, const UTF8String& name = "");

			llvm::Value* EmitStubCall(const UTF8String& name, llvm::ArrayRef<llvm::Value*> args, const UTF8String& returnName = mo_null);

			llvm::Value* EmitCall(const UTF8String& name, llvm::Value* arg, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall(const UTF8String& name, const UTF8String& returnName = mo_null) { return EmitCall(GetFunction(name), returnName); }
			llvm::Value* EmitCall(const UTF8String& name, llvm::ArrayRef<llvm::Value*> args, const UTF8String& returnName = mo_null) { return EmitCall(GetFunction(name), args, returnName); }
			
			llvm::Value* EmitCall1(const UTF8String& name, llvm::Value* arg1, const UTF8String& returnName = mo_null) { return EmitCall1(GetFunction(name), arg1, returnName); }
			llvm::Value* EmitCall2(const UTF8String& name, llvm::Value* arg1, llvm::Value* arg2, const UTF8String& returnName = mo_null) { return EmitCall2(GetFunction(name), arg1, arg2, returnName); }
			llvm::Value* EmitCall3(const UTF8String& name, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, const UTF8String& returnName = mo_null) { return EmitCall3(GetFunction(name), arg1, arg2, arg3, returnName); }
			llvm::Value* EmitCall4(const UTF8String& name, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, const UTF8String& returnName = mo_null) { return EmitCall4(GetFunction(name), arg1, arg2, arg3, arg4, returnName); }
			llvm::Value* EmitCall5(const UTF8String& name, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, llvm::Value* arg5, const UTF8String& returnName = mo_null) { return EmitCall5(GetFunction(name), arg1, arg2, arg3, arg4, arg5, returnName); }

			llvm::Value* EmitCall(llvm::Value* func, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall(llvm::Value* func, llvm::ArrayRef<llvm::Value*> args, const UTF8String& returnName = mo_null);
			
			llvm::Value* EmitCall1(llvm::Value* func, llvm::Value* arg1, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall2(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall3(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall4(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, const UTF8String& returnName = mo_null);
			llvm::Value* EmitCall5(llvm::Value* func, llvm::Value* arg1, llvm::Value* arg2, llvm::Value* arg3, llvm::Value* arg4, llvm::Value* arg5, const UTF8String& returnName = mo_null);

			IRCallInst EmitCallWithAttrs(const UTF8String& name, mo_int32 attrs, const UTF8String& returnName = mo_null) { return EmitCallWithAttrs(GetFunction(name), attrs, returnName); }
			IRCallInst EmitCallWithAttrs(const UTF8String& name, llvm::ArrayRef<llvm::Value*> args, mo_int32 attrs, const UTF8String& returnName = mo_null) { return EmitCallWithAttrs(GetFunction(name), args, attrs, returnName); }

			IRCallInst EmitCallWithAttrs(llvm::Value* func, mo_int32 attrs, const UTF8String& returnName = mo_null);
			IRCallInst EmitCallWithAttrs(llvm::Value* func, llvm::ArrayRef<llvm::Value*> args, mo_int32 attrs, const UTF8String& returnName = mo_null);

			IRValue EmitStringArray(const UTF8StringCollection& strings, const UTF8String& name = "");

			llvm::Value* EmitArray(IRType type, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name = "");
			llvm::Value* EmitArray(llvm::Type* type, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name = "");
			llvm::Value* EmitArray(llvm::Value* arr, llvm::ArrayRef<llvm::Value*> values, const UTF8String& name = "");


			//-------------------------------------------------------------------------------------------------
			// Allocation
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			IRValue EmitAlloc(IRType type, const UTF8String& name = "");
			IRValue EmitAlloc(llvm::Type* type, const UTF8String& name = "");

			IRValue EmitAlloc(IRType type, const mo_int32 arraySize, const UTF8String& name = "");
			IRValue EmitAlloc(llvm::Type* type, mo_int32 arraySize, const UTF8String& name = "");

			IRValue EmitAllocStruct(llvm::StructType* type, const UTF8String& name = "");

			IRValue EmitAllocWithValue(IRType type, const IRValue& value, const UTF8String& name = "");
			IRValue EmitAllocWithValue(llvm::Type* type, const IRValue& value, const UTF8String& name = "");

			
			//-------------------------------------------------------------------------------------------------
			// Store
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			IRValue EmitStore(const IRValue& ptr, const IRValue& value, mo_bool isVolatile = mo_false, const IRInstruction& insertBeforeInst = IRInstruction::Null);
			IRValue EmitStoreNull(const IRValue& ptr, IRType type, mo_bool isVolatile = mo_false);
			IRValue EmitArrayStore(const IRValue& arrayPtr, const IRValue& value, mo_int32 index, mo_bool isVolatile = mo_false, const UTF8String& elementName = "");


			llvm::LoadInst* EmitLoad(const IRValue& ptr, const UTF8String& name = "");
			llvm::LoadInst* EmitVolatileLoad(const IRValue& ptr, const UTF8String& name = "");

			llvm::Value* EmitGEP(llvm::Value* ptr, mo_int32 index = 0, mo_int32 count = 1, const UTF8String& name = "");
			llvm::Value* EmitGEP2(llvm::Value* ptr, mo_int32 index1, mo_int32 index2 = 0, mo_int32 count = 1, const UTF8String& name = "");
			llvm::Value* EmitGEP3(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 index3 = 0, mo_int32 count = 1, const UTF8String& name = "");

			llvm::Value* EmitStructGEP(llvm::Value* ptr, mo_int32 index = 0, mo_int32 count = 1, const UTF8String& name = "");
			llvm::Value* EmitStructGEP2(llvm::Value* ptr, mo_int32 index1, mo_int32 index2 = 0, mo_int32 count = 1, const UTF8String& name = "");
			llvm::Value* EmitStructGEP3(llvm::Value* ptr, mo_int32 index1, mo_int32 index2, mo_int32 index3 = 0, mo_int32 count = 1, const UTF8String& name = "");

			void EmitMemSet(const IRValue& ptr, const IRValue& value, mo_uint64 size, mo_uint32 align = 0, mo_bool isVolatile = mo_false);
			void EmitMemSet(const IRValue& ptr, const IRValue& value, const IRValue& size, mo_uint32 align = 0, mo_bool isVolatile = mo_false);

			IRValue EmitDecodedTagValue(IRValue encodedValue, const UTF8String& name = "");
			IRValue EmitDecodedPayloadValue(IRValue encodedValue, const UTF8String& name = "");

			IRValue EmitEncodedValue(IRValue payload, IRValue tag, const UTF8String& name = "");
			IRValue EmitEncodedStringValue(const UTF8String& str, const UTF8String& name = mo_null);

			llvm::Value* LoadEmptyValue(const UTF8String& name = "");
			llvm::Value* LoadUndefinedValue(const UTF8String& name = "");
			llvm::Value* LoadNullValue(const UTF8String& name = "");
			llvm::Value* LoadTrueValue(const UTF8String& name = "");
			llvm::Value* LoadFalseValue(const UTF8String& name = "");

			IRValue ValueWithName(const IRValue& value, const UTF8String& name);

			const llvm::StructLayout* GetStructLayout(llvm::StructType* type);
			mo_uint64 GetStructSize(llvm::StructType* type);

			IRConstant GetString(const UTF8String& str);
			IRValue GetEncodedString(const UTF8String& str);

			llvm::GlobalVariable* FindGlobal(const UTF8String& name);
			llvm::Value* FindSymbol(const UTF8String& name);

			IRBasicBlock GetUnreachableBlock();
			IRInstruction EmitUnreachable();

			//-------------------------------------------------------------------------------------------------
			// Exception Handling / Cleanup
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			IRCleanupRef GetCleanupStackDepth() const;
			IRCleanupRef GetInnermostCleanup() const;

			void PopCleanupBlocks(IRCleanupRef& endDepth);
			void PopCleanupBlock(mo_bool fallthroughIsBranchThrough = mo_false);

			IRJumpDestination GetJumpDestination(const IRBasicBlock& targetBlock);
			IRJumpDestination GetJumpDestination(const UTF8String& name);

			void EmitCleanup(IRCleanupTask* task);

			IRBasicBlock EnsureCleanupHasBasicBlock(IRCleanup& cleanup);
			IRBasicBlock SimplifyCleanupEntry(const IRBasicBlock& entryBlock);

			mo_bool CanBranchWithoutCleanups(const IRJumpDestination& dest);

			void EmitCallToExceptionThrow(const IRValue& exceptionValue);
			IRValue EmitCallToExceptionExtract(const IRValue& exceptionData);
			void EmitCallToExceptionTryEnter(const IRValue& exceptionData);
			void EmitCallToExceptionTryExit(const IRValue& exceptionData);
			void EmitCallToExceptionCatchEnter();
			void EmitCallToExceptionCatchExit();
			void EmitCallToExceptionSetGlobalHandler(const IRValue& exceptionData);
			void EmitCallToExceptionReport(const IRValue& exceptionData);
			IRValue EmitCallToExceptionErrorCode(const IRValue& exceptionData);
			void EmitCallToInitialize();
			IRValue EmitCallToInitializeEnvironment();
			void EmitCallToShutdown();

			//-------------------------------------------------------------------------------------------------
			// Functions
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			IRFunction EmitFunction(const UTF8String& name, llvm::FunctionType* type, llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::InternalLinkage);
			void EmitReturnBlock(IRJumpDestination& returnDest);
			
			IRFunction BeginFunction(IRFunctionType type, const UTF8String& name, llvm::FunctionType* signature, llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::InternalLinkage);
			IRFunction BeginFunction(IRFunctionType type, const IRFunction& function);
			IRFunction EndFunction();

			IRFunctionScope* GetFunctionScope();

			IRFunction GetFunction();
			IRFunction GetFunction(const UTF8String& name);
			IRConstant GetOrInsertFunction(const UTF8String& name, llvm::Type* type = mo_null, llvm::CallingConv::ID callConv = llvm::CallingConv::C);

			IRValue GetNormalCleanupDestSlot();

			mo_bool IsJSFunction(const IRFunction& function);
			void MoveFunctionToFront(const IRFunction& function);

			//-------------------------------------------------------------------------------------------------
			// Basic Blocks
			//-------------------------------------------------------------------------------------------------
			//-------------------------------------------------------------------------------------------------
			IRBasicBlock NewEntryBasicBlock(const IRFunction parent = mo_null);
			IRBasicBlock NewBasicBlock(const UTF8String& name = "", const IRFunction parent = mo_null, const IRBasicBlock& beforeBlock = mo_null);

			IRBasicBlock EmitBasicBlock(IRBasicBlock& block, mo_bool isFinished = mo_false);
			IRBasicBlock EmitBasicBlockBefore(const IRBasicBlock& insertBeforeBlock, const UTF8String& name);
			
			void EmitBasicBlockIfUsed(IRBasicBlock& block);

			IRBasicBlock EnsureBasicBlock(const UTF8String& name = "");

			IRBasicBlock BeginBasicBlock(const IRBasicBlock& block);
			IRBasicBlock EndBasicBlock();

			IRBasicBlock GetInsertBasicBlock();

			mo_bool InBasicBlock();
			mo_bool HasPredecessors(const IRBasicBlock& block) const;
			mo_bool ReduceForwardingBlocks(const IRBasicBlock& block);

		private: // AST Emitters

			IRValue EmitOp(Operator op, const IRValue& lhs, const IRValue& rhs);
			IRValue EmitExpressionAsBoolean(Expression* expr);
			void EmitTryEnter(const IRValue& exceptionDataValue, const IRValue& gotoTryExitVar, IRBasicBlock& normalBlock, IRBasicBlock& exceptionBlock, mo_bool inCatchBlock);
			void EmitCaseBlockNode(CaseBlockNode* node);
			void EmitClauseListNode(ClauseListNode* node, mo_int32& caseId);
			void EmitDefaultCaseClauseNode(CaseClauseNode* node);
			IRValue EmitCaseBlockNodeCondition(const IRValue& switchValue, CaseBlockNode* node);
			IRValue EmitCaseClauseNodeCondition(const IRValue& switchValue, CaseClauseNode* node);
			IRValue EmitLeftForInExpression(Expression* expr, const IRValue& propIterator);
			IRFunction EmitFunctionSignature(FunctionNode* funcNode);
			void EmitFunctionBody(FunctionNode* funcNode, const IRFunction& nativeFunction);
			IRValue EmitArgumentsNode(ArgumentsNode* node, mo_uint32* outCount, const UTF8String& name = "");
			IRValue EmitJSCall(llvm::Function* func, ArgumentsNode* arguments);
			IRValue EmitHostCallByName(const UTF8String& functionName, const IRValue& baseValue, ArgumentsNode* arguments);
			IRValue EmitHostCallByName(const IRValue& functionName, const IRValue& baseValue, ArgumentsNode* arguments);
			IRValue EmitHostCall(const IRValue& function, const IRValue& baseValue, ArgumentsNode* arguments);

			#define DECLARE_EMIT_EXPRESSION_FUNCTION(type) IRValue Emit##type(type* node)
			#define DECLARE_EMIT_STATEMENT_FUNCTION(type) void Emit##type(type* statement)

			DECLARE_EMIT_STATEMENT_FUNCTION(FunctionDeclStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ReturnStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(BreakStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ContinueStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ThrowStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(DebuggerStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(VarStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ConstStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ExpressionStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(EmptyStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(BlockStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(IfStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(WhileStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(DoWhileStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ForStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(ForInStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(SwitchStatement);
			DECLARE_EMIT_STATEMENT_FUNCTION(TryStatement);

			DECLARE_EMIT_EXPRESSION_FUNCTION(ResolvableExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ResolveFunctionCallExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DotFunctionCallExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(BracketFunctionCallExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ValueFunctionCallExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(AssignResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(AssignDotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(AssignBracketExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ReadModifyResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ReadModifyDotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ReadModifyBracketExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DotAccessorExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(BracketAccessorExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DeleteResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DeleteDotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DeleteBracketExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(DeleteValueExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(TypeOfResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(TypeOfValueExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(NewExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ThisExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(NumberExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(BooleanExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(StringExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(NullExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(VoidExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ArrayExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ConditionalExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(CommaExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(ConstDeclExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(BitwiseNotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(LogicalNotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(LogicalExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(BinaryExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(NegateExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(UnaryPlusExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PrefixBracketExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PrefixDotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PrefixResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PrefixErrorExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PostfixBracketExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PostfixDotExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PostfixResolveExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(PostfixErrorExpression);
			DECLARE_EMIT_EXPRESSION_FUNCTION(FunctionExpression);

			#undef DECLARE_EMIT_STATEMENT_FUNCTION
			#undef DECLARE_EMIT_EXPRESSION_FUNCTION

		private:
			IRCodeGen* mGenerator;
			IRStubEmitter* mStubEmitter;
			IRBuilderDef mBuilder;
			IRTypes mTypes;
			IRFunctionScopeCollection mFunctionScopeStack;
			IRCleanupStack mCleanupStack;
			IRValue mNativeContext;
			IRConstant mStringTableValue;
			SymbolTable mStringTable;
			SymbolTable mStringTable_OLD;
			SymbolTable mEncodedStringTable;
			IID mStringTableID;
			IID mEncodedStringTableID;
			IID mCleanupDestID;
	};
}

#endif