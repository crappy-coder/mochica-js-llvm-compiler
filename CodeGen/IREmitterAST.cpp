#include "CodeGen/IREmitter.h"
#include "CodeGen/IRAutoCleanupScope.h"
#include "CodeGen/IRExceptionHandlingHazards.h"
#include "CodeGen/IRCodeGen.h"
#include "CodeGen/IRConsts.h"
#include "CodeGen/IRFunctionScope.h"
#include "CodeGen/IRPerformCatchCleanup.h"
#include "CodeGen/IRPerformFinallyCleanup.h"
#include "CodeGen/IRStubEmitter.h"
#include "CodeGen/IRType.h"
#include "CodeGen/LLVMUtils.h"
#include "CodeGen/StubCall.h"
#include "AST/AST.h"
#include "AST/ASTContext.h"
#include "AST/OpCodes.h"
#include "Platform/LLVM.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	#define DEFINE_EMIT_EXPRESSION_FUNCTION(type) IRValue IREmitter::Emit##type(type* node)
	#define DEFINE_EMIT_STATEMENT_FUNCTION(type) void IREmitter::Emit##type(type* statement)

	void IREmitter::EmitStatements(StatementList* statements)
	{
		if(!statements || !statements->HasStatements())
			return;

		for(mo_int32 i = 0, len = statements->GetLength(); i < len; ++i)
			EmitStatement(statements->GetStatement(i));
	}

	void IREmitter::EmitStatement(Statement* statement)
	{
		if(!statement)
			return;

		switch(statement->GetType())
		{
			#define EMIT_STATEMENT(ntype, type) case ntype: mo_debug_print("Emitting: %s\n", #type); Emit##type(mo_cast<type*>(statement)); break

			EMIT_STATEMENT(NODE_BLOCK, BlockStatement);
			EMIT_STATEMENT(NODE_FUNCTION_DECL, FunctionDeclStatement);
			EMIT_STATEMENT(NODE_RETURN, ReturnStatement);
			EMIT_STATEMENT(NODE_BREAK, BreakStatement);
			EMIT_STATEMENT(NODE_CONTINUE, ContinueStatement);
			EMIT_STATEMENT(NODE_THROW, ThrowStatement);
			EMIT_STATEMENT(NODE_DEBUGGER, DebuggerStatement);
			EMIT_STATEMENT(NODE_VAR, VarStatement);
			EMIT_STATEMENT(NODE_CONST_STATEMENT, ConstStatement);
			EMIT_STATEMENT(NODE_EXPR_STATEMENT, ExpressionStatement);
			EMIT_STATEMENT(NODE_EMPTY, EmptyStatement);
			EMIT_STATEMENT(NODE_IF, IfStatement);
			EMIT_STATEMENT(NODE_IF_ELSE, IfStatement); // if and if..else are handled in the same emit function
			EMIT_STATEMENT(NODE_WHILE, WhileStatement);
			EMIT_STATEMENT(NODE_DO_WHILE, DoWhileStatement);
			EMIT_STATEMENT(NODE_FOR, ForStatement);
			EMIT_STATEMENT(NODE_FOR_IN, ForInStatement);
			EMIT_STATEMENT(NODE_SWITCH, SwitchStatement);
			EMIT_STATEMENT(NODE_TRY, TryStatement);

			#undef EMIT_STATEMENT
		}
	}

	IRValue IREmitter::EmitExpression(Expression* expression)
	{
		if(expression)
		{
			switch(expression->GetType())
			{
				#define EMIT_EXPRESSION(ntype, type) case ntype: mo_debug_print("Emitting: %s\n", #type); return Emit##type(mo_cast<type*>(expression))

				EMIT_EXPRESSION(NODE_ASSIGN_RESOLVE, AssignResolveExpression);
				EMIT_EXPRESSION(NODE_ASSIGN_DOT, AssignDotExpression);
				EMIT_EXPRESSION(NODE_ASSIGN_BRACKET, AssignBracketExpression);
				EMIT_EXPRESSION(NODE_READ_MODIFY_RESOLVE, ReadModifyResolveExpression);
				EMIT_EXPRESSION(NODE_READ_MODIFY_DOT, ReadModifyDotExpression);
				EMIT_EXPRESSION(NODE_READ_MODIFY_BRACKET, ReadModifyBracketExpression);
				EMIT_EXPRESSION(NODE_DOT_ACCESSOR, DotAccessorExpression);
				EMIT_EXPRESSION(NODE_BRACKET_ACCESSOR, BracketAccessorExpression);
				EMIT_EXPRESSION(NODE_DELETE_RESOLVE, DeleteResolveExpression);
				EMIT_EXPRESSION(NODE_DELETE_DOT, DeleteDotExpression);
				EMIT_EXPRESSION(NODE_DELETE_BRACKET, DeleteBracketExpression);
				EMIT_EXPRESSION(NODE_DELETE_VALUE, DeleteValueExpression);
				EMIT_EXPRESSION(NODE_TYPEOF_RESOLVE, TypeOfResolveExpression);
				EMIT_EXPRESSION(NODE_TYPEOF_VALUE, TypeOfValueExpression);
				EMIT_EXPRESSION(NODE_BITWISE_NOT, BitwiseNotExpression);
				EMIT_EXPRESSION(NODE_LOGICAL_NOT, LogicalNotExpression);
				EMIT_EXPRESSION(NODE_LOGICAL_OP, LogicalExpression);
				EMIT_EXPRESSION(NODE_NEGATE, NegateExpression);
				EMIT_EXPRESSION(NODE_UNARY_PLUS, UnaryPlusExpression);
				EMIT_EXPRESSION(NODE_PREFIX_BRACKET, PrefixBracketExpression);
				EMIT_EXPRESSION(NODE_PREFIX_DOT, PrefixDotExpression);
				EMIT_EXPRESSION(NODE_PREFIX_RESOLVE, PrefixResolveExpression);
				EMIT_EXPRESSION(NODE_PREFIX_ERROR, PrefixErrorExpression);
				EMIT_EXPRESSION(NODE_POSTFIX_BRACKET, PostfixBracketExpression);
				EMIT_EXPRESSION(NODE_POSTFIX_DOT, PostfixDotExpression);
				EMIT_EXPRESSION(NODE_POSTFIX_RESOLVE, PostfixResolveExpression);
				EMIT_EXPRESSION(NODE_POSTFIX_ERROR, PostfixErrorExpression);
				EMIT_EXPRESSION(NODE_NEW, NewExpression);
				EMIT_EXPRESSION(NODE_THIS, ThisExpression);
				EMIT_EXPRESSION(NODE_FUNC_CALL_RESOLVE, ResolveFunctionCallExpression);
				EMIT_EXPRESSION(NODE_FUNC_CALL_VALUE, ValueFunctionCallExpression);
				EMIT_EXPRESSION(NODE_FUNC_CALL_DOT, DotFunctionCallExpression);
				EMIT_EXPRESSION(NODE_FUNC_CALL_BRACKET, BracketFunctionCallExpression);
				EMIT_EXPRESSION(NODE_NUMBER, NumberExpression);
				EMIT_EXPRESSION(NODE_BOOLEAN, BooleanExpression);
				EMIT_EXPRESSION(NODE_STRING, StringExpression);
				EMIT_EXPRESSION(NODE_NULL, NullExpression);
				EMIT_EXPRESSION(NODE_VOID, VoidExpression);
				EMIT_EXPRESSION(NODE_ARRAY, ArrayExpression);
				EMIT_EXPRESSION(NODE_CONDITIONAL, ConditionalExpression);
				EMIT_EXPRESSION(NODE_COMMA, CommaExpression);
				EMIT_EXPRESSION(NODE_CONST_DECL, ConstDeclExpression);
				EMIT_EXPRESSION(NODE_RESOLVE, ResolvableExpression);
				EMIT_EXPRESSION(NODE_ADD, BinaryExpression);
				EMIT_EXPRESSION(NODE_SUBTRACT, BinaryExpression);
				EMIT_EXPRESSION(NODE_MULTIPLY, BinaryExpression);
				EMIT_EXPRESSION(NODE_DIVIDE, BinaryExpression);
				EMIT_EXPRESSION(NODE_MODULUS, BinaryExpression);
				EMIT_EXPRESSION(NODE_LEFT_SHIFT, BinaryExpression);
				EMIT_EXPRESSION(NODE_RIGHT_SHIFT, BinaryExpression);
				EMIT_EXPRESSION(NODE_RIGHT_SHIFT_UNSIGNED, BinaryExpression);
				EMIT_EXPRESSION(NODE_LESS, BinaryExpression);
				EMIT_EXPRESSION(NODE_LESS_EQ, BinaryExpression);
				EMIT_EXPRESSION(NODE_GREATER, BinaryExpression);
				EMIT_EXPRESSION(NODE_GREATER_EQ, BinaryExpression);
				EMIT_EXPRESSION(NODE_INSTANCEOF, BinaryExpression);
				EMIT_EXPRESSION(NODE_IN, BinaryExpression);
				EMIT_EXPRESSION(NODE_EQUAL, BinaryExpression);
				EMIT_EXPRESSION(NODE_EQUAL_STRICT, BinaryExpression);
				EMIT_EXPRESSION(NODE_NOT_EQUAL, BinaryExpression);
				EMIT_EXPRESSION(NODE_NOT_EQUAL_STRICT, BinaryExpression);
				EMIT_EXPRESSION(NODE_BIT_AND, BinaryExpression);
				EMIT_EXPRESSION(NODE_BIT_OR, BinaryExpression);
				EMIT_EXPRESSION(NODE_BIT_XOR, BinaryExpression);
				EMIT_EXPRESSION(NODE_FUNCTION_EXPR, FunctionExpression);

				#undef EMIT_EXPRESSION
			}

			mo_debug_print("EXPRESSION NOT SUPPORTED: %d\n", expression->GetType());
		}

		return LoadUndefinedValue("UNKNOWN_TYPE");
	}




	//-------------------------------------------------------------------------------------------------
	// SECTION: Expressions
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------


	DEFINE_EMIT_EXPRESSION_FUNCTION(ResolvableExpression)
	{
		return mStubEmitter->EmitResolve(node->GetID());
	}

	//-------------------------------------------------------------------------------------------------
	// FIXME: Currently, all function calls go through the runtime (stub_call_func), which set's up the
	//        environment for the function call and then executes the native function, which goes back
	//        into the runtime to finish setting up it's local environment.
	//
	//        We can certainly optimize this by looking up the native function directly here and 
	//        emit a direct call to it. This will need to setup it up though like ExecuteCall does in
	//        JSStackFrame.
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	DEFINE_EMIT_EXPRESSION_FUNCTION(ResolveFunctionCallExpression)
	{
		return EmitHostCallByName(node->GetId(), IRValue::Null, node->GetArguments());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DotFunctionCallExpression)
	{
		IRValue base = EmitExpression(node->GetBase());

		return EmitHostCallByName(node->GetId(), base, node->GetArguments());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(BracketFunctionCallExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue functionName = EmitExpression(node->GetSubscript());

		return EmitHostCallByName(functionName, base, node->GetArguments());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ValueFunctionCallExpression)
	{
		IRValue functionValue = EmitExpression(node->GetExpression());

		return EmitHostCall(functionValue, IRValue::Null, node->GetArguments());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(AssignResolveExpression)
	{
		const UTF8String& id = node->GetId();
		IRValue value = EmitExpression(node->GetRight());

		return mStubEmitter->EmitPutVarByName(id, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(AssignDotExpression)
	{
		const UTF8String& id = node->GetId();
		IRValue base = EmitExpression(node->GetBase());
		IRValue value = EmitExpression(node->GetRight());

		return mStubEmitter->EmitPutVar(base, id, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(AssignBracketExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());
		IRValue value = EmitExpression(node->GetRight());

		return mStubEmitter->EmitPutVar(base, property, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ReadModifyResolveExpression)
	{
		const UTF8String& id = node->GetId();
		IRValue leftValue = mStubEmitter->EmitResolve(id);
		IRValue rightValue = EmitExpression(node->GetRight());
		IRValue value = EmitOp(node->GetOp(), leftValue, rightValue);

		return mStubEmitter->EmitPutVarByName(id, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ReadModifyDotExpression)
	{
		const UTF8String& id = node->GetId();
		IRValue base = EmitExpression(node->GetBase());
		IRValue leftValue = mStubEmitter->EmitGetVar(base, id);
		IRValue rightValue = EmitExpression(node->GetRight());
		IRValue value = EmitOp(node->GetOp(), leftValue, rightValue);

		return mStubEmitter->EmitPutVar(base, id, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ReadModifyBracketExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());
		IRValue leftValue = mStubEmitter->EmitGetVar(base, property);
		IRValue rightValue = EmitExpression(node->GetRight());
		IRValue value = EmitOp(node->GetOp(), leftValue, rightValue);

		return mStubEmitter->EmitPutVar(base, property, value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DotAccessorExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		
		return mStubEmitter->EmitGetVar(base, node->GetID());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(BracketAccessorExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());

		return mStubEmitter->EmitGetVar(base, property);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DeleteResolveExpression)
	{
		return mStubEmitter->EmitDeleteVarInThis(node->GetID());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DeleteDotExpression)
	{
		IRValue base = EmitExpression(node->GetBase());

		return mStubEmitter->EmitDeleteVar(base, node->GetID());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DeleteBracketExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());

		return mStubEmitter->EmitDeleteVar(base, property);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(DeleteValueExpression)
	{
		return LoadTrueValue();
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(TypeOfResolveExpression)
	{
		IRValue value = mStubEmitter->EmitResolve(node->GetId());

		return mStubEmitter->EmitTypeOf(value);	
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(TypeOfValueExpression)
	{
		IRValue value = EmitExpression(node->GetExpression());

		return mStubEmitter->EmitTypeOf(value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(NewExpression)
	{
		mo_uint32 argumentCount = 0;
		IRValue arguments = EmitArgumentsNode(node->GetArguments(), &argumentCount, MO_IR_LOCAL_VAR_STRING(construct_args));
		IRValue func = EmitExpression(node->GetExpression());

		return mStubEmitter->EmitConstruct(func, arguments, argumentCount);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ThisExpression)
	{
		return mStubEmitter->EmitThis();
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(NumberExpression)
	{			
		return IRTypeBuilder::GetConstInt64(JSValue::Encode(JSValue::MakeNumber(node->GetValue())));
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(BooleanExpression)
	{
		return (node->GetValue() ? mGenerator->JSValueTrueConstant : mGenerator->JSValueFalseConstant);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(StringExpression)
	{
		return GetEncodedString(node->GetValue());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(NullExpression)
	{
		return LoadNullValue();
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(VoidExpression)
	{
		EmitExpression(node->GetExpression());

		return LoadUndefinedValue();
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ArrayExpression)
	{
		IRValue arrValue = IRTypeBuilder::GetConstNullPtr(IRBytePtrTy);
		std::vector<llvm::Value*> values;

		for(Element* el = node->GetElement(); el; el = el->GetNext())
		{
			IRValue elValue = EmitExpression(el->GetExpression());

			// add <elision> count undefs to array
			if(el->GetElision() != 0)
				values.insert(values.end(), el->GetElision(), mGenerator->JSValueUndefinedConstant);
			
			// add real element
			values.push_back(elValue.Get());
		}

		// add more <elision> count undefs to array
		if(node->GetElision() != 0)
			values.insert(values.end(), node->GetElision(), mGenerator->JSValueUndefinedConstant);

		// empty array
		if(!values.empty())
			arrValue = EmitPtrCast(EmitArray(IRJSValueTy, values, IR_VAR_NAME("array_initializer_values")), IRBytePtrTy, IR_VAR_NAME("array_initializer_ptr"));

		return mStubEmitter->EmitNewArray(arrValue, values.size());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ConditionalExpression)
	{
		Expression* exprCondition = node->GetCondition();
		Expression* exprTrue = node->GetLeft();
		Expression* exprFalse = node->GetRight();

		// condition is a known falsey node, just return the false expression
		mo_bool exprConditionBoolValue = mo_false;
		
		if(CanExpressionFoldToBoolean(exprCondition, exprConditionBoolValue))
			return EmitExpression(exprConditionBoolValue ? exprTrue : exprFalse);

		// emit the boolean condition comparison
		IRValue condition = EmitExpressionAsBoolean(exprCondition);
		IRValue conditionResult = mBuilder.CreateICmpEQ(condition, mGenerator->JSValueTrueConstant, IR_VAR_NAME("conditional_is_true"));

		IRValue valueTrue;
		IRValue valueFalse;

		// when both expressions are simple primitive types then we can just emit
		// them here and do a select since they do not have any sideeffects
		if(AST_IS_PRIMITIVE_OR_NULL_NODE(exprTrue) && AST_IS_PRIMITIVE_OR_NULL_NODE(exprFalse))
		{
			valueTrue = EmitExpression(exprTrue);
			valueFalse = EmitExpression(exprFalse);

			return mBuilder.CreateSelect(conditionResult, valueTrue, valueFalse, IR_VAR_NAME("conditional_value"));
		}


		// otherwise, we have to emit them in their own basic blocks to ensure that
		// the losing expression does not get evaluated, which can produce unwanted
		// side effects (i.e. assignment, calling a function, exceptions, etc...)
		IRBasicBlock trueBlock = NewBasicBlock(IR_BLOCK_NAME("ConditionalTrue"));
		IRBasicBlock falseBlock = NewBasicBlock(IR_BLOCK_NAME("ConditionalFalse"));
		IRBasicBlock exitBlock = NewBasicBlock(IR_BLOCK_NAME("ConditionalEnd"));

		// jump to winning block
		EmitBranch(conditionResult, trueBlock, falseBlock);

		// emit the true/left block
		EmitBasicBlock(trueBlock);
		valueTrue = EmitExpression(exprTrue);

		// make sure we sync the true block, it could have changed
		trueBlock = GetInsertBasicBlock();

		// jump to the conditional end
		EmitBranchTo(exitBlock);


		// emit the false/right block
		EmitBasicBlock(falseBlock);
		valueFalse = EmitExpression(exprFalse);

		// make sure we sync the false block, it could have changed
		falseBlock = GetInsertBasicBlock();

		// emit the exit block and generate our phi node
		EmitBasicBlock(exitBlock);

		llvm::PHINode* phi = mBuilder.CreatePHI(IRTypeBuilder::GetType(IRJSValueTy), 2, IR_VAR_NAME("conditional_value"));

		phi->addIncoming(valueTrue.Get(), trueBlock.Get());
		phi->addIncoming(valueFalse.Get(), falseBlock.Get());

		return phi;
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(CommaExpression)
	{
		mo_int32 count = node->GetCount();

		if(!count)
			return LoadUndefinedValue();

		if(count == 1)
			return EmitExpression(node->GetAt(0));

		for(mo_int32 i = 0; i < count-1; ++i)
			EmitExpression(node->GetAt(i));

		return EmitExpression(node->GetAt(count-1));
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(ConstDeclExpression)
	{
		IRValue resultValue;

		// emit each const expression, currently, we just treat these the same as a
		// var declaration, the last result is the result returned, even though
		// no predecessors actually use it, at the moment...
		for(ConstDeclExpression* constExpr = node; node; node = node->GetNext())
		{
			const UTF8String& id = node->GetId();
			IRValue value = (node->HasInitializer() ? EmitExpression(node->GetInitializer()) : LoadUndefinedValue());

			resultValue = mStubEmitter->EmitPutVarByName(id, value);
		}

		return resultValue;
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(BitwiseNotExpression)
	{
		IRValue value = EmitExpression(node->GetExpression());

		return mStubEmitter->EmitBitOp(value, IRTypeBuilder::GetConstInt64(JSValue::Encode(jsNumber(-1))), kBitOpXOR);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(LogicalNotExpression)
	{
		IRValue value = EmitExpression(node->GetExpression());
		
		return mStubEmitter->EmitNot(value);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(LogicalExpression)
	{
		IRValue xValue = EmitExpression(node->GetLeft());
		IRValue yValue = EmitExpression(node->GetRight());

		if(node->GetOp() == OpLogicalOr)
			return mStubEmitter->EmitLogicalOr(xValue, yValue);

		return mStubEmitter->EmitLogicalAnd(xValue, yValue);	
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(BinaryExpression)
	{
		IRValue xValue = EmitExpression(node->GetLeftExpression());
		IRValue yValue = EmitExpression(node->GetRightExpression());

		#define DEFINE_EMIT_BINOP_STUB(op, stub) \
			case op_##op: return mStubEmitter->Emit##stub(xValue, yValue)

		#define DEFINE_EMIT_BITOP_STUB(opL, opU) \
			case op_bit##opL: return mStubEmitter->EmitBitOp(xValue, yValue, kBitOp##opU)

		switch(node->GetOpCodeID())
		{
			DEFINE_EMIT_BINOP_STUB(add, Add);
			DEFINE_EMIT_BINOP_STUB(sub, Subtract);
			DEFINE_EMIT_BINOP_STUB(mul, Multiply);
			DEFINE_EMIT_BINOP_STUB(div, Divide);
			DEFINE_EMIT_BINOP_STUB(mod, Modulus);
			DEFINE_EMIT_BINOP_STUB(lshift, LeftShift);
			DEFINE_EMIT_BINOP_STUB(rshift, RightShift);
			DEFINE_EMIT_BINOP_STUB(urshift, UnsignedRightShift);
			DEFINE_EMIT_BINOP_STUB(less, IsLess);
			DEFINE_EMIT_BINOP_STUB(lesseq, IsLessOrEqual);
			DEFINE_EMIT_BINOP_STUB(greater, IsGreater);
			DEFINE_EMIT_BINOP_STUB(greatereq, IsGreaterOrEqual);
			DEFINE_EMIT_BINOP_STUB(instanceof, IsInstanceOf);
			DEFINE_EMIT_BINOP_STUB(in, IsIn);
			DEFINE_EMIT_BINOP_STUB(eq, Equal);
			DEFINE_EMIT_BINOP_STUB(stricteq, EqualStrict);
			DEFINE_EMIT_BINOP_STUB(neq, NotEqual);
			DEFINE_EMIT_BINOP_STUB(nstricteq, NotEqualStrict);

			DEFINE_EMIT_BITOP_STUB(and, AND);
			DEFINE_EMIT_BITOP_STUB(or, OR);
			DEFINE_EMIT_BITOP_STUB(xor, XOR);
		}

		#undef DEFINE_EMIT_BINOP_STUB
		#undef DEFINE_EMIT_BITOP_STUB

		LoadEmptyValue(IR_ERROR_NAME("UNKNOWN_BINARY_OP"));
		return LoadEmptyValue(UTF8String::FromNumber(mo_cast<mo_int32>(node->GetOpCodeID())));
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(NegateExpression)
	{
		return mStubEmitter->EmitNegate(EmitExpression(node->GetExpression()));
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(UnaryPlusExpression)
	{
		return mStubEmitter->EmitToNumber(EmitExpression(node->GetExpression()));
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PrefixBracketExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());

		return mStubEmitter->EmitPrefixOp(node->GetOperator(), base, property);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PrefixDotExpression)
	{
		IRValue base = EmitExpression(node->GetBase());

		return mStubEmitter->EmitPrefixOp(node->GetOperator(), base, node->GetId());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PrefixResolveExpression)
	{
		IRValue base = mStubEmitter->EmitResolveBase(node->GetId());

		return mStubEmitter->EmitPrefixOp(node->GetOperator(), base, node->GetId());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PrefixErrorExpression)
	{
		// TODO - figure out how this should be handled, use undefined as it is? return null, NaN, throw error or something else?
		return mGenerator->JSValueUndefinedConstant;
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PostfixBracketExpression)
	{
		IRValue base = EmitExpression(node->GetBase());
		IRValue property = EmitExpression(node->GetSubscript());

		return mStubEmitter->EmitPostfixOp(node->GetOperator(), base, property);
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PostfixDotExpression)
	{
		IRValue base = EmitExpression(node->GetBase());

		return mStubEmitter->EmitPostfixOp(node->GetOperator(), base, node->GetId());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PostfixResolveExpression)
	{
		IRValue base = mStubEmitter->EmitResolveBase(node->GetId());

		return mStubEmitter->EmitPostfixOp(node->GetOperator(), base, node->GetId());
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(PostfixErrorExpression)
	{
		// TODO - figure out how this should be handled, use undefined as it is? return null, NaN, throw error or something else?
		return mGenerator->JSValueUndefinedConstant;
	}

	DEFINE_EMIT_EXPRESSION_FUNCTION(FunctionExpression)
	{
		FunctionNode* funcNode = node->GetFunction();
		IRFunction nativeFunction = GetFunction(MO_IR_JS_FUNCTION_VAR_NAME(funcNode->GetQualifiedName()));

		// emit the functions body
		EmitFunctionBody(funcNode, nativeFunction);

		// create a new function object
		return mStubEmitter->EmitNewFunc(funcNode->GetName(), funcNode->GetParameterCount(), funcNode->IsStrictMode(), nativeFunction);
	}



	//-------------------------------------------------------------------------------------------------
	// SECTION: Statements
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	DEFINE_EMIT_STATEMENT_FUNCTION(FunctionDeclStatement)
	{
		FunctionNode* funcNode = statement->GetFunction();
		IRFunction nativeFunction = GetFunction(MO_IR_JS_FUNCTION_VAR_NAME(funcNode->GetQualifiedName()));

		// emit the functions body
		EmitFunctionBody(funcNode, nativeFunction);
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ReturnStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRValue returnValue = IRValue::Null;
		IRValue returnValueVariable = scope->GetReturnValue();

		// js functions emitted through ir should always have a return
		// type and thus a return variable for it
		MO_ASSERT(!returnValueVariable.IsNull());

		// ECMA 5.1, §12.9
		//
		// returns without a value must always return undefined
		returnValue = statement->GetValue() ? EmitExpression(statement->GetValue()) : mGenerator->JSValueUndefinedConstant;

		// store the value in our variable
		EmitStore(returnValueVariable, returnValue);

		// keep track of the total number of returns and
		// total number of returns that have a value, we'll
		// use this later for stats/debugging
		scope->SetReturnCount(scope->GetReturnCount() + 1);

		if(statement->GetValue())
			scope->SetReturnsWithValueCount(scope->GetReturnsWithValueCount() + 1);

		EmitBranchThroughCleanup(scope->GetReturnDestination());
	}

	
	//-------------------------------------------------------------------------------------------------
	// FIXME: Need to implement labelled break/continue statements
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	DEFINE_EMIT_STATEMENT_FUNCTION(BreakStatement)
	{	
		if(!InBasicBlock())
			return;

		EmitBranchThroughCleanup(GetFunctionScope()->GetBreakDestination());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ContinueStatement)
	{
		if(!InBasicBlock())
			return;

		EmitBranchThroughCleanup(GetFunctionScope()->GetContinueDestination());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ThrowStatement)
	{
		IRValue exceptionValue = EmitExpression(statement->GetExpression());

		EmitCallToExceptionThrow(exceptionValue);
		EmitUnreachable();

		// ensure the builder's insertion point is cleared to also
		// indicate to further emission code that we are now in 
		// unreachable code
		mBuilder.ClearInsertionPoint();
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(DebuggerStatement)
	{
		mStubEmitter->EmitDebugBreak(statement->GetStartLine(), statement->GetEndLine());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(VarStatement)
	{
		EmitExpression(statement->GetExpression());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ConstStatement)
	{
		EmitExpression(statement->GetExpression());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ExpressionStatement)
	{
		EmitExpression(statement->GetExpression());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(EmptyStatement)
	{
		LoadEmptyValue();
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(BlockStatement)
	{
		EmitStatements(statement->GetStatements());
	}


	//-------------------------------------------------------------------------------------------------
	// FIXME: Optimize the condition here by checking the type of expression, most of this is already
	//        done in the parser but if the expression is a BooleanExpression or NumberExpression we
	//        can certainly determine whether or not we need to emit one block or the other and branch.
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	DEFINE_EMIT_STATEMENT_FUNCTION(IfStatement)
	{
		// both 'if' and 'if..else' statements come through here
		IfElseStatement* ifElseStmt = (statement->GetType() == NODE_IF_ELSE ? mo_cast<IfElseStatement*>(statement) : mo_null);

		// generate the boolean condition and compare it to jsval(true)
		IRValue condition = EmitExpressionAsBoolean(statement->GetCondition());
		IRValue conditionResult = mBuilder.CreateICmpEQ(condition, mGenerator->JSValueTrueConstant, MO_IR_LOCAL_VAR_STRING(cond));

		// setup the basic blocks for just an 'if'
		IRBasicBlock thenBlock = NewBasicBlock(IR_BLOCK_NAME("IfThen"));
		IRBasicBlock continueBlock = NewBasicBlock(IR_BLOCK_NAME("IfEnd"));
		IRBasicBlock elseBlock = continueBlock;

		// create the else block for an if..else statement
		if(ifElseStmt)
			elseBlock = NewBasicBlock(IR_BLOCK_NAME("IfElse"));

		// emit a conditional branch
		EmitBranch(conditionResult, thenBlock, elseBlock);


		// emit the 'then' block
		EmitBasicBlock(thenBlock);
		EmitStatement(statement->GetBlock());

		// branch to the else block in an if..else or to the end block
		// when just an if..
		EmitBranch(continueBlock);


		// emit the 'else' block, if there is one
		if(ifElseStmt)
		{
			EmitBasicBlock(elseBlock);
			EmitStatement(ifElseStmt->GetElseBlock());

			// branch to the continuation (end) block
			EmitBranch(continueBlock);
		}

		// finish with the continuation (end) block
		EmitBasicBlock(continueBlock, mo_true);
	}

	//-------------------------------------------------------------------------------------------------
	// FIXME: Optimize for common loop scenarios in while(...) and do..while(...) loops
	//
	// • When we have a 'while(1)' or a 'while(true)' we can skip the condition branch
	// • When we have a 'do..while(0)', 'do..while(1)', 'do..while(false)' or a 'do..while(true)' we
	//   can skip the condition branch.
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	DEFINE_EMIT_STATEMENT_FUNCTION(WhileStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRJumpDestination conditionDest = GetJumpDestination(IR_BLOCK_NAME("WhileCondition"));
		IRJumpDestination exitDest = GetJumpDestination(IR_BLOCK_NAME("WhileEnd"));
		IRBasicBlock bodyBlock = NewBasicBlock(IR_BLOCK_NAME("WhileBody"));
		IRValue condition = 0;
		IRValue conditionResult = 0;


		// emit the condition block, which will also be the continue target
		EmitBasicBlock(conditionDest.GetBlock());

		// store the break/continue destinations
		scope->BeginBreakContinueScope(conditionDest, exitDest);

		// generate the boolean condition and compare it to jsval(true)
		condition = EmitExpressionAsBoolean(statement->GetExpression());
		conditionResult = mBuilder.CreateICmpEQ(condition, mGenerator->JSValueTrueConstant, MO_IR_LOCAL_VAR_STRING(cond));
		
		// jump to the loop body when the condition is true
		EmitBranch(conditionResult, bodyBlock, exitDest.GetBlock());


		// emit the loop body
		EmitBasicBlock(bodyBlock);
		EmitStatement(statement->GetStatement());

		// step out of the break/continue scope
		scope->EndBreakContinueScope();

		// emit a branch back to the start of the loop
		EmitBranch(conditionDest.GetBlock());


		// emit and finish with the exit block
		EmitBasicBlock(exitDest.GetBlock(), true);
	}

	//-------------------------------------------------------------------------------------------------
	// FIXME: Optimize do..while using the same optimizations as a while statement. See optimization
	//        notes in EmitWhileStatement.
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	DEFINE_EMIT_STATEMENT_FUNCTION(DoWhileStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRJumpDestination conditionDest = GetJumpDestination(IR_BLOCK_NAME("DoWhileCondition"));
		IRJumpDestination exitDest = GetJumpDestination(IR_BLOCK_NAME("DoWhileEnd"));
		IRBasicBlock bodyBlock = NewBasicBlock(IR_BLOCK_NAME("DoWhileBody"));
		IRValue condition = 0;
		IRValue conditionResult = 0;


		// store the break/continue destinations
		scope->BeginBreakContinueScope(conditionDest, exitDest);

		// emit the body of the loop
		EmitBasicBlock(bodyBlock);
		EmitStatement(statement->GetStatement());

		// step out of the break/continue scope
		scope->EndBreakContinueScope();


		// emit the condition block
		EmitBasicBlock(conditionDest.GetBlock());

		// generate a boolean condition and compare it to a jsval(true)
		condition = EmitExpressionAsBoolean(statement->GetExpression());
		conditionResult = mBuilder.CreateICmpEQ(condition, mGenerator->JSValueTrueConstant, MO_IR_LOCAL_VAR_STRING(cond));

		// jump to the loop body when the condition is true
		EmitBranch(conditionResult, bodyBlock, exitDest.GetBlock());

		
		// emit and finish with the exit block
		EmitBasicBlock(exitDest.GetBlock());
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ForStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRJumpDestination exitDest = GetJumpDestination(IR_BLOCK_NAME("ForEnd"));
		IRJumpDestination conditionDest = GetJumpDestination(IR_BLOCK_NAME("ForCondition"));
		IRJumpDestination incrementDest = conditionDest;


		// emit the initialization part before the loop
		if(statement->GetInitializeExpression())
			EmitExpression(statement->GetInitializeExpression());


		// begin the loop with a block that will test the condition
		EmitBasicBlock(conditionDest.GetBlock());

		// emit the condition evaluation and create a body block to branch
		// into, if we don't have a condition we don't need to create a new
		// block for the body, we can simply fall into it.
		if(statement->GetConditionExpression())
		{
			// create a block for the loops body
			IRBasicBlock bodyBlock = NewBasicBlock(IR_BLOCK_NAME("ForBody"));

			// generate a boolean condition and compare it to a jsval(true)
			IRValue condition = EmitExpressionAsBoolean(statement->GetConditionExpression());
			IRValue conditionResult = mBuilder.CreateICmpEQ(condition, mGenerator->JSValueTrueConstant, MO_IR_LOCAL_VAR_STRING(cond));

			// jump to the loop body when the condition is true
			EmitBranch(conditionResult, bodyBlock, exitDest.GetBlock());
			
			// emit the body block
			EmitBasicBlock(bodyBlock);
		}

		// create a new block for the increment expression, if one does
		// not exist, we can just use the condition block (see above 
		// for assignment) for continuation
		if(statement->GetIncrementExpression())
			incrementDest = GetJumpDestination(IR_BLOCK_NAME("ForIncrement"));

		// store the break/continue destinations
		scope->BeginBreakContinueScope(incrementDest, exitDest);

		// emit the loops body statement
		EmitStatement(statement->GetStatement());


		// emit the increment, if one exists
		if(statement->GetIncrementExpression())
		{
			EmitBasicBlock(incrementDest.GetBlock());
			EmitExpression(statement->GetIncrementExpression());
		}

		// step out of the break/continue scope
		scope->EndBreakContinueScope();

		// jump back to the condition block
		EmitBranch(conditionDest.GetBlock());


		// emit and finish with the exit block
		EmitBasicBlock(exitDest.GetBlock(), mo_true);
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(ForInStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRJumpDestination exitDest = GetJumpDestination(IR_BLOCK_NAME("ForInEnd"));
		IRJumpDestination continueDest = GetJumpDestination(IR_BLOCK_NAME("ForInNext"));
		IRBasicBlock bodyBlock = NewBasicBlock(IR_BLOCK_NAME("ForInBody"));
		IRValue base;
		IRValue basePropIterator;
		IRValue basePropIteratorIsNull;
		IRValue incPropIteratorResultIsTrue;
		
		if(!AST_IS_LOCATION_NODE(statement->GetLeft()))
		{
			// TODO: need to throw a reference error here
			mo_debug_print("EmitForInStatement: Left side of for-in statement is not a reference. NodeType=%d", statement->GetType());
			return;
		}

		// if there is an initialization, emit it now
		if(statement->GetInitializer())
			EmitExpression(statement->GetInitializer());

		// emit the right part of the for..in loop
		base = EmitExpression(statement->GetRight());


		// create a new property iterator over the base object and compare whether
		// the new property iterator is null or not
		basePropIterator = EmitAllocWithValue(IRBytePtrTy, mStubEmitter->EmitNewPropertyIterator(base), MO_IR_LOCAL_VAR_STRING(prop_iterator));
		basePropIteratorIsNull = mBuilder.CreateIsNull(EmitLoad(basePropIterator, IR_VAR_NAME("prop_iterator_value")), IR_VAR_NAME("prop_iterator_is_null")); //mBuilder.CreateICmpEQ(EmitLoad(basePropIterator, MO_IR_LOCAL_VAR_STRING(prop_iterator_value)), mGenerator->JSValueNullConstant, MO_IR_LOCAL_VAR_STRING(prop_iterator_is_null));

		// if the new property iterator is null then jump to the exit block
		// otherwise jump to the continuation block
		EmitBranch(basePropIteratorIsNull, exitDest.GetBlock(), continueDest.GetBlock());


		// emit the continuation block
		EmitBasicBlock(continueDest.GetBlock());

		// emit left of the for..in expression as well as the incrementing property iterator
		incPropIteratorResultIsTrue = mBuilder.CreateICmpEQ(
			EmitLeftForInExpression(statement->GetLeft(), basePropIterator), IRTypeBuilder::GetConstTrue(), IR_VAR_NAME("next_property_result"));

		// emit branch back to the body if a property name was assigned, otherwise 
		// branch out to the end
		EmitBranch(incPropIteratorResultIsTrue, bodyBlock, exitDest.GetBlock());

		// store the continue/break blocks
		scope->BeginBreakContinueScope(continueDest, exitDest);

		// emit the body of the loop
		EmitBasicBlock(bodyBlock);
		EmitStatement(statement->GetStatement());

		// no longer need this scope
		scope->EndBreakContinueScope();

		// emit continuation block and branch
		EmitBranch(continueDest.GetBlock());

		// emit the fall-through exit block
		EmitBasicBlock(exitDest.GetBlock(), mo_true);
	}

	//-------------------------------------------------------------------------------------------------
	// FIXME: Optimize switch statements when all types are the same
	//
	// • If the cases of a switch statement use all integers or all strings, we can omit the condition
	//   checks and use the integer/string lookup directly
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	DEFINE_EMIT_STATEMENT_FUNCTION(SwitchStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();
		IRJumpDestination exitDest = GetJumpDestination(IR_BLOCK_NAME("SwitchExit"));
		IRJumpDestination outerContinueDest;
		IRBasicBlock defaultBlock = NewBasicBlock(IR_BLOCK_NAME("SwitchCaseDefault"));
		IRValue condition = EmitCaseBlockNodeCondition(EmitExpression(statement->GetExpression()), statement->GetBlock());

		// begin a new switch, this allows for nested switches to properly continue/break
		scope->BeginSwitch(mBuilder.CreateSwitch(condition, defaultBlock));

		// end the current block, if one exists
		EndBasicBlock();
		
		// check if we are in a break/continue and use the outer continue destination
		if(scope->InBreakContinueScope())
			outerContinueDest = scope->GetContinueDestination();

		// step into a new break/continue scope
		scope->BeginBreakContinueScope(outerContinueDest, exitDest);

		// emit the case nodes
		EmitCaseBlockNode(statement->GetBlock());

		// step out of break/continue scope
		scope->EndBreakContinueScope();

		// set the default switch destination
		scope->GetSwitch()->setDefaultDest(defaultBlock);

		// if the default block has not been used then just replace it
		// with the exit block, no reason to have it if it's not being 
		// used anywhere
		if(!defaultBlock->getParent())
		{
			defaultBlock->replaceAllUsesWith(exitDest.GetBlock().Get());
			defaultBlock.Delete();
		}

		// emit the exit block and finish
		EmitBasicBlock(exitDest.GetBlock(), mo_true);

		// end the switch
		scope->EndSwitch();
	}

	DEFINE_EMIT_STATEMENT_FUNCTION(TryStatement)
	{
		IRFunctionScope* scope = GetFunctionScope();

		// the block for the fall-through edges of the catch handlers to jump to
		IRJumpDestination finallyEndDest = GetJumpDestination(IR_BLOCK_NAME("FinallyExit"));

		// the block for the rethrow edge of the catch handlers to jump to
		IRJumpDestination finallyRethrowDest = GetJumpDestination(IR_BLOCK_NAME("FinallyRethrow"));

		// the block for the statements within the try { .. }
		IRBasicBlock tryBlock = NewBasicBlock(IR_BLOCK_NAME("TryEnter"));

		// the block to handle exceptions thrown from TryEnter and begin the catch phase
		IRBasicBlock tryHandlerBlock = NewBasicBlock(IR_BLOCK_NAME("CatchEnter"));

		// allocate some memory for the setjmp buffer, this needs to be kept live
		// throughout the try and catch blocks so we allocate it in the function scope
		IRValue exceptionDataValue = EmitAllocStruct(mTypes.NativeExceptionDataTy, IR_INTERNAL_VAR_NAME("exception_data"));

		// create the fragile hazards, this will not capture any of the allocas 
		// required for exception processing, but it will capture the current 
		// basic block which extends all the way to the setjmp call (i.e. before the 'try')
		IRExceptionHandlingHazards hazards(this);

		// create a variable to hold a flag indicating whether the cleanup needs
		// to call mo_js_exception_try_exit. this is true except when:
		//    - no catches match and we're branching through the cleanup just to
		//      rethrow the exception, or..
		//    - a catch matched and we're falling out of the catch handler.
		// 
		// the setjmp safety rule here is that we should always store this variable
		// in a place that dominates the branch through the cleanup without passing
		// through any setjmps.
		IRValue gotoTryExitVariable = EmitAlloc(IRBitTy, IR_INTERNAL_VAR_NAME("goto_try_exit"));

		// a slot containing the exception to rethrow. this is only needed when
		// we have both a 'catch' and a 'finally' statement.
		IRValue propagatingExceptionVariable;


		// push a normal cleanup to leave the try scope
		mCleanupStack.Push<IRPerformFinallyCleanup>(kIRCleanupTypeNormalAndException, statement, gotoTryExitVariable.Get(), exceptionDataValue.Get());

		// emit the try/catch prologue
		EmitTryEnter(exceptionDataValue, gotoTryExitVariable, tryBlock, tryHandlerBlock, mo_false);

		// emit the statements for the try {..} block
		EmitStatement(statement->GetTryBlock());

		// save the current point
		IRBuilderDef::InsertPoint tryFallthroughIP = mBuilder.saveAndClearIP();

		// emit the exception handler block
		EmitBasicBlock(tryHandlerBlock);

		// dont optimize loads of the in-scope locals across this point
		hazards.EmitWriteHazard();

		// if the try has no catch, then just branch through the cleanup to the 
		// rethrow block
		if(!statement->HasCatchBlock())
		{
			// set flag to not call mo_exception_try_exit, otherwise the handler will be popped twice
			EmitStore(gotoTryExitVariable, IRTypeBuilder::GetConstFalse());
			EmitBranchThroughCleanup(finallyRethrowDest);
		}

		// otherwise, handle the caught exception. JavaScript does not use multiple catch handlers
		// for different exception types, it has one catch handler that receives a jsval which contains
		// the exception thrown.
		else
		{
			IRAutoCleanupScope catchScope(this);
			IRBasicBlock catchBlock = NewBasicBlock(IR_BLOCK_NAME("Catch"));
			IRBasicBlock catchHandlerBlock = NewBasicBlock(IR_BLOCK_NAME("CatchException"));

			mCleanupStack.Push<IRPerformCatchCleanup>(kIRCleanupTypeNormal);

			// retreive the exception object, we may emit multiple blocks but nothing can cross
			// this so the value is already in SSA form.
			IRValue caughtExceptionValue = EmitCallToExceptionExtract(exceptionDataValue);

			// save the currently propgating exception before mo_exception_try_enter clears it
			propagatingExceptionVariable = EmitAlloc(caughtExceptionValue->getType(), IR_INTERNAL_VAR_NAME("current_exception"));
			EmitStore(propagatingExceptionVariable.Get(), caughtExceptionValue);

			// begin a catch scope
			scope->BeginExceptionCatchScope(statement->GetExceptionId(), caughtExceptionValue);

			// enter a new try, if another exception is thrown while processing the catch 
			// block then we will still be able to properly exit out of the catch scope and
			// enter into a finally block, if there is one
			EmitTryEnter(exceptionDataValue, gotoTryExitVariable, catchBlock, catchHandlerBlock, mo_true);	

			// emit the catch statements
			EmitStatement(statement->GetCatchBlock());

			// done emiting the catch statements
			scope->EndExceptionCatchScope();

			// branch through to the exit
			EmitBranchThroughCleanup(finallyEndDest);

			// NOTE: leave this out for now while debugging
			//
			// if nothing wanted anything to do with the caught exception,
			// the kill the extract call.
			//if(caughtExceptionValue->use_empty())
			//	caughtExceptionValue->eraseFromParent();
			

			// emit the exception handler for the catch blocks
			EmitBasicBlock(catchHandlerBlock);
				
			// FIXME: do we need to emit a write hazard here?
			//
			// if there is no local-accessing code between
			// the try's write hazard and here, the no...

			// extract the new exception and save it to the propagating-exception slot
			MO_ASSERT(!propagatingExceptionVariable.IsNull());

			EmitStore(propagatingExceptionVariable.Get(), EmitCallToExceptionExtract(exceptionDataValue));

			// don't pop the catch handler, the throw already did
			EmitStore(gotoTryExitVariable, IRTypeBuilder::GetConstFalse());
			EmitBranchThroughCleanup(finallyRethrowDest);
		}

		// insert read hazards as required in the new blocks
		hazards.EmitHazardsInNewBlocks();

		// pop the cleanup
		mBuilder.restoreIP(tryFallthroughIP);

		if(InBasicBlock() && !gotoTryExitVariable->isUsedInBasicBlock(GetInsertBasicBlock()))
			EmitStore(gotoTryExitVariable, IRTypeBuilder::GetConstTrue());

		PopCleanupBlock();
		EmitBasicBlock(finallyEndDest.GetBlock(), mo_true);

		// save current point
		IRBuilderDef::InsertPoint savedIP = mBuilder.saveAndClearIP();

		// emit the re-throw block
		EmitBasicBlock(finallyRethrowDest.GetBlock(), mo_true);

		if(InBasicBlock())
		{
			// if we have a propagating exception variable, then check it
			IRValue propagatingException;

			if(!propagatingExceptionVariable.IsNull())
				propagatingException = EmitLoad(propagatingExceptionVariable, IR_INTERNAL_VAR_NAME("current_exception_value"));

			// otherwise, just look in the buffer for the exception to re-throw
			else
				propagatingException = EmitCallToExceptionExtract(exceptionDataValue);

			// throw the exception
			EmitCallToExceptionThrow(propagatingException);
			mBuilder.CreateUnreachable();
		}

		mBuilder.restoreIP(savedIP);	
	}

	IRValue IREmitter::EmitOp(Operator op, const IRValue& lhs, const IRValue& rhs)
	{
		switch(op)
		{
			case OpAndEqual:
				return mStubEmitter->EmitBitOp(lhs, rhs, kBitOpAND);
			case OpOrEqual:
				return mStubEmitter->EmitBitOp(lhs, rhs, kBitOpOR);
			case OpXOrEqual:
				return mStubEmitter->EmitBitOp(lhs, rhs, kBitOpXOR);
			case OpDivideEqual:
				return mStubEmitter->EmitDivide(lhs, rhs);
			case OpMinusEqual:
				return mStubEmitter->EmitSubtract(lhs, rhs);
			case OpModEqual:
				return mStubEmitter->EmitModulus(lhs, rhs);
			case OpMultiplyEqual:
				return mStubEmitter->EmitMultiply(lhs, rhs);
			case OpPlusEqual:
				return mStubEmitter->EmitAdd(lhs, rhs);
			case OpLShiftEqual:
				return mStubEmitter->EmitLeftShift(lhs, rhs);
			case OpRShiftEqual:
				return mStubEmitter->EmitRightShift(lhs, rhs);
			case OpRShiftUnsignedEqual:
				return mStubEmitter->EmitUnsignedRightShift(lhs, rhs);
			default:
				return LoadUndefinedValue(UTF8String("UNSUPPORTED_OP_") + mo_cast<mo_int32>(op));
		}
	}

	IRValue IREmitter::EmitExpressionAsBoolean(Expression* expr)
	{
		// emit the expression as usual
		IRValue value = EmitExpression(expr);

		// then check whether we can simply return it's value because we
		// know that the result is a jsval(bool), or if we have to first
		// convert it to a boolean value
		NodeType type = expr->GetType();

		switch(type)
		{
			case NODE_GREATER:
			case NODE_GREATER_EQ:
			case NODE_LESS:
			case NODE_LESS_EQ:
			case NODE_LOGICAL_NOT:
			case NODE_EQUAL:
			case NODE_EQUAL_STRICT:
			case NODE_NOT_EQUAL:
			case NODE_NOT_EQUAL_STRICT:
			case NODE_IN:
			case NODE_INSTANCEOF:
			case NODE_BOOLEAN:
				return value;
		}

		return mStubEmitter->EmitToBoolean(value);
	}

	void IREmitter::EmitTryEnter(const IRValue& exceptionDataValue, const IRValue& gotoTryExitVar, IRBasicBlock& normalBlock, IRBasicBlock& exceptionBlock, mo_bool inCatchBlock)
	{
		IRCallInst jmpValue;
		IRValue haveExceptionValue;
		IRValue exceptionDataBuf;

		// emit call to mo_exception_try_enter
		EmitCallToExceptionTryEnter(exceptionDataValue);

		// the statements of a catch block are executed in their own lexical environment, the
		// mo_exception_catch_enter will create and keep track of this new environment
		if(inCatchBlock)
			EmitCallToExceptionCatchEnter();
			
		// get a pointer to the 'buf' member field
		exceptionDataBuf = EmitStructGEP(exceptionDataValue, 0, 2, IR_INTERNAL_VAR_NAME("exception_data_buf"));

		// emit a call to setjmp
		jmpValue = EmitCallWithAttrs(mTypes.GetSetJmpFn(), exceptionDataBuf.Get(), kIRFunctionAttributeNoUnwind | kIRFunctionAttributeReturnsTwice, IR_CALL_RETVAL_NAME("setjmp"));

		// check if an exception was thrown
		haveExceptionValue = mBuilder.CreateIsNotNull(jmpValue.Get(), IR_INTERNAL_VAR_NAME("have_exception"));

		// branch to exception block if an exception was thrown, otherwise go to normal
		EmitBranch(haveExceptionValue, exceptionBlock, normalBlock);

		// emit the normal block
		EmitBasicBlock(normalBlock);

		// set flag to call mo_exception_try_exit
		EmitStore(gotoTryExitVar, IRTypeBuilder::GetConstTrue());
	}

	void IREmitter::EmitCaseBlockNode(CaseBlockNode* node)
	{
		mo_int32 caseId = 1;

		EmitClauseListNode(node->GetFirstList(), caseId);

		if(node->GetDefaultClause())
			EmitDefaultCaseClauseNode(node->GetDefaultClause());

		EmitClauseListNode(node->GetSecondList(), caseId);
	}

	void IREmitter::EmitClauseListNode(ClauseListNode* node, mo_int32& caseId)
	{
		for(ClauseListNode* list = node; list; list = list->GetNext(), ++caseId)
		{
			llvm::ConstantInt* caseValue = IRTypeBuilder::GetConstInt32(caseId);
			IRBasicBlock caseDestBlock = EmitBasicBlock(NewBasicBlock(IR_BLOCK_NAME("SwitchCase")));

			GetFunctionScope()->GetSwitch()->addCase(caseValue, caseDestBlock);

			EmitStatements(list->GetClause()->GetStatements());
		}
	}

	void IREmitter::EmitDefaultCaseClauseNode(CaseClauseNode* node)
	{
		IRBasicBlock defaultBlock = GetFunctionScope()->GetSwitch()->getDefaultDest();

		MO_ASSERT(defaultBlock->empty() && "EmitDefaultCaseClauseNode: Default block already defined?");

		EmitBasicBlock(defaultBlock);
		EmitStatements(node->GetStatements());
	}

	IRValue IREmitter::EmitCaseBlockNodeCondition(const IRValue& switchValue, CaseBlockNode* node)
	{
		IRBasicBlock beginBlock = NewBasicBlock(IR_BLOCK_NAME("SwitchCaseCondition"));
		IRBasicBlock defaultBlock = NewBasicBlock(IR_BLOCK_NAME("SwitchCaseConditionDefault"));
		IRBasicBlock returnBlock = NewBasicBlock(IR_BLOCK_NAME("SwitchSelect"));
		IRBasicBlock currentBlock = beginBlock;
		llvm::PHINode* phi = llvm::PHINode::Create(IRTypeBuilder::GetType(IRInt32Ty), 0);
		mo_int32 caseId = 1;

		// add the default condition block as our first phi node, no value in the
		// switch will be zero so this will always jump to whatever the default
		// switch destination is set to
		phi->addIncoming(IRTypeBuilder::GetConstInt32(0), defaultBlock);

		ClauseListNode* listNode = node->GetFirstList();
		mo_bool isFirstList = mo_true;

		while(listNode)
		{
			// get the next node, if one doesn't exist then use the
			// second list
			ClauseListNode* nextNode = listNode->GetNext();

			if(!nextNode && isFirstList)
			{
				nextNode = node->GetSecondList();
				isFirstList = mo_false;
			}

			// we need to have the next block before emitting the first one so we
			// have something to branch to when done, if there is no more case nodes
			// then we branch out to the default block
			IRBasicBlock nextBlock = nextNode ? NewBasicBlock(IR_BLOCK_NAME("SwitchCaseCondition")) : defaultBlock;

			// add the current block to the incoming phi nodes
			phi->addIncoming(IRTypeBuilder::GetConstInt32(caseId), currentBlock);

			// emit the current block
			EmitBasicBlock(currentBlock);

			// emit the case expression and comparison then branch to one
			// of returnBlock or the nextBlock in our list
			EmitBranch(EmitCaseClauseNodeCondition(switchValue, listNode->GetClause()), returnBlock, nextBlock);

			// update to the next block
			currentBlock = nextBlock;

			// move to the next node
			listNode = nextNode;
			caseId++;
		}

		// now emit the default condition block which is just a single branch to
		// the return block, this way the phi node knows to use the default value
		EmitBasicBlock(defaultBlock);
		EmitBranch(returnBlock);

		// emit the return block and add the phi node to it, this block
		// also then becomes our starting block for the switch
		EmitBasicBlock(returnBlock);

		return mBuilder.Insert(phi, MO_IR_LOCAL_VAR_STRING(switch_value));
	}

	IRValue IREmitter::EmitCaseClauseNodeCondition(const IRValue& switchValue, CaseClauseNode* node)
	{
		IRValue caseValue = EmitExpression(node->GetExpression());

		return mBuilder.CreateICmpEQ(mStubEmitter->EmitEqualStrict(switchValue, caseValue), mGenerator->JSValueTrueConstant, MO_IR_LOCAL_VAR_STRING(case_condition));
	}

	IRValue IREmitter::EmitLeftForInExpression(Expression* expr, const IRValue& propIterator)
	{
		IRValue propBase;
		IRValue propName;

		// figure out the property base and property name, these will be used to assign
		// the next property name value to it
		if(AST_IS_RESOLVE_NODE(expr))
		{
			const UTF8String& name = mo_cast<ResolvableExpression*>(expr)->GetID();

			propName = GetEncodedString(name);
			propBase = mStubEmitter->EmitResolveBase(propName);
		}
		else if(AST_IS_DOT_ACCESSOR_NODE(expr))
		{
			DotAccessorExpression* node = mo_cast<DotAccessorExpression*>(expr);
			propBase = EmitExpression(node->GetBase());
			propName = GetEncodedString(node->GetID());
		}
		else // must be a Bracket Accessor
		{
			MO_ASSERT(AST_IS_BRACKET_ACCESSOR_NODE(expr));

			BracketAccessorExpression* node = mo_cast<BracketAccessorExpression*>(expr);
			propBase = EmitExpression(node->GetBase());
			propName = EmitExpression(node->GetSubscript());
		}

		// assign the property name value to propBase[propName] and increment the iterator, if no more
		// properties are available, this just returns jsfalse, otherwise jstrue
		return mStubEmitter->EmitIncrementPropertyIterator(EmitLoad(propIterator, MO_IR_LOCAL_VAR_STRING(prop_iterator_value)), propBase, propName);
	}

	IRFunction IREmitter::EmitFunctionSignature(FunctionNode* funcNode)
	{
		llvm::Function* func = mo_null;
		llvm::FunctionType* type = mo_null;
		const UTF8String& name = funcNode->GetQualifiedName();

		// 
		// - functions defined in script must always return a value, if no
		//   return statement is found, then it simply returns 'undefined'
		//
		// - functions are in the following format:
		//      i64 f(i32* %callframe, i64 %1, i64 %2, i64 %n)
		//
		// - the first argument is always the callers stack frame, then
		//   the functions defined arguments follow.
		//

		// all functions always have at least one argument, which is
		std::vector<llvm::Type*> types(1, IRTypeBuilder::GetType(IRBytePtrTy));

		// append the functions actual arguments, all declared function arguments
		// are always int16 types, which represent an encoded JSValue
		//if(node->GetParameterCount() > 0)
		//	types.assign(node->GetParameterCount(), IRTypeBuilder::GetType(IRInt64Ty));
		
		// create the native function
		func = EmitFunction(MO_IR_JS_FUNCTION_VAR_NAME(name), IRTypeBuilder::GetFunctionType(IRInt64Ty, types));
		func->arg_begin()->setName(IRConsts::JSStackFrameCalleeName);

		//if(!func->arg_empty())
		//{
		//	llvm::Function::arg_iterator begin = func->arg_begin();

		//	// set the callframe's argument name
		//	begin->setName(IRConsts::JSStackFrameCalleeName);
		//	begin++;

		//	// set the remaining function names
		//	for(llvm::Function::arg_iterator it = begin; it != func->arg_end(); ++it)
		//	{
		//		mo_int32 n = std::distance(begin, it);

		//		if(n < node->GetParameterCount())
		//			(*it).setName(node->GetParameters()->at(n).GetChars());
		//	}
		//}

		return func;
	}

	void IREmitter::EmitFunctionBody(FunctionNode* funcNode, const IRFunction& nativeFunction)
	{
		BeginFunction(kIRFunctionTypeJS, nativeFunction);
		EmitFunctionPrologue(funcNode);
		EmitStatements(funcNode->GetStatements());

		if(!funcNode->HasReturnValue())
			EmitStore(GetFunctionScope()->GetReturnValue(), mGenerator->JSValueUndefinedConstant);

		EmitFunctionEpilogue();
		EndFunction();
	}



	//-------------------------------------------------------------------------------------------------
	// SECTION: Function Calls (Host and Native)
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	

	IRValue IREmitter::EmitJSCall(llvm::Function* func, ArgumentsNode* arguments)
	{
		std::vector<llvm::Value*> values;

		// when it's a js function, then we need to pass the 
		// current  js stack frame (the 'caller') along to 
		// the called function (the 'callee'), this will be 
		// used to construct the callee's stack frame.
		if(IsJSFunction(func))
		{
			MO_ASSERT(GetFunctionScope()->HasExecutionContext());

			IRValue jsCallFrame = GetFunctionScope()->GetExecutionContext();

			// load the call frame pointer to pass into the function
			values.push_back(mBuilder.CreateLoad(jsCallFrame, IRConsts::JSStackFrameCallerPtrName));
		}

		if(arguments->HasArguments())
		{
			ArgumentListNode* arg = arguments->GetFirstArgument();

			while(arg)
			{
				values.push_back(EmitExpression(arg->GetExpression()));
				arg = arg->GetNext();
			}
		}

		return EmitCall(func, values);
	}

	IRValue IREmitter::EmitHostCallByName(const UTF8String& functionName, const IRValue& baseValue, ArgumentsNode* arguments)
	{
		return EmitHostCallByName(GetEncodedString(functionName), baseValue, arguments);
	}

	IRValue IREmitter::EmitHostCallByName(const IRValue& functionName, const IRValue& baseValue, ArgumentsNode* arguments)
	{
		mo_uint32 argumentCount = 0;
		IRValue argumentValues = EmitArgumentsNode(arguments, &argumentCount, MO_IR_LOCAL_VAR_STRING(host_call_args));

		return mStubEmitter->EmitCallByName(functionName, baseValue.IsNull() ? mGenerator->JSValueUndefinedConstant : baseValue, argumentValues, argumentCount);
	}

	IRValue IREmitter::EmitHostCall(const IRValue& function, const IRValue& baseValue, ArgumentsNode* arguments)
	{
		mo_uint32 argumentCount = 0;
		IRValue argumentValues = EmitArgumentsNode(arguments, &argumentCount, MO_IR_LOCAL_VAR_STRING(host_call_args));

		return mStubEmitter->EmitCall(function, baseValue.IsNull() ? mGenerator->JSValueUndefinedConstant : baseValue, argumentValues, argumentCount);
	}



	//-------------------------------------------------------------------------------------------------
	// SECTION: Arguments List
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	

	IRValue IREmitter::EmitArgumentsNode(ArgumentsNode* node, mo_uint32* outCount, const UTF8String& name)
	{
		*outCount = 0;

		if(!node || !node->HasArguments())
			return IRValue::Null;

		std::vector<llvm::Value*> argumentValues;
		ArgumentListNode* arg = node->GetFirstArgument();
		
		while(arg)
		{
			llvm::Value* argVal = EmitExpression(arg->GetExpression());

			argumentValues.push_back(argVal);
			arg = arg->GetNext();
		}

		*outCount = argumentValues.size();

		return EmitArray(IRJSValueTy, argumentValues, name);
	}
}