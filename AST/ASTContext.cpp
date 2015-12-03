#include "AST/ASTContext.h"
#include "AST/AST.h"
#include "Parser/Lexer.h"
#include "Runtime/GlobalContext.h"
#include "Runtime/SourceCode.h"
#include "Runtime/JSConvert.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	#define SET_EXCEPTION_LOC(node) (node->SetExceptionLocation(divot, divot - start, end - divot))
	#define SET_LOC(node) (node->SetLocation(begin, end))
	#define STRIP_UNARY_PLUS(node) (AST_IS_UNARY_PLUS_NODE(node) ? MO_CAST_PTR(UnaryExpression, node)->GetExpression() : node)

	ASTContext::ASTContext(GlobalContext* globalContext, SourceCode* source, const Lexer* lexer)
		: mGlobalContext(globalContext)
		, mSource(source)
		, mLexer(lexer)
		, mEvalCount(0) { }

	Expression* ASTContext::CreateLogicalNot(Expression* expression)
	{
		return new LogicalNotExpression(GetLineNumber(), expression);
	}

	Expression* ASTContext::CreateUnaryPlus(Expression* expression)
	{
		return new UnaryPlusExpression(GetLineNumber(), expression);
	}

	Expression* ASTContext::CreateVoid(Expression* expression)
	{
		IncrementConstantCount();
		return new VoidExpression(GetLineNumber(), expression);
	}

	Expression* ASTContext::CreateThis()
	{
		SetUsesThis();
		return new ThisExpression(GetLineNumber());
	}

	Expression* ASTContext::CreateResolve(const UTF8String& id, mo_int32 start)
	{
		if(KnownIdentifiers::arguments == id)
			SetUsesArguments();

		return new ResolvableExpression(GetLineNumber(), id, start);
	}

	Expression* ASTContext::CreateObjectLiteral()
	{
		return new ObjectLiteralExpression(GetLineNumber());
	}

	Expression* ASTContext::CreateObjectLiteral(PropertyListNode* properties)
	{
		return new ObjectLiteralExpression(GetLineNumber(), properties);
	}

	Expression* ASTContext::CreateArray(Element* elements)
	{
		return new ArrayExpression(GetLineNumber(), elements);
	}

	Expression* ASTContext::CreateArray(Element* elements, mo_int32 elisions)
	{
		if(elisions > 0)
			IncrementConstantCount();

		return new ArrayExpression(GetLineNumber(), elisions, elements);
	}

	Expression* ASTContext::CreateArray(mo_int32 elisions)
	{
		if(elisions > 0)
			IncrementConstantCount();

		return new ArrayExpression(GetLineNumber(), elisions);
	}

	Expression* ASTContext::CreateNumber(mo_double value, mo_bool isConstant)
	{
		if(isConstant)
			IncrementConstantCount();

		return new NumberExpression(GetLineNumber(), value);
	}

	Expression* ASTContext::CreateString(const UTF8String& value)
	{
		IncrementConstantCount();
		return new StringExpression(GetLineNumber(), value);
	}

	Expression* ASTContext::CreateBoolean(mo_bool value)
	{
		IncrementConstantCount();
		return new BooleanExpression(GetLineNumber(), value);
	}

	Expression* ASTContext::CreateNull()
	{
		IncrementConstantCount();
		return new NullExpression(GetLineNumber());
	}

	Expression* ASTContext::CreateBracketAccess(Expression* base, Expression* property, mo_bool propertyHasAssignments, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		BracketAccessorExpression* expr = new BracketAccessorExpression(GetLineNumber(), base, property, propertyHasAssignments);
		SET_EXCEPTION_LOC(expr);

		return expr;
	}

	Expression* ASTContext::CreateDotAccess(Expression* base, const UTF8String& property, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		DotAccessorExpression* expr = new DotAccessorExpression(GetLineNumber(), base, property);
		SET_EXCEPTION_LOC(expr);

		return expr;
	}

	Expression* ASTContext::CreateRegExp(const UTF8String& pattern, const UTF8String& flags, mo_int32 start)
	{
		// TODO : need to validate the syntax of pattern
		//
		RegExExpression* expr = new RegExExpression(GetLineNumber(), pattern, flags);
		mo_int32 size = pattern.GetLength() + 2; /* need to add 2 for the two / chars */
		SetExceptionLocation(expr, start, start + size, start + size);

		return expr;
	}

	Expression* ASTContext::CreateNew(Expression* expression, ArgumentsNode* arguments, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		NewExpression* expr = new NewExpression(GetLineNumber(), expression, arguments);
		SET_EXCEPTION_LOC(expr);

		return expr;
	}

	Expression* ASTContext::CreateNew(Expression* expression, mo_int32 start, mo_int32 end)
	{
		NewExpression* expr = new NewExpression(GetLineNumber(), expression);
		expr->SetExceptionLocation(start, end, end);

		return expr;
	}

	Expression* ASTContext::CreateConditional(Expression* condition, Expression* lhs, Expression* rhs)
	{
		return new ConditionalExpression(GetLineNumber(), condition, lhs, rhs);
	}

	Expression* ASTContext::CreateAssignResolve(const UTF8String& id, Expression* rhs, mo_bool rhsHasAssignment, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(rhs->GetType() == NODE_FUNCTION_EXPR)
			static_cast<FunctionExpression*>(rhs)->GetFunction()->SetInferredName(id);

		AssignResolveExpression* expr = new AssignResolveExpression(GetLineNumber(), id, rhs, rhsHasAssignment);
		SET_EXCEPTION_LOC(expr);

		return expr;
	}

	Expression* ASTContext::CreateFunction(const UTF8String& name, FunctionNode* function, ParameterNode* parameters, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine)
	{
		FunctionExpression* expr = new FunctionExpression(GetLineNumber(), name, function, mSource->CreateExpression(openBracePosition, closeBracePosition, bodyBeginLine), parameters);

		mScope.mFunctions.Append(expr->GetFunction());
		function->SetLocation(bodyBeginLine, bodyEndLine);

		return expr;
	}

	Expression* ASTContext::CreateFunctionCall(Expression* function, ArgumentsNode* arguments, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(!AST_IS_LOCATION_NODE(function))
			return new ValueFunctionCallExpression(GetLineNumber(), function, arguments, divot, divot - start, end - divot);

		if(AST_IS_RESOLVE_NODE(function))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(function);
			const UTF8String& id = resolve->GetID();

			if(id == KnownIdentifiers::eval)
			{
				SetUsesEval();
				return new EvalFunctionCallExpression(GetLineNumber(), arguments, divot, divot - start, end - divot);
			}

			return new ResolveFunctionCallExpression(GetLineNumber(), id, arguments, divot, divot - start, end - divot);
		}

		if(AST_IS_BRACKET_ACCESSOR_NODE(function))
		{
			BracketAccessorExpression* bracket = static_cast<BracketAccessorExpression*>(function);
			BracketFunctionCallExpression* node = new BracketFunctionCallExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), arguments, divot, divot - start, end - divot);
			node->SetExpressionInfo(bracket->GetDivot(), bracket->GetEndPosition());

			return node;
		}

		MO_ASSERT(AST_IS_DOT_ACCESSOR_NODE(function));

		DotAccessorExpression* dot = static_cast<DotAccessorExpression*>(function);
		DotFunctionCallExpression* node = mo_null;
		
		if(dot->GetID() == KnownIdentifiers::call)
			node = new CallDotFunctionCallExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), arguments, divot, divot - start, end - divot);
		else if(dot->GetID() == KnownIdentifiers::apply)
			node = new ApplyDotFunctionCallExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), arguments, divot, divot - start, end - divot);
		else
			node = new DotFunctionCallExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), arguments, divot, divot - start, end - divot);

		node->SetExpressionInfo(dot->GetDivot(), dot->GetEndPosition());

		return node;
	}

	Expression* ASTContext::CreateAssignment(mo_int32& assignmentStackDepth, Expression* rhs, mo_int32 initialCount, mo_int32 currentCount, mo_int32 lastTokenEnd)
	{
		AssignmentInfo assignInfo = mAssignmentStack.back();
		Expression* expr = CreateAssignment(assignInfo.op, assignInfo.node, rhs, assignInfo.initAssignments != initialCount, assignInfo.initAssignments != currentCount, assignInfo.start, assignInfo.divot + 1, lastTokenEnd);

		mAssignmentStack.pop_back();
		assignmentStackDepth--;

		return expr;
	}

	Expression* ASTContext::CreateAssignment(Operator op, Expression* left, Expression* right, mo_bool leftHasAssignments, mo_bool rightHasAssignments, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(!AST_IS_LOCATION_NODE(left))
			return new AssignErrorExpression(GetLineNumber(), left, right, op, divot, divot - start, end - divot);

		if(AST_IS_RESOLVE_NODE(left))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(left);

			if(op == OpEqual)
			{
				if(AST_IS_FUNCTION_EXPRESSION_NODE(right))
					static_cast<FunctionExpression*>(right)->GetFunction()->SetInferredName(resolve->GetID());

				AssignResolveExpression* node = new AssignResolveExpression(GetLineNumber(), resolve->GetID(), right, rightHasAssignments);
				SET_EXCEPTION_LOC(node);

				return node;
			}

			return new ReadModifyResolveExpression(GetLineNumber(), resolve->GetID(), op, right, rightHasAssignments, divot, divot - start, end - divot);
		}

		if(AST_IS_BRACKET_ACCESSOR_NODE(left))
		{
			BracketAccessorExpression* bracket = static_cast<BracketAccessorExpression*>(left);

			if(op == OpEqual)
				return new AssignBracketExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), right, leftHasAssignments, rightHasAssignments, bracket->GetDivot(), bracket->GetDivot() - start, end - bracket->GetDivot());

			ReadModifyBracketExpression* node = new ReadModifyBracketExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), op, right, leftHasAssignments, rightHasAssignments, divot, divot - start, end - divot);
			node->SetExpressionInfo(bracket->GetDivot(), bracket->GetEndPosition());

			return node;
		}

		MO_ASSERT(AST_IS_DOT_ACCESSOR_NODE(left));

		DotAccessorExpression* dot = static_cast<DotAccessorExpression*>(left);

		if(op == OpEqual)
		{
			if(AST_IS_FUNCTION_EXPRESSION_NODE(right))
				static_cast<FunctionExpression*>(right)->GetFunction()->SetInferredName(dot->GetID());

			return new AssignDotExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), right, rightHasAssignments, dot->GetDivot(), dot->GetDivot() - start, end - dot->GetDivot());
		}

		ReadModifyDotExpression* node = new ReadModifyDotExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), op, right, rightHasAssignments, divot, divot - start, end - divot);
		node->SetExpressionInfo(dot->GetDivot(), dot->GetEndPosition());

		return node;
	}

	Expression* ASTContext::CreatePrefix(Expression* expression, Operator op, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(!AST_IS_LOCATION_NODE(expression))
			return new PrefixErrorExpression(GetLineNumber(), expression, op, divot, divot - start, end - divot);

		if(AST_IS_RESOLVE_NODE(expression))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(expression);
			return new PrefixResolveExpression(GetLineNumber(), op, resolve->GetID(), divot, divot - start, end - divot);
		}

		if(AST_IS_BRACKET_ACCESSOR_NODE(expression))
		{
			BracketAccessorExpression* bracket = static_cast<BracketAccessorExpression*>(expression);
			PrefixBracketExpression* node = new PrefixBracketExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), op, divot, divot - start, end - divot);
			node->SetExpressionInfo(bracket->GetDivot(), bracket->GetStartPosition());

			return node;
		}

		MO_ASSERT(AST_IS_DOT_ACCESSOR_NODE(expression));

		DotAccessorExpression* dot = static_cast<DotAccessorExpression*>(expression);
		PrefixDotExpression* node = new PrefixDotExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), op, divot, divot - start, end - divot);
		node->SetExpressionInfo(dot->GetDivot(), dot->GetStartPosition());

		return node;
	}

	Expression* ASTContext::CreatePostfix(Expression* expression, Operator op, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(!AST_IS_LOCATION_NODE(expression))
			return new PostfixErrorExpression(GetLineNumber(), expression, op, divot, divot - start, end - divot);

		if(AST_IS_RESOLVE_NODE(expression))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(expression);
			return new PostfixResolveExpression(GetLineNumber(), op, resolve->GetID(), divot, divot - start, end - divot);
		}

		if(AST_IS_BRACKET_ACCESSOR_NODE(expression))
		{
			BracketAccessorExpression* bracket = static_cast<BracketAccessorExpression*>(expression);
			PostfixBracketExpression* node = new PostfixBracketExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), op, divot, divot - start, end - divot);
			node->SetExpressionInfo(bracket->GetDivot(), bracket->GetEndPosition());

			return node;
		}

		MO_ASSERT(AST_IS_DOT_ACCESSOR_NODE(expression));

		DotAccessorExpression* dot = static_cast<DotAccessorExpression*>(expression);
		PostfixDotExpression* node = new PostfixDotExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), op, divot, divot - start, end - divot);
		node->SetExpressionInfo(dot->GetDivot(), dot->GetEndPosition());

		return node;
	}

	Expression* ASTContext::CreateTypeOf(Expression* expression)
	{
		if(AST_IS_RESOLVE_NODE(expression))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(expression);
			return new TypeOfResolveExpression(GetLineNumber(), resolve->GetID());
		}

		return new TypeOfValueExpression(GetLineNumber(), expression);
	}

	Expression* ASTContext::CreateDelete(Expression* expression, mo_int32 start, mo_int32 divot, mo_int32 end)
	{
		if(!AST_IS_LOCATION_NODE(expression))
			return new DeleteValueExpression(GetLineNumber(), expression);

		if(AST_IS_RESOLVE_NODE(expression))
		{
			ResolvableExpression* resolve = static_cast<ResolvableExpression*>(expression);
			return new DeleteResolveExpression(GetLineNumber(), resolve->GetID(), divot, divot - start, end - divot);
		}

		if(AST_IS_BRACKET_ACCESSOR_NODE(expression))
		{
			BracketAccessorExpression* bracket = static_cast<BracketAccessorExpression*>(expression);
			return new DeleteBracketExpression(GetLineNumber(), bracket->GetBase(), bracket->GetSubscript(), divot, divot - start, end - divot);
		}

		MO_ASSERT(AST_IS_DOT_ACCESSOR_NODE(expression));

		DotAccessorExpression* dot = static_cast<DotAccessorExpression*>(expression);
		return new DeleteDotExpression(GetLineNumber(), dot->GetBase(), dot->GetID(), divot, divot - start, end - divot);
	}

	Expression* ASTContext::CreateNegate(Expression* expression)
	{
		if(AST_IS_NUMBER_NODE(expression))
		{
			NumberExpression* num = MO_CAST_PTR(NumberExpression, expression);
			num->SetValue(-num->GetValue());
			return num;
		}

		return new NegateExpression(GetLineNumber(), expression);
	}

	Expression* ASTContext::CreateBitwiseNot(Expression* expression)
	{
		if(AST_IS_NUMBER_NODE(expression))
			return CreateNumber(~JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, expression)->GetValue()), false);

		return new BitwiseNotExpression(GetLineNumber(), expression);
	}

	StatementList* ASTContext::CreateStatementList()
	{
		return new StatementList();
	}

	Statement* ASTContext::CreateFunctionDeclaration(const UTF8String& name, FunctionNode* function, ParameterNode* parameters, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine)
	{
		FunctionDeclStatement* decl = new FunctionDeclStatement(GetLineNumber(), name, function, mSource->CreateExpression(openBracePosition, closeBracePosition, bodyBeginLine), parameters);

		if(name == KnownIdentifiers::arguments)
			SetUsesArguments();

		mScope.mFunctions.Append(decl->GetFunction());
		function->SetLocation(bodyBeginLine, bodyEndLine);

		return decl;
	}

	Statement* ASTContext::CreateBlock(StatementList* statements, mo_int32 begin, mo_int32 end)
	{
		BlockStatement* s = new BlockStatement(GetLineNumber(), statements);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateExpression(Expression* expression, mo_int32 begin, mo_int32 end)
	{
		ExpressionStatement* s = new ExpressionStatement(GetLineNumber(), expression);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateIf(Expression* condition, Statement* trueBlock, mo_int32 begin, mo_int32 end)
	{
		IfStatement* s = new IfStatement(GetLineNumber(), condition, trueBlock);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateIf(Expression* condition, Statement* trueBlock, Statement* falseBlock, mo_int32 begin, mo_int32 end)
	{
		IfElseStatement* s = new IfElseStatement(GetLineNumber(), condition, trueBlock, falseBlock);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateForLoop(Expression* initializer, Expression* condition, Expression* iter, Statement* statements, mo_bool isFirstExprVarDecl, mo_int32 begin, mo_int32 end)
	{
		ForStatement* s = new ForStatement(GetLineNumber(), initializer, condition, iter, statements, isFirstExprVarDecl);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateForInLoop(const UTF8String& id, Expression* initializer, Expression* iter, Statement* statements, mo_int32 start, mo_int32 divot, mo_int32 end, mo_int32 initBegin, mo_int32 initEnd, mo_int32 beginLine, mo_int32 endLine)
	{
		ForInStatement* s = new ForInStatement(mGlobalContext, GetLineNumber(), initializer, iter, statements, id, initBegin, initBegin - start, initEnd - initBegin);
		s->SetLocation(beginLine, endLine);

		SetExceptionLocation(s, start, divot + 1, end);

		return s;

	}

	Statement* ASTContext::CreateForInLoop(Expression* lhs, Expression* iter, Statement* statements, mo_int32 start, mo_int32 divot, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		ForInStatement* s = new ForInStatement(mGlobalContext, GetLineNumber(), lhs, iter, statements);
		s->SetLocation(beginLine, endLine);
		SET_EXCEPTION_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateEmpty()
	{
		return new EmptyStatement(GetLineNumber());
	}

	Statement* ASTContext::CreateVar(Expression* expression, mo_int32 begin, mo_int32 end)
	{
		Statement* s = mo_null;

		if(!expression)
			s = new EmptyStatement(GetLineNumber());
		else
			s = new VarStatement(GetLineNumber(), expression);

		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateReturn(Expression* expression, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		ReturnStatement* s = new ReturnStatement(GetLineNumber(), expression);
		SetExceptionLocation(s, start, end, end);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateBreak(mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		BreakStatement* s = new BreakStatement(GetLineNumber());
		SetExceptionLocation(s, start, end, end);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateBreak(const UTF8String& id, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		BreakStatement* s = new BreakStatement(GetLineNumber(), id);
		SetExceptionLocation(s, start, end, end);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateContinue(mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		ContinueStatement* s = new ContinueStatement(mGlobalContext, GetLineNumber());
		SetExceptionLocation(s, start, end, end);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateContinue(const UTF8String& id, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		ContinueStatement* s = new ContinueStatement(GetLineNumber(), id);
		SetExceptionLocation(s, start, end, end);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateTry(const UTF8String& id, Statement* tryBlock, Statement* catchBlock, Statement* finallyBlock, mo_int32 begin, mo_int32 end)
	{
		TryStatement* s = new TryStatement(GetLineNumber(), id, tryBlock, catchBlock, finallyBlock);

		if(catchBlock)
			SetUsesCatch();

		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateSwitch(Expression* expression, ClauseListNode* firstClauses, CaseClauseNode* defaultClause, ClauseListNode* secondClauses, mo_int32 begin, mo_int32 end)
	{
		CaseBlockNode* cases = new CaseBlockNode(firstClauses, defaultClause, secondClauses);
		SwitchStatement* s = new SwitchStatement(GetLineNumber(), expression, cases);
		
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateWhile(Expression* expression, Statement* statement, mo_int32 begin, mo_int32 end)
	{
		WhileStatement* s = new WhileStatement(GetLineNumber(), expression, statement);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateDoWhile(Expression* expression, Statement* statement, mo_int32 begin, mo_int32 end)
	{
		DoWhileStatement* s = new DoWhileStatement(GetLineNumber(), statement, expression);
		SET_LOC(s);

		return s;
	}

	Statement* ASTContext::CreateLabel(const UTF8String& id, Statement* statement, mo_int32 begin, mo_int32 end)
	{
		LabelStatement* s = new LabelStatement(GetLineNumber(), id, statement);
		SetExceptionLocation(s, begin, end, end);

		return s;
	}

	Statement* ASTContext::CreateWith(Expression* expression, Statement* statement, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		WithStatement* s = new WithStatement(GetLineNumber(), expression, statement, end, end - start);
		s->SetLocation(beginLine, endLine);

		SetUsesWith();

		return s;
	}

	Statement* ASTContext::CreateThrow(Expression* expression, mo_int32 start, mo_int32 end, mo_int32 beginLine, mo_int32 endLine)
	{
		ThrowStatement* s = new ThrowStatement(GetLineNumber(), expression);
		s->SetLocation(beginLine, endLine);

		SetExceptionLocation(s, start, end, end);

		return s;
	}

	Statement* ASTContext::CreateDebugger(mo_int32 beginLine, mo_int32 endLine)
	{
		DebuggerStatement* s = new DebuggerStatement(GetLineNumber());
		s->SetLocation(beginLine, endLine);

		return s;
	}

	Statement* ASTContext::CreateConst(ConstDeclExpression* decls, mo_int32 beginLine, mo_int32 endLine)
	{
		ConstStatement* s = new ConstStatement(GetLineNumber(), decls);
		s->SetLocation(beginLine, endLine);

		return s;
	}

	ConstDeclExpression* ASTContext::CreateConstDecl(const UTF8String& name, Expression* initializer, ConstDeclExpression* next)
	{
		return new ConstDeclExpression(GetLineNumber(), name, initializer, next);
	}

	CommaExpression* ASTContext::CreateComma(Expression* lhs, Expression* rhs)
	{
		return new CommaExpression(GetLineNumber(), lhs, rhs);
	}

	FunctionNode* ASTContext::CreateFunction(mo_bool inStrictContext)
	{
		return new FunctionNode(mGlobalContext, GetLineNumber(), inStrictContext);
	}

	FunctionNode* ASTContext::CreateFunction(StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_bool hasReturn)
	{
		return new FunctionNode(mGlobalContext, GetLineNumber(), mSource->CreateExpression(openBracePosition, closeBracePosition, bodyBeginLine), statements, variables, functions, capturedVariables, features, constantCount, hasReturn);
	}

	PropertyNode* ASTContext::CreateGetterOrSetter(PropertyType type, const UTF8String& name, ParameterNode* parameters, FunctionNode* function, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine)
	{
		MO_ASSERT(!name.IsEmpty());

		function->SetLocation(bodyBeginLine, bodyEndLine);
		function->SetInferredName(name);

		return new PropertyNode(name, new FunctionExpression(GetLineNumber(), KnownIdentifiers::Null, function, mSource->CreateExpression(openBracePosition, closeBracePosition, bodyBeginLine), parameters), type);
	}

	PropertyNode* ASTContext::CreateGetterOrSetter(PropertyType type, mo_double name, ParameterNode* parameters, FunctionNode* function, mo_int32 openBracePosition, mo_int32 closeBracePosition, mo_int32 bodyBeginLine, mo_int32 bodyEndLine)
	{
		function->SetLocation(bodyBeginLine, bodyEndLine);

		return new PropertyNode(name, new FunctionExpression(GetLineNumber(), KnownIdentifiers::Null, function, mSource->CreateExpression(openBracePosition, closeBracePosition, bodyBeginLine), parameters), type);
	}

	PropertyNode* ASTContext::CreateProperty(const UTF8String& propertyName, Expression* expression, PropertyType type)
	{
		if(expression->GetType() == NODE_FUNCTION_EXPR)
			static_cast<FunctionExpression*>(expression)->GetFunction()->SetInferredName(propertyName);

		return new PropertyNode(propertyName, expression, type);
	}

	PropertyNode* ASTContext::CreateProperty(mo_double propertyName, Expression* expression, PropertyType type)
	{
		return new PropertyNode(propertyName, expression, type);
	}

	PropertyListNode* ASTContext::CreatePropertyList(PropertyNode* property)
	{
		return new PropertyListNode(GetLineNumber(), property);
	}

	PropertyListNode* ASTContext::CreatePropertyList(PropertyNode* property, PropertyListNode* tail)
	{
		return new PropertyListNode(GetLineNumber(), property, tail);
	}

	ArgumentsNode* ASTContext::CreateArguments()
	{
		return new ArgumentsNode();
	}

	ArgumentsNode* ASTContext::CreateArguments(ArgumentListNode* arguments)
	{
		return new ArgumentsNode(arguments);
	}

	ArgumentListNode* ASTContext::CreateArgumentsList(Expression* argument)
	{
		return new ArgumentListNode(GetLineNumber(), argument);
	}

	ArgumentListNode* ASTContext::CreateArgumentsList(ArgumentListNode* arguments, Expression* argument)
	{
		return new ArgumentListNode(GetLineNumber(), arguments, argument);
	}

	Element* ASTContext::CreateElementList(Expression* expression, mo_int32 elisions)
	{
		return new Element(elisions, expression);
	}

	Element* ASTContext::CreateElementList(Expression* expression, mo_int32 elisions, Element* elements)
	{
		return new Element(elisions, expression, elements);
	}

	ParameterNode* ASTContext::CreateParameterList(const UTF8String& id)
	{
		return new ParameterNode(id);
	}

	ParameterNode* ASTContext::CreateParameterList(const UTF8String& id, ParameterNode* list)
	{
		return new ParameterNode(id, list);
	}

	CaseClauseNode* ASTContext::CreateClause(Expression* expression, StatementList* statements)
	{
		return new CaseClauseNode(expression, statements);
	}

	ClauseListNode* ASTContext::CreateClauseList(CaseClauseNode* clause)
	{
		return new ClauseListNode(clause);
	}

	ClauseListNode* ASTContext::CreateClauseList(CaseClauseNode* clause, ClauseListNode* tail)
	{
		return new ClauseListNode(clause, tail);
	}

	void ASTContext::AddVariable(const UTF8String& id, mo_unsigned attributes)
	{
		if(id == KnownIdentifiers::arguments)
			SetUsesArguments();

		mScope.mVariableDecls.Append(id, attributes);
	}

	Expression* ASTContext::CombineCommaExpressions(Expression* list, Expression* init)
	{
		if(list == mo_null)
			return init;

		if(AST_IS_COMMA_NODE(list))
		{
			MO_CAST_PTR(CommaExpression, list)->Append(init);
			return list;
		}

		return new CommaExpression(GetLineNumber(), list, init);
	}

	mo_bool ASTContext::PrecedenceIsLowerThanOperatorStack(mo_int32 precedence)
	{
		return (precedence <= mBinaryOperatorStack.back().second);
	}

	void ASTContext::FinishBinaryOperation(mo_int32& operandStackDepth, mo_int32& operatorStackDepth)
	{
		BinaryOperand rhs = mBinaryOperandStack[mBinaryOperandStack.size() - 1];
		BinaryOperand lhs = mBinaryOperandStack[mBinaryOperandStack.size() - 2];

		// decrement the two we just took from the stack
		operandStackDepth -= 2;

		// ensure we have not gone under
		MO_ASSERT(operandStackDepth >= 0);

		// resize the stack to remove the two operands
		mBinaryOperandStack.resize(mBinaryOperandStack.size() - 2);

		// append a new operand from the last two
		mBinaryOperandStack.push_back(MakeOperand(CreateBinary(mBinaryOperatorStack.back().first, lhs, rhs), BinaryOpInfo(lhs.info, rhs.info)));

		// remove the last operator
		mBinaryOperatorStack.pop_back();

		// save reference depths
		operatorStackDepth--;
		operandStackDepth++;
	}

	void ASTContext::AppendBinaryOperator(mo_int32& operatorStackDepth, mo_int32 op, mo_int32 precedence)			
	{
		operatorStackDepth++;
		mBinaryOperatorStack.push_back(std::make_pair(op, precedence));
	}

	Expression* ASTContext::PopBinaryOperand()
	{
		BinaryOperand operand = mBinaryOperandStack.back();
		mBinaryOperandStack.pop_back();

		return operand.node;
	}

	void ASTContext::AppendBinaryExpressionInfo(mo_int32& operandStackDepth, Expression* current, mo_int32 start, mo_int32 lhs, mo_int32 rhs, mo_bool hasAssignments)
	{
		operandStackDepth++;
		mBinaryOperandStack.push_back(MakeOperand(current, BinaryOpInfo(start, lhs, rhs, hasAssignments)));
	}

	void ASTContext::AppendUnaryToken(mo_int32& tokenStackDepth, mo_int32 type, mo_int32 start)
	{
		tokenStackDepth++;
		mUnaryTokenStack.push_back(std::make_pair(type, start));
	}

	void ASTContext::RemoveLastUnaryToken(mo_int32& tokenStackDepth)
	{
		tokenStackDepth--;
		mUnaryTokenStack.pop_back();
	}

	mo_int32 ASTContext::GetLastUnaryTokenType()
	{
		return mUnaryTokenStack.back().first;
	}

	mo_int32 ASTContext::GetLastUnaryTokenStart()
	{
		return mUnaryTokenStack.back().second;
	}

	void ASTContext::AppendAssignment(mo_int32& depth, mo_int32 count, Expression* expression, Operator op, mo_int32 start, mo_int32 divot)
	{
		depth++;
		mAssignmentStack.push_back(AssignmentInfo(expression, start, divot, count, op));
	}

	Expression* ASTContext::CreateBinary(mo_int32 token, BinaryOperand lhs, BinaryOperand rhs)
	{
		switch(token)
		{
			case TOK_OR:
				return new LogicalExpression(GetLineNumber(), lhs.node, rhs.node, OpLogicalOr);
			case TOK_AND:
				return new LogicalExpression(GetLineNumber(), lhs.node, rhs.node, OpLogicalAnd);
			case TOK_BIT_OR:
				return CreateBitwiseOr(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_BIT_XOR:
				return CreateBitwiseXOr(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_BIT_AND:
				return CreateBitwiseAnd(lhs.node, rhs.node, rhs.info.hasAssignment);

			// FIXME - we can optimize the following four equality expressions by performing the equality test when
			//         both the left and right nodes are of the same primitive type and putting the result in a 
			//         BooleanExpression and returning that instead of doing the test at runtime.
			case TOK_EQEQ:
				return new EqualExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_NE:
				return new NotEqualExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_STREQ:
				return new EqualStrictExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_STRNEQ:
				return new NotEqualStrictExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_LT:
				return CreateLessThan(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_LE:
				return CreateLessThanOrEqual(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_GT:
				return CreateGreaterThan(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_GE:
				return CreateGreaterThanOrEqual(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_INSTANCEOF:
			{
				InstanceOfExpression* expr = new InstanceOfExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
				SetExceptionLocation(expr, lhs.info.start, rhs.info.start, rhs.info.end);
				return expr;
			}
			case TOK_IN:
			{
				InExpression* expr = new InExpression(GetLineNumber(), lhs.node, rhs.node, rhs.info.hasAssignment);
				SetExceptionLocation(expr, lhs.info.start, rhs.info.start, rhs.info.end);
				return expr;
			}
			case TOK_LSHIFT:
				return CreateLeftShift(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_RSHIFT:
				return CreateRightShift(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_URSHIFT:
				return CreateRightShiftUnsigned(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_PLUS:
				return CreateAdd(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_MINUS:
				return CreateSubtract(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_TIMES:
				return CreateMultiply(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_DIVIDE:
				return CreateDivide(lhs.node, rhs.node, rhs.info.hasAssignment);
			case TOK_MOD:
				return CreateModulus(lhs.node, rhs.node, rhs.info.hasAssignment);
		}

		MO_ASSERT_NOT_REACHED();
		return mo_null;
	}

	Expression* ASTContext::CreateMultiply(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		left = STRIP_UNARY_PLUS(left);
		right = STRIP_UNARY_PLUS(right);

		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(MO_CAST_PTR(NumberExpression, left)->GetValue() * MO_CAST_PTR(NumberExpression, right)->GetValue(), false);

		if(AST_IS_NUMBER_NODE(left) && MO_CAST_PTR(NumberExpression, left)->GetValue() == 1)
			return new UnaryPlusExpression(GetLineNumber(), right);

		if(AST_IS_NUMBER_NODE(right) && MO_CAST_PTR(NumberExpression, right)->GetValue() == 1)
			return new UnaryPlusExpression(GetLineNumber(), left);

		return new MultiplyExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateDivide(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		left = STRIP_UNARY_PLUS(left);
		right = STRIP_UNARY_PLUS(right);

		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(MO_CAST_PTR(NumberExpression, left)->GetValue() / MO_CAST_PTR(NumberExpression, right)->GetValue(), false);

		return new DivideExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateModulus(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		left = STRIP_UNARY_PLUS(left);
		right = STRIP_UNARY_PLUS(right);

		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(fmod(MO_CAST_PTR(NumberExpression, left)->GetValue(),MO_CAST_PTR(NumberExpression, right)->GetValue()), false);

		return new ModulusExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateAdd(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(MO_CAST_PTR(NumberExpression, left)->GetValue() + MO_CAST_PTR(NumberExpression, right)->GetValue(), false);

		return new AddExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateSubtract(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		left = STRIP_UNARY_PLUS(left);
		right = STRIP_UNARY_PLUS(right);

		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(MO_CAST_PTR(NumberExpression, left)->GetValue() - MO_CAST_PTR(NumberExpression, right)->GetValue(), false);

		return new SubtractExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateLessThan(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateBoolean(mo_cast<NumberExpression*>(left)->GetValue() < mo_cast<NumberExpression*>(right)->GetValue());

		if(AST_IS_BOOLEAN_NODE(left) && AST_IS_BOOLEAN_NODE(right))
			return CreateBoolean(mo_cast<BooleanExpression*>(left)->GetValue() < mo_cast<BooleanExpression*>(right)->GetValue());

		if(AST_IS_STRING_NODE(left) && AST_IS_STRING_NODE(right))
			return CreateBoolean(mo_cast<StringExpression*>(left)->GetValue() < mo_cast<StringExpression*>(right)->GetValue());

		return new LessExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateLessThanOrEqual(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateBoolean(mo_cast<NumberExpression*>(left)->GetValue() <= mo_cast<NumberExpression*>(right)->GetValue());

		if(AST_IS_BOOLEAN_NODE(left) && AST_IS_BOOLEAN_NODE(right))
			return CreateBoolean(mo_cast<BooleanExpression*>(left)->GetValue() <= mo_cast<BooleanExpression*>(right)->GetValue());

		if(AST_IS_STRING_NODE(left) && AST_IS_STRING_NODE(right))
			return CreateBoolean((mo_cast<StringExpression*>(left)->GetValue() < mo_cast<StringExpression*>(right)->GetValue()) || (mo_cast<StringExpression*>(left)->GetValue() == mo_cast<StringExpression*>(right)->GetValue()));

		return new LessEqualExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateGreaterThan(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateBoolean(mo_cast<NumberExpression*>(left)->GetValue() > mo_cast<NumberExpression*>(right)->GetValue());

		if(AST_IS_BOOLEAN_NODE(left) && AST_IS_BOOLEAN_NODE(right))
			return CreateBoolean(mo_cast<BooleanExpression*>(left)->GetValue() > mo_cast<BooleanExpression*>(right)->GetValue());

		if(AST_IS_STRING_NODE(left) && AST_IS_STRING_NODE(right))
			return CreateBoolean(mo_cast<StringExpression*>(left)->GetValue() > mo_cast<StringExpression*>(right)->GetValue());

		return new GreaterExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateGreaterThanOrEqual(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateBoolean(mo_cast<NumberExpression*>(left)->GetValue() >= mo_cast<NumberExpression*>(right)->GetValue());

		if(AST_IS_BOOLEAN_NODE(left) && AST_IS_BOOLEAN_NODE(right))
			return CreateBoolean(mo_cast<BooleanExpression*>(left)->GetValue() >= mo_cast<BooleanExpression*>(right)->GetValue());

		if(AST_IS_STRING_NODE(left) && AST_IS_STRING_NODE(right))
			return CreateBoolean((mo_cast<StringExpression*>(left)->GetValue() > mo_cast<StringExpression*>(right)->GetValue()) || (mo_cast<StringExpression*>(left)->GetValue() == mo_cast<StringExpression*>(right)->GetValue()));

		return new GreaterEqualExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateBitwiseAnd(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) & JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()), false);

		return new BitwiseAndExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateBitwiseOr(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) | JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()), false);

		return new BitwiseOrExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateBitwiseXOr(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) ^ JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()), false);

		return new BitwiseXOrExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateLeftShift(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) << (JSConvert::ToUInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()) & 0x1f), false);

		return new LeftShiftExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateRightShift(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) >> (JSConvert::ToUInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()) & 0x1f), false);

		return new RightShiftExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	Expression* ASTContext::CreateRightShiftUnsigned(Expression* left, Expression* right, mo_bool rightHasAssignments)
	{
		if(AST_IS_NUMBER_NODE(left) && AST_IS_NUMBER_NODE(right))
			return CreateNumber(JSConvert::ToUInt32(MO_CAST_PTR(NumberExpression, left)->GetValue()) >> (JSConvert::ToUInt32(MO_CAST_PTR(NumberExpression, right)->GetValue()) & 0x1f), false);

		return new RightShiftUnsignedExpression(GetLineNumber(), left, right, rightHasAssignments);
	}

	const mo_int32 ASTContext::GetLineNumber() const
	{
		return mLexer->GetLastLineNumber();
	}

	#undef SET_EXCEPTION_LOC
	#undef SET_LOC
	#undef STRIP_UNARY_PLUS
}