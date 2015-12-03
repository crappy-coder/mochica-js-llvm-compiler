#include "Parser/Parser.h"
#include "Runtime/KnownIdentifiers.h"
#include <Poco/HashMap.h>

namespace MoEnjin
{
	struct LabelDef 
	{
		LabelDef(const UTF8String& id, mo_int32 begin, mo_int32 end)
			: id(id), begin(begin), end(end) { }

		const UTF8String& id;
		mo_int32 begin;
		mo_int32 end;	
	};

	#include "Parser/ParserMacros.h"

	Parser::Parser(GlobalContext* globalContext, const SourceCode& source, const UTF8StringCollection& parameters, ParserStrictness strictness, ParserMode mode)
		: mGlobalContext(globalContext)
		, mSource(&source)
		, mHasError(false)
		, mError("Parse Error")
		, mAllowsIn(true)
		, mLastLine(0)
		, mLastTokenBegin(0)
		, mLastTokenEnd(0)
		, mAssignmentCount(0)
		, mNonLHSCount(0)
		, mNonTrivialExpressionCount(0)
		, mStatementDepth(0)
		, mHasSyntaxBeenValidated(true)
		, mLastIdentifier(0)
		, mStatements(0)
	{
		mLexer = new Lexer(globalContext, source);

		ParserScopeRef scope = PushScope();

		if(mode == ParserModeFunction)
			scope->SetIsFunction();
		if(strictness == ParserStrictnessStrict)
			scope->SetStrictMode();

		if(!parameters.empty())
		{
			for(mo_int32 i = 0; i < parameters.size(); ++i)
				scope->DeclareParameter(parameters.at(i));
		}

		Next();
		mLexer->SetLastLineNumber(GetTokenLine());
	}

	UTF8String Parser::GetToken()
	{
		return mSource->ToString(GetTokenBegin(), GetTokenEnd());
	}

	UTF8String Parser::Parse()
	{
		UTF8String error = UTF8String::MakeNull();
		ASTContext context(const_cast<GlobalContext*>(mGlobalContext), const_cast<SourceCode*>(mSource), mLexer);

		if(mLexer->IsReparsing())
			mStatementDepth--;

		ParserScopeRef scope = GetCurrentScope();
		StatementList* statements = ParseStatements(context, true);

		if(!statements || !Consume(TOK_EOF))
			error = mError;

		IdentifierSet capturedVariables;
		CodeFeatures features = context.GetFeatures();

		scope->GetCapturedVariables(capturedVariables);

		if(scope->GetStrictMode())
			features |= CodeFeaturesStrictMode;
		if(scope->GetShadowsArguments())
			features |= CodeFeaturesShadowsArguments;

		mStatements = statements;
		mVariableDeclarations = context.GetVariableDeclarations();
		mFunctions = context.GetFunctions();
		mFeatures = features;
		mConstantCount = context.GetConstantCount();
		mCapturedVariables.swap(capturedVariables);

		return error;
	}

	StatementList* Parser::ParseStatements(ASTContext& context, mo_bool checkForStrictMode)
	{
		const mo_unsigned useStrictLiteralLength = 12; // len("use strict") == 12
		const UTF8String* directive = mo_null;
		mo_bool hasDirective = mo_false;
		mo_bool hasSetStrict = mo_false;
		mo_unsigned directiveLiteralLength = 0;
		mo_unsigned start = mToken.begin;
		mo_unsigned oldLastLineNumber = mLexer->GetLastLineNumber();
		mo_unsigned oldLineNumber = mLexer->GetLineNumber();
		StatementList* statements = context.CreateStatementList();

		while(Statement* statement = ParseStatement(context, directive, &directiveLiteralLength))
		{
			if(checkForStrictMode && !hasDirective)
			{
				if(directive)
				{
					if(!hasSetStrict && directiveLiteralLength == useStrictLiteralLength && KnownIdentifiers::UseStrict == *directive)
					{
						SetStrictMode();
						hasSetStrict = true;

						FailIfFalse(IsStrictModeValid());

						mLexer->SetPosition(start);
						Next();
						mLexer->SetLastLineNumber(oldLastLineNumber);
						mLexer->SetLineNumber(oldLineNumber);

						FailIfTrue(mHasError);
						continue;
					}
				}
				else
				{
					hasDirective = true;
				}
			}

			statements->Append(statement);
		}

		if(mHasError)
			Fail();

		return statements;
	}

	Statement* Parser::ParseStatement(ASTContext& context, const UTF8String*& outDirective, mo_unsigned* outDirectiveLiteralLength)
	{
		mo_int32 nonTrivialExpressionCount = 0;
		DepthManager depth(&mStatementDepth);
		mStatementDepth++;
		outDirective = mo_null;

		FailIfStackOverflow();

		switch(mToken.type)
		{
			case TOK_OPEN_BRACE:
				return ParseBlock(context);
			case TOK_VAR:
				return ParseVarDeclaration(context);
			case TOK_CONST:
				return ParseConstDeclaration(context);
			case TOK_FUNCTION:
				FailIfFalseIfStrictWithMessage(mStatementDepth == 1, "Functions cannot be declared in a nested block in strict mode");
				return ParseFunctionDeclaration(context);
			case TOK_SEMICOLON:
				Next();
				return context.CreateEmpty();
			case TOK_IF:
				return ParseIf(context);
			case TOK_DO:
				return ParseDoWhile(context);
			case TOK_WHILE:
				return ParseWhile(context);
			case TOK_FOR:
				return ParseFor(context);
			case TOK_CONTINUE:
				return ParseContinue(context);
			case TOK_BREAK:
				return ParseBreak(context);
			case TOK_RETURN:
				return ParseReturn(context);
			case TOK_WITH:
				return ParseWith(context);
			case TOK_SWITCH:
				return ParseSwitch(context);
			case TOK_THROW:
				return ParseThrow(context);
			case TOK_TRY:
				return ParseTry(context);
			case TOK_DEBUGGER:
				return ParseDebugger(context);

			// these tokens implicitly mark the end of a set of statements
			case TOK_EOF:
			case TOK_CASE:
			case TOK_CLOSE_BRACE:
			case TOK_DEFAULT:
				return mo_null;

			case TOK_IDENTIFIER:
				return ParseExpressionOrLabel(context);
			case TOK_STRING:
				outDirective = mToken.data.id;

				if(outDirectiveLiteralLength > 0)
					*outDirectiveLiteralLength = mToken.end - mToken.begin;

				nonTrivialExpressionCount = mNonTrivialExpressionCount;
			default:
				Statement* expr = ParseExpressionStatement(context);

				if(outDirective && nonTrivialExpressionCount != mNonTrivialExpressionCount)
					outDirective = mo_null;

				return expr;
		}
	}

	Statement* Parser::ParseFunctionDeclaration(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_FUNCTION));

		const UTF8String* name = mo_null;
		ParameterNode* parameters = mo_null;
		FunctionNode* function = mo_null;
		mo_int32 openBracePosition = 0;
		mo_int32 closeBracePosition = 0;
		mo_int32 bodyBeginLine = 0;

		Next();

		FailIfFalse((ParseFunction(context, true, name, parameters, function, openBracePosition, closeBracePosition, bodyBeginLine)));
		FailIfNull(name);
		FailIfFalseIfStrict(DeclareVariable(*name));

		return context.CreateFunctionDeclaration(*name, function, parameters, openBracePosition, closeBracePosition, bodyBeginLine, mLastLine);
	}

	Statement* Parser::ParseVarDeclaration(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_VAR));

		mo_int32 begin = GetTokenLine();
		mo_int32 end = 0;
		mo_int32 decls = 0;
		mo_int32 n = 0;
		const UTF8String* id = mo_null;
		Expression* initializer = mo_null;
		Expression* varDecls = ParseVarDeclarationList(context, decls, id, initializer, n, n, n);

		FailIfError();
		FailIfFalse(InsertSemicolon());

		return context.CreateVar(varDecls, begin, end);
	}

	Statement* Parser::ParseConstDeclaration(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_CONST));

		mo_int32 begin = GetTokenLine();
		mo_int32 end = 0;
		ConstDeclExpression* constDecls = ParseConstDeclarationList(context);

		FailIfError();
		FailIfFalse(InsertSemicolon());

		return context.CreateConst(constDecls, begin, end);
	}

	Statement* Parser::ParseDoWhile(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_DO));

		mo_int32 begin = GetTokenLine();
		mo_int32 end = 0;
		const UTF8String* unused = mo_null;
		Statement* statement = mo_null;
		Expression* expr = mo_null;

		Next();
		BeginLoop();
		statement = ParseStatement(context, unused);
		EndLoop();

		FailIfNull(statement);

		end = GetTokenLine();

		ConsumeOrFail(TOK_WHILE);
		ConsumeOrFail(TOK_OPEN_PAREN);

		expr = ParseExpression(context);

		FailIfNull(expr);

		ConsumeOrFail(TOK_CLOSE_PAREN);

		if(Match(TOK_SEMICOLON))
			Next(); // will always perform automatic semicolon insertion

		return context.CreateDoWhile(expr, statement, begin, end);
	}

	Statement* Parser::ParseWhile(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_WHILE));

		mo_int32 begin = GetTokenLine();
		mo_int32 end = 0;
		const UTF8String* unused = mo_null;
		Statement* statement = mo_null;
		Expression* expr = mo_null;

		Next();
		ConsumeOrFail(TOK_OPEN_PAREN);

		expr = ParseExpression(context);
		FailIfNull(expr);

		end = GetTokenLine();
		ConsumeOrFail(TOK_CLOSE_PAREN);

		BeginLoop();
		statement = ParseStatement(context, unused);
		EndLoop();

		FailIfNull(statement);

		return context.CreateWhile(expr, statement, begin, end);
	}

	Statement* Parser::ParseFor(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_FOR));

		mo_bool hasDecl = mo_false;
		mo_int32 begin = GetTokenLine();
		mo_int32 end = 0;
		mo_int32 nonLHSCount = mNonLHSCount;
		mo_int32 declsCount = 0;
		mo_int32 declsBegin = 0;
		mo_int32 declsEnd = 0;
		mo_int32 exprEnd = 0;
		Expression* decls = mo_null;
		Expression* expr = mo_null;
		Statement* statement = mo_null;
		const UTF8String* unused = mo_null;

		Next();
		ConsumeOrFail(TOK_OPEN_PAREN);

		if(Match(TOK_VAR))
		{
			/*
				for(var <ID> in <EXPRESSION>) <STATEMENT>
				for(var <ID> = <EXPRESSION> in <EXPRESSION>) <STATEMENT>
				for(var <VAR-DECL-LIST>; <EXPRESSION-opt>; <EXPRESSION-opt>)
			*/

			const UTF8String* forInTarget = mo_null;
			Expression* forInInitializer = mo_null;
			mo_int32 initBegin = 0;
			mo_int32 initEnd = 0;

			mAllowsIn = mo_false;
			hasDecl = mo_true;
			decls = ParseVarDeclarationList(context, declsCount, forInTarget, forInInitializer, declsBegin, initBegin, initEnd);
			mAllowsIn = mo_true;

			FailIfError();

			if(Match(TOK_SEMICOLON))
				goto STANDARD_FOR_LOOP;

			FailIfFalse(declsCount == 1);

			// handle for-in with var declaration
			mo_int32 inLocation = GetTokenBegin();

			ConsumeOrFail(TOK_IN);

			expr = ParseExpression(context);
			FailIfNull(expr);

			exprEnd = GetLastTokenEnd();
			end = GetTokenLine();
			ConsumeOrFail(TOK_CLOSE_PAREN);

			BeginLoop();
			statement = ParseStatement(context, unused);
			EndLoop();
			FailIfNull(statement);

			return context.CreateForInLoop(*forInTarget, forInInitializer, expr, statement, declsBegin, inLocation, exprEnd, initBegin, initEnd, begin, end);
		}

		if(!Match(TOK_SEMICOLON))
		{
			mAllowsIn = mo_false;
			declsBegin = GetTokenBegin();
			decls = ParseExpression(context);
			declsEnd = GetLastTokenEnd();
			mAllowsIn = mo_true;

			FailIfNull(decls);
		}

		if(Match(TOK_SEMICOLON))
		{
		// standard for loop:
		//    for( <INITIALIZER>; <CONDITION>; <ITERATOR>; )
		//    for( <EXPRESSION-NO-IN-opt>; <EXPRESSION-opt>; <EXPRESSION-opt> )
		//
		STANDARD_FOR_LOOP:
			Expression* condition = mo_null;
			Expression* increment = mo_null;

			Next();
			
			if(!Match(TOK_SEMICOLON))
			{
				condition = ParseExpression(context);
				FailIfNull(condition);
			}

			ConsumeOrFail(TOK_SEMICOLON);

			if(!Match(TOK_CLOSE_PAREN))
			{
				increment = ParseExpression(context);
				FailIfNull(increment);
			}

			end = GetTokenLine();
			ConsumeOrFail(TOK_CLOSE_PAREN);

			BeginLoop();
			statement = ParseStatement(context, unused);
			EndLoop();
			FailIfNull(statement);

			return context.CreateForLoop(decls, condition, increment, statement, hasDecl, begin, end);
		}

		FailIfFalse(nonLHSCount == mNonLHSCount);
		ConsumeOrFail(TOK_IN);
		
		expr = ParseExpression(context);
		FailIfNull(expr);

		exprEnd = GetLastTokenEnd();
		end = GetTokenLine();

		ConsumeOrFail(TOK_CLOSE_PAREN);

		BeginLoop();
		statement = ParseStatement(context, unused);
		EndLoop();
		FailIfNull(statement);

		return context.CreateForInLoop(decls, expr, statement, declsBegin, declsEnd, exprEnd, begin, end);
	}

	Statement* Parser::ParseBreak(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_BREAK));

		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 endColumn = GetTokenEnd();
		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = GetTokenLine();
		const UTF8String* id = mo_null;

		Next();

		if(InsertSemicolon())
		{
			FailIfFalseWithMessage(CanBreak(), "The 'break' keyword can only be used inside a switch or loop statement");
			return context.CreateBreak(beginColumn, endColumn, beginLine, endLine);
		}

		MatchOrFail(TOK_IDENTIFIER);
		id = mToken.data.id;
		FailIfNullWithNameAndMessage(GetLabel(*id), "Label", id->Get(), "is not defined");

		endColumn = GetTokenEnd();
		endLine = GetTokenLine();

		Next();
		FailIfFalse(InsertSemicolon());

		return context.CreateBreak(*id, beginColumn, endColumn, beginLine, endLine);
	}

	Statement* Parser::ParseContinue(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_CONTINUE));

		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 endColumn = GetTokenEnd();
		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = GetTokenLine();
		const UTF8String* id = mo_null;

		Next();

		if(InsertSemicolon())
		{
			FailIfFalseWithMessage(CanContinue(), "The 'continue' keyword can only be used inside a loop statement");
			return context.CreateContinue(beginColumn, endColumn, beginLine, endLine);
		}

		MatchOrFail(TOK_IDENTIFIER);
		id = mToken.data.id;
		ParserScope::LabelInfo* label = GetLabel(*id);

		FailIfNullWithNameAndMessage(label, "Label", id->Get(), "is not defined");
		FailIfFalseWithMessage(label->isLoop, "The 'continue' keyword can only be used inside a loop statement");

		endColumn = GetTokenEnd();
		endLine = GetTokenLine();

		Next();
		FailIfFalse(InsertSemicolon());

		return context.CreateContinue(*id, beginColumn, endColumn, beginLine, endLine);
	}

	Statement* Parser::ParseReturn(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_RETURN));

		ParserScopeRef scope = GetCurrentScope();
		FailIfFalse(scope->IsFunction());

		Expression* expr = mo_null;
		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 endColumn = GetTokenEnd();
		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = GetTokenLine();

		Next();

		// need to check for automatic semicolon before trying to parse an expression
		// so that we are sure a line break after the return will correctly terminate
		// the statement
		if(Match(TOK_SEMICOLON))
			endLine = GetTokenLine();

		if(InsertSemicolon())
			return context.CreateReturn(mo_null, beginColumn, endColumn, beginLine, endLine);

		expr = ParseExpression(context);
		FailIfNull(expr);

		endColumn = GetLastTokenEnd();

		if(Match(TOK_SEMICOLON))
			endLine = GetTokenLine();

		FailIfFalse(InsertSemicolon());

		scope->SetFunctionDoesReturn();
		return context.CreateReturn(expr, beginColumn, endColumn, beginLine, endLine);
	}

	Statement* Parser::ParseThrow(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_THROW));

		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 endColumn = 0;
		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = 0;
		Expression* expr = mo_null;

		Next();
		FailIfTrue(InsertSemicolon());

		expr = ParseExpression(context);
		FailIfNull(expr);

		endColumn = GetLastTokenEnd();
		endLine = GetTokenLine();
		FailIfFalse(InsertSemicolon());

		return context.CreateThrow(expr, beginColumn, endColumn, beginLine, endLine);
	}

	Statement* Parser::ParseWith(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_WITH));
		
		FailIfTrueWithMessage(GetStrictMode(), "The 'with' keyword is not allowed while in strict mode");
		GetCurrentScope()->RequireFullActivations();

		mo_int32 beginColumn = 0;
		mo_int32 endColumn = 0;
		mo_int32 beginLine = 0;
		mo_int32 endLine = 0;
		Expression* expr = mo_null;
		Statement* statement = mo_null;
		const UTF8String* unused = mo_null;
		
		beginLine = GetTokenLine();
		
		Next();
		ConsumeOrFail(TOK_OPEN_PAREN);

		beginColumn = GetTokenBegin();
		expr = ParseExpression(context);
		FailIfNull(expr);

		endColumn = GetLastTokenEnd();
		endLine = GetTokenLine();
		
		ConsumeOrFail(TOK_CLOSE_PAREN);

		statement = ParseStatement(context, unused);
		FailIfNull(statement);

		return context.CreateWith(expr, statement, beginColumn, endColumn, beginLine, endLine);
	}

	Statement* Parser::ParseSwitch(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_SWITCH));

		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = 0;
		Expression* expr = mo_null;
		ClauseListNode* firstClauses = mo_null;
		ClauseListNode* secondClauses = mo_null;
		CaseClauseNode* defaultClause = mo_null;

		Next();
		ConsumeOrFail(TOK_OPEN_PAREN);

		expr = ParseExpression(context);
		FailIfNull(expr);

		endLine = GetTokenLine();

		ConsumeOrFail(TOK_CLOSE_PAREN);
		ConsumeOrFail(TOK_OPEN_BRACE);

		BeginSwitch();
		firstClauses = ParseSwitchClauses(context);
		FailIfError();

		defaultClause = ParseSwitchDefaultClause(context);
		FailIfError();

		secondClauses = ParseSwitchClauses(context);
		FailIfError();
		EndSwitch();

		ConsumeOrFail(TOK_CLOSE_BRACE);

		return context.CreateSwitch(expr, firstClauses, defaultClause, secondClauses, beginLine, endLine);
	}

	Statement* Parser::ParseTry(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_TRY));

		mo_int32 beginLine = 0;
		mo_int32 endLine = 0;
		Statement* tryBlock = mo_null;
		Statement* catchBlock = mo_null;
		Statement* finallyBlock = mo_null;
		const UTF8String* id = &KnownIdentifiers::Null;

		beginLine = GetTokenLine();

		Next();
		MatchOrFail(TOK_OPEN_BRACE);

		tryBlock = ParseBlock(context);
		FailIfNull(tryBlock);

		endLine = mLastLine;

		if(Match(TOK_CATCH))
		{
			GetCurrentScope()->RequireFullActivations();

			Next();
			ConsumeOrFail(TOK_OPEN_PAREN);

			MatchOrFail(TOK_IDENTIFIER);
			id = mToken.data.id;
			Next();

			AutoPopScopeRef catchScope(this, PushScope());
			FailIfFalseIfStrictWithNameAndMessage(DeclareVariable(*id), "Cannot declare the variable named", id->Get(), "in strict mode");
			
			catchScope->PreventNewDeclarations();
			ConsumeOrFail(TOK_CLOSE_PAREN);

			MatchOrFail(TOK_OPEN_BRACE);
			catchBlock = ParseBlock(context);

			FailIfNullWithMessage(catchBlock, "A 'try' must have a catch or finally block");
			
			PopScope(catchScope, true);
		}

		if(Match(TOK_FINALLY))
		{
			Next();
			MatchOrFail(TOK_OPEN_BRACE);

			finallyBlock = ParseBlock(context);
			FailIfNull(finallyBlock);
		}

		FailIfFalse(catchBlock || finallyBlock);

		return context.CreateTry(*id, tryBlock, catchBlock, finallyBlock, beginLine, endLine);
	}

	Statement* Parser::ParseDebugger(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_DEBUGGER));

		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = beginLine;

		Next();

		if(Match(TOK_SEMICOLON))
			beginLine = GetTokenLine();

		FailIfFalse(InsertSemicolon());

		return context.CreateDebugger(beginLine, endLine);
	}

	Statement* Parser::ParseIf(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_IF));

		mo_int32 beginLine = GetTokenLine();
		mo_int32 endLine = 0;
		mo_bool hasTrailingElse = mo_false;
		Expression* condition = mo_null;
		Statement* trueBlock = mo_null;
		const UTF8String* unused = mo_null;

		Next();
		ConsumeOrFail(TOK_OPEN_PAREN);

		condition = ParseExpression(context);
		FailIfNull(condition);

		endLine = GetTokenLine();
		ConsumeOrFail(TOK_CLOSE_PAREN);

		trueBlock = ParseStatement(context, unused);
		FailIfNull(trueBlock);

		if(!Match(TOK_ELSE))
			return context.CreateIf(condition, trueBlock, beginLine, endLine);

		std::vector<Expression*> expressions;
		std::vector<Statement*> statements;
		std::vector<std::pair<int, int> > positions;
		
		do 
		{
			Next();

			if(!Match(TOK_IF))
			{
				Statement* statement = ParseStatement(context, unused);
				FailIfNull(statement);

				statements.push_back(statement);
				hasTrailingElse = mo_true;
				break;;
			}

			mo_int32 innerBeginLine = GetTokenLine();
			mo_int32 innerEndLine = 0;
			Expression* innerCondition = mo_null;
			Statement* innerTrueBlock = mo_null;

			Next();
			ConsumeOrFail(TOK_OPEN_PAREN);

			innerCondition = ParseExpression(context);
			FailIfNull(innerCondition);

			innerEndLine = GetTokenLine();
			ConsumeOrFail(TOK_CLOSE_PAREN);

			innerTrueBlock = ParseStatement(context, unused);
			FailIfNull(innerTrueBlock);

			expressions.push_back(innerCondition);
			positions.push_back(std::make_pair(innerBeginLine, innerEndLine));
			statements.push_back(innerTrueBlock);
		} 
		while(Match(TOK_ELSE));

		if(!hasTrailingElse)
		{
			Expression* expr = expressions.back();
			expressions.pop_back();

			Statement* block = statements.back();
			statements.pop_back();

			std::pair<int, int> pos = positions.back();
			positions.pop_back();

			statements.push_back(context.CreateIf(expr, block, pos.first, pos.second));
		}

		while(!expressions.empty())
		{
			Expression* expr = expressions.back();
			expressions.pop_back();

			Statement* falseBlock = statements.back();
			statements.pop_back();

			Statement* trueBlock = statements.back();
			statements.pop_back();

			std::pair<int, int> pos = positions.back();
			positions.pop_back();

			statements.push_back(context.CreateIf(expr, trueBlock, falseBlock, pos.first, pos.second));
		}

		return context.CreateIf(condition, trueBlock, statements.back(), beginLine, endLine);
	}

	Statement* Parser::ParseExpressionStatement(ASTContext& context)
	{
		mo_int32 beginLine = GetTokenLine();
		Expression* expr = ParseExpression(context);

		FailIfNull(expr);
		FailIfFalse(InsertSemicolon());

		return context.CreateExpression(expr, beginLine, mLastLine);
	}

	Statement* Parser::ParseExpressionOrLabel(ASTContext& context)
	{
		std::vector<LabelDef> labels;

		do
		{
			mo_int32 beginColumn = GetTokenBegin();
			mo_int32 endColumn = 0;
			mo_int32 beginLine = GetTokenLine();
			const UTF8String* id = mo_null;

			if(!mLexer->IsNextTokenColon())
			{
				Expression* expr = ParseExpression(context);
				FailIfNull(expr);
				FailIfFalse(InsertSemicolon());

				return context.CreateExpression(expr, beginLine, mLastLine);
			}

			id = mToken.data.id;
			endColumn = GetTokenEnd();

			Next();
			ConsumeOrFail(TOK_COLON);

			if(!mHasSyntaxBeenValidated)
			{
				for(size_t i = 0; i < labels.size(); i++)
					FailIfTrue((id->Get() == labels[i].id.Get()));
				
				FailIfTrue(GetLabel(*id) != mo_null);
				labels.push_back(LabelDef(*id, beginColumn, endColumn));
			}
		} 
		while(Match(TOK_IDENTIFIER));

		mo_bool isLoop = mo_false;

		switch(mToken.type)
		{
			case TOK_FOR:
			case TOK_WHILE:
			case TOK_DO:
				isLoop = mo_true;
				break;
			default:
				break;
		}

		const UTF8String* unused = mo_null;

		if(!mHasSyntaxBeenValidated)
		{
			for(size_t i = 0; i < labels.size(); i++)
				PushLabel(labels[i].id, isLoop);
		}

		Statement* statement = ParseStatement(context, unused);

		if(!mHasSyntaxBeenValidated)
		{
			for(size_t i = 0; i < labels.size(); i++)
				PopLabel();
		}

		FailIfNull(statement);

		for(size_t i = 0; i < labels.size(); i++)
		{
			const LabelDef& info = labels[labels.size() - i - 1];
			statement = context.CreateLabel(info.id, statement, info.begin, info.end);
		}

		return statement;
	}

	Statement* Parser::ParseBlock(ASTContext& context)
	{
		MO_ASSERT(Match(TOK_OPEN_BRACE));

		mo_int32 beginLine = GetTokenLine();
		StatementList* statements = mo_null;

		Next();

		if(Match(TOK_CLOSE_BRACE))
		{
			Next();
			return context.CreateBlock(mo_null, beginLine, mLastLine);
		}

		statements = ParseStatements(context, mo_false);
		FailIfNull(statements);

		MatchOrFail(TOK_CLOSE_BRACE);
		Next();

		return context.CreateBlock(statements, beginLine, mLastLine);
	}

	Expression* Parser::ParseExpression(ASTContext& context)
	{
		FailIfStackOverflow();

		Expression* left = mo_null;
		Expression* right = mo_null;
		
		left = ParseAssignment(context);
		FailIfNull(left);

		if(!Match(TOK_COMMA))
			return left;

		Next();
		
		mNonTrivialExpressionCount++;
		mNonLHSCount++;

		right = ParseAssignment(context);
		FailIfNull(right);

		CommaExpression* commaExpr = context.CreateComma(left, right);

		while(Match(TOK_COMMA))
		{
			Next();

			right = ParseAssignment(context);
			FailIfNull(right);

			commaExpr->Append(right);
		}

		return commaExpr;
	}

	Expression* Parser::ParseAssignment(ASTContext& context)
	{
		FailIfStackOverflow();

		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 initialAssignmentCount = mAssignmentCount;
		mo_int32 initialNonLHSCount = mNonLHSCount;
		mo_int32 assignmentDepth = 0;
		mo_bool hasAssignment = mo_false;
		Operator op;
		Expression* lhs = ParseConditional(context);
		FailIfNull(lhs);

		if(initialNonLHSCount != mNonLHSCount)
			return lhs;

		while(true)
		{
			switch(mToken.type)
			{
				case TOK_EQUAL:
					op = OpEqual;
					break;
				case TOK_PLUS_EQUAL:
					op = OpPlusEqual;
					break;
				case TOK_MINUS_EQUAL:
					op = OpMinusEqual;
					break;
				case TOK_MULT_EQUAL:
					op = OpMultiplyEqual;
					break;
				case TOK_DIV_EQUAL:
					op = OpDivideEqual;
					break;
				case TOK_LSHIFT_EQUAL:
					op = OpLShiftEqual;
					break;
				case TOK_RSHIFT_EQUAL:
					op = OpRShiftEqual;
					break;
				case TOK_URSHIFT_EQUAL:
					op = OpRShiftUnsignedEqual;
					break;
				case TOK_AND_EQUAL:
					op = OpAndEqual;
					break;
				case TOK_OR_EQUAL:
					op = OpOrEqual;
					break;
				case TOK_XOR_EQUAL:
					op = OpXOrEqual;
					break;
				case TOK_MOD_EQUAL:
					op = OpModEqual;
					break;
				default:
					goto END;
			}

			mNonTrivialExpressionCount++;
			hasAssignment = mo_true;

			context.AppendAssignment(assignmentDepth, mAssignmentCount, lhs, op, beginColumn, GetTokenBegin());

			beginColumn = GetTokenBegin();
			mAssignmentCount++;

			Next();

			if(GetStrictMode() && mLastIdentifier != mo_null && lhs->GetType() == NODE_RESOLVE)
			{
				FailIfTrueIfStrictWithMessage(KnownIdentifiers::eval == *mLastIdentifier, "'eval' cannot be modified in strict mode");
				FailIfTrueIfStrictWithMessage(KnownIdentifiers::arguments == *mLastIdentifier, "'arguments' cannot be modified in strict mode");

				DeclareWrite(*mLastIdentifier);
				mLastIdentifier = mo_null;
			}

			lhs = ParseConditional(context);
			FailIfNull(lhs);

			if(initialNonLHSCount != mNonLHSCount)
				break;
		}

	END:
		if(hasAssignment)
			mNonLHSCount++;

		while(assignmentDepth > 0)
			lhs = context.CreateAssignment(assignmentDepth, lhs, initialAssignmentCount, mAssignmentCount, GetLastTokenEnd());

		return lhs;
	}

	Expression* Parser::ParseConditional(ASTContext& context)
	{
		Expression* lhs = mo_null;
		Expression* rhs = mo_null;
		Expression* condition = ParseBinary(context);
		FailIfNull(condition);

		if(!Match(TOK_QUESTION))
			return condition;

		mNonTrivialExpressionCount++;
		mNonLHSCount++;

		Next();

		lhs = ParseAssignment(context);
		ConsumeOrFail(TOK_COLON);

		rhs = ParseAssignment(context);
		FailIfNull(rhs);

		return context.CreateConditional(condition, lhs, rhs);
	}

	Expression* Parser::ParseBinary(ASTContext& context)
	{
		mo_int32 operandDepth = 0;
		mo_int32 operatorDepth = 0;

		while(true)
		{
			mo_int32 beginColumn = GetTokenBegin();
			mo_int32 initialAssignmentCount = mAssignmentCount;
			mo_int32 precedence = 0;
			TokenType operatorToken = TOK_UNKNOWN;
			Expression* currentExpr = ParseUnary(context);
			FailIfNull(currentExpr);

			context.AppendBinaryExpressionInfo(operandDepth, currentExpr, beginColumn, GetLastTokenEnd(), GetLastTokenEnd(), initialAssignmentCount != mAssignmentCount);

			precedence = GetBinaryOperatorPrecedence(mToken.type);

			if(precedence == 0)
				break;

			operatorToken = mToken.type;

			mNonTrivialExpressionCount++;
			mNonLHSCount++;

			Next();

			while(operatorDepth > 0 && context.PrecedenceIsLowerThanOperatorStack(precedence))
			{
				MO_ASSERT(operandDepth > 1);
				context.FinishBinaryOperation(operandDepth, operatorDepth);
			}

			context.AppendBinaryOperator(operatorDepth, (mo_int32)operatorToken, precedence);
		}

		while(operatorDepth > 0)
		{
			MO_ASSERT(operandDepth > 1);
			context.FinishBinaryOperation(operandDepth, operatorDepth);
		}

		return context.PopBinaryOperand();
	}

	Expression* Parser::ParseUnary(ASTContext& context)
	{
		AllowInOverride inOverride(this);
		mo_int32 tokenStackDepth = 0;
		mo_int32 beginColumn = 0;
		mo_int32 endColumn = 0;
		mo_bool modifiesExpression = mo_false;
		mo_bool requiresLExpr = mo_false;
		mo_bool isEvalOrArguments = mo_false;
		Expression* expr = mo_null;

		while(IsUnaryOp(mToken.type))
		{
			if(GetStrictMode())
			{
				switch(mToken.type)
				{
					case TOK_PLUSPLUS:
					case TOK_MINUSMINUS:
					case TOK_AUTO_PLUSPLUS:
					case TOK_AUTO_MINUSMINUS:
						FailIfTrue(requiresLExpr);
						modifiesExpression = mo_true;
						requiresLExpr = mo_true;
						break;
					case TOK_DELETE:
						FailIfTrue(requiresLExpr);
						requiresLExpr = mo_true;
						break;
					default:
						FailIfTrue(requiresLExpr);
						break;
				}
			}

			context.AppendUnaryToken(tokenStackDepth, mToken.type, GetTokenBegin());

			mNonLHSCount++;
			mNonTrivialExpressionCount++;

			Next();
		}

		beginColumn = GetTokenBegin();

		expr = ParseMember(context);
		FailIfNull(expr);

		isEvalOrArguments = mo_false;

		if(GetStrictMode() && !mHasSyntaxBeenValidated)
		{
			if(expr->GetType() == NODE_RESOLVE)
				isEvalOrArguments = (*mLastIdentifier == KnownIdentifiers::eval || *mLastIdentifier == KnownIdentifiers::arguments);
		}

		FailIfTrueIfStrictWithNameAndMessage(isEvalOrArguments && modifiesExpression, "'", mLastIdentifier->Get(), "' cannot be modified in strict mode");

		switch(mToken.type)
		{
			case TOK_PLUSPLUS:
			case TOK_MINUSMINUS:
				mNonTrivialExpressionCount++;
				mNonLHSCount++;
				expr = context.CreatePostfix(expr, (mToken.type == TOK_PLUSPLUS ? OpPlusPlus : OpMinusMinus), beginColumn, GetLastTokenEnd(), GetTokenEnd());
				mAssignmentCount++;

				FailIfTrueIfStrictWithNameAndMessage(isEvalOrArguments, "'", mLastIdentifier->Get(), "' cannot be modified in strict mode");
				FailIfTrueIfStrict(requiresLExpr);
				Next();
				break;
			default:
				break;
		}

		endColumn = GetLastTokenEnd();

		while(tokenStackDepth > 0)
		{
			switch(context.GetLastUnaryTokenType())
			{
				case TOK_EXCLAMATION:
					expr = context.CreateLogicalNot(expr);
					break;
				case TOK_TILDE:
					expr = context.CreateBitwiseNot(expr);
					break;
				case TOK_MINUS:
					expr = context.CreateNegate(expr);
					break;
				case TOK_PLUS:
					expr = context.CreateUnaryPlus(expr);
					break;
				case TOK_PLUSPLUS:
				case TOK_AUTO_PLUSPLUS:
					expr = context.CreatePrefix(expr, OpPlusPlus, context.GetLastUnaryTokenStart(), beginColumn + 1, endColumn);
					mAssignmentCount++;
					break;
				case TOK_MINUSMINUS:
				case TOK_AUTO_MINUSMINUS:
					expr = context.CreatePrefix(expr, OpMinusMinus, context.GetLastUnaryTokenStart(), beginColumn + 1, endColumn);
					mAssignmentCount++;
					break;
				case TOK_TYPEOF:
					expr = context.CreateTypeOf(expr);
					break;
				case TOK_VOID:
					expr = context.CreateVoid(expr);
					break;
				case TOK_DELETE:
					FailIfTrueIfStrictWithNameAndMessage((expr->GetType() == NODE_RESOLVE), "Cannot delete unqualified property", mLastIdentifier->Get(), "in strict mode");
					expr = context.CreateDelete(expr, context.GetLastUnaryTokenStart(), endColumn, endColumn);
					break;
				default:
					MO_ASSERT_NOT_REACHED();
					break;
			}

			beginColumn = context.GetLastUnaryTokenStart();
			context.RemoveLastUnaryToken(tokenStackDepth);
		}

		return expr;
	}

	Expression* Parser::ParseMember(ASTContext& context)
	{
		Expression* base = mo_null;
		mo_int32 beginColumn = GetTokenBegin();
		mo_int32 endColumn = 0;
		mo_int32 newCount = 0;
		mo_int32 nonLHSCount = 0;
		mo_int32 initialAssignmentCount = 0;

		while(Match(TOK_NEW))
		{
			Next();
			newCount++;
		}

		if(Match(TOK_FUNCTION))
		{
			const UTF8String* name = &KnownIdentifiers::Null;
			ParameterNode* parameters = mo_null;
			FunctionNode* function = mo_null;
			mo_int32 openBrace = 0;
			mo_int32 closeBrace = 0;
			mo_int32 bodyBeginLine = 0;

			Next();
			FailIfFalse((ParseFunction(context, false, name, parameters, function, openBrace, closeBrace, bodyBeginLine)));

			base = context.CreateFunction(*name, function, parameters, openBrace, closeBrace, bodyBeginLine, mLastLine);
		}
		else
		{
			base = ParsePrimary(context);
		}

		FailIfNull(base);

		while(true)
		{
			switch(mToken.type)
			{
				case TOK_OPEN_BRACKET:
				{
					mNonTrivialExpressionCount++;

					endColumn = GetLastTokenEnd();
					nonLHSCount = mNonLHSCount;
					initialAssignmentCount = mAssignmentCount;

					Next();

					Expression* prop = ParseExpression(context);
					FailIfNull(prop);

					base = context.CreateBracketAccess(base, prop, initialAssignmentCount != mAssignmentCount, beginColumn, endColumn, GetTokenEnd());
					ConsumeOrFail(TOK_CLOSE_BRACKET);
					mNonLHSCount = nonLHSCount;
					break;
				}
				case TOK_OPEN_PAREN:
				{
					mNonTrivialExpressionCount++;
					nonLHSCount = mNonLHSCount;

					if(newCount > 0)
					{
						newCount--;
						endColumn = GetLastTokenEnd();

						ArgumentsNode* arguments = ParseArguments(context);
						FailIfNull(arguments);

						base = context.CreateNew(base, arguments, beginColumn, endColumn, GetLastTokenEnd());
					}
					else
					{
						endColumn = GetLastTokenEnd();

						ArgumentsNode* arguments = ParseArguments(context);
						FailIfNull(arguments);
						base = context.CreateFunctionCall(base, arguments, beginColumn, endColumn, GetLastTokenEnd());
					}

					mNonLHSCount = nonLHSCount;
					break;
				}
				case TOK_DOT:
				{
					endColumn = GetLastTokenEnd();
					mNonTrivialExpressionCount++;

					NextIdentifier();
					MatchOrFail(TOK_IDENTIFIER);
					
					base = context.CreateDotAccess(base, *mToken.data.id, beginColumn, endColumn, GetTokenEnd());
					Next();
					break;
				}
				default:
					goto END_MEMBER_EXPRESSION;
			}
		}

	END_MEMBER_EXPRESSION:
		while(newCount-- > 0)
			base = context.CreateNew(base, beginColumn, GetLastTokenEnd());

		return base;
	}

	Expression* Parser::ParsePrimary(ASTContext& context)
	{
		FailIfStackOverflow();

		switch(mToken.type)
		{
			case TOK_OPEN_BRACE:
				if(GetStrictMode())
					return ParseObjectLiteralStrict(context);

				return ParseObjectLiteral(context);
			case TOK_OPEN_BRACKET:
				return ParseArrayLiteral(context);
			case TOK_OPEN_PAREN:
			{
				mo_int32 nonLHSCount = mNonLHSCount;
				Expression* expr = mo_null;

				Next();

				expr = ParseExpression(context);
				mNonLHSCount = nonLHSCount;
				ConsumeOrFail(TOK_CLOSE_PAREN);

				return expr;
			}
			case TOK_THIS:
			{
				Next();
				return context.CreateThis();
			}
			case TOK_IDENTIFIER:
			{
				mo_int32 beginColumn = GetTokenBegin();
				const UTF8String* id = mToken.data.id;

				Next();
				GetCurrentScope()->UseVariable(*id, (*id == KnownIdentifiers::eval));
				mLastIdentifier = id;

				return context.CreateResolve(*id, beginColumn);
			}
			case TOK_STRING:
			{
				const UTF8String* id = mToken.data.id;
				Next();

				return context.CreateString(*id);
			}
			case TOK_NUMBER:
			{
				mo_double value = mToken.data.doubleValue;
				Next();

				return context.CreateNumber(value);
			}
			case TOK_NULL:
			{
				Next();
				return context.CreateNull();
			}
			case TOK_TRUE:
			{
				Next();
				return context.CreateBoolean(mo_true);
			}
			case TOK_FALSE:
			{
				Next();
				return context.CreateBoolean(mo_false);
			}
			case TOK_DIV_EQUAL:
			case TOK_DIVIDE:
			{
				// regular expression
				const UTF8String* pattern = mo_null;
				const UTF8String* flags = mo_null;

				if(Match(TOK_DIV_EQUAL))
					FailIfFalse(mLexer->ScanRegEx(pattern, flags, '='));
				else
					FailIfFalse(mLexer->ScanRegEx(pattern, flags));

				mo_int32 beginColumn = GetTokenBegin();
				Expression* expr = mo_null;

				Next();
				expr = context.CreateRegExp(*pattern, *flags, beginColumn);

				if(!expr)
				{
					// TODO : check syntax of pattern
					MO_ASSERT_NOT_REACHED();
				}

				return expr;
			}
			default:
				Fail();
		}
	}

	Expression* Parser::ParseArrayLiteral(ASTContext& context)
	{
		ConsumeOrFail(TOK_OPEN_BRACKET);

		mo_int32 nonLHSCount = mNonLHSCount;
		mo_int32 elisions = 0;
		Expression* elem = mo_null;

		while(Match(TOK_COMMA))
		{
			Next();
			elisions++;
		}

		if(Match(TOK_CLOSE_BRACKET))
		{
			Next();
			return context.CreateArray(elisions);
		}

		elem = ParseAssignment(context);
		FailIfNull(elem);

		Element* elements = context.CreateElementList(elem, elisions);
		Element* next = elements;

		elisions = 0;

		while(Match(TOK_COMMA))
		{
			Next();

			elisions = 0;

			while(Match(TOK_COMMA))
			{
				Next();
				elisions++;
			}

			if(Match(TOK_CLOSE_BRACKET))
			{
				Next();
				return context.CreateArray(elements, elisions);
			}

			elem = ParseAssignment(context);
			FailIfNull(elem);
			next = context.CreateElementList(elem, elisions, next);
		}

		ConsumeOrFail(TOK_CLOSE_BRACKET);
		mNonLHSCount = nonLHSCount;

		return context.CreateArray(elements);
	}

	Expression* Parser::ParseObjectLiteral(ASTContext& context)
	{
		mo_int32 beginOffset = mToken.data.intValue;
		mo_int32 nonLHSCount = 0;
		mo_unsigned lastLastLineNumber = mLexer->GetLastLineNumber();
		mo_unsigned lastLineNumber = mLexer->GetLineNumber();

		ConsumeOrFail(TOK_OPEN_BRACE);

		nonLHSCount = mNonLHSCount;

		if(Match(TOK_CLOSE_BRACE))
		{
			Next();
			return context.CreateObjectLiteral();
		}

		PropertyNode* prop = ParseProperty(context, mo_false);
		FailIfNull(prop);

		if(!mHasSyntaxBeenValidated && prop->GetType() != PropertyTypeConstant)
		{
			mLexer->SetPosition(beginOffset);
			Next();
			mLexer->SetLastLineNumber(lastLastLineNumber);
			mLexer->SetLineNumber(lastLineNumber);

			return ParseObjectLiteralStrict(context);
		}

		PropertyListNode* properties = context.CreatePropertyList(prop);
		PropertyListNode* next = properties;

		while(Match(TOK_COMMA))
		{
			Next();

			if(Match(TOK_CLOSE_BRACE))
				break;

			prop = ParseProperty(context, mo_false);
			FailIfNull(prop);

			if(!mHasSyntaxBeenValidated && prop->GetType() != PropertyTypeConstant)
			{
				mLexer->SetPosition(beginOffset);
				Next();
				mLexer->SetLastLineNumber(lastLastLineNumber);
				mLexer->SetLineNumber(lastLineNumber);
				
				return ParseObjectLiteralStrict(context);
			}

			next = context.CreatePropertyList(prop, next);
		}

		ConsumeOrFail(TOK_CLOSE_BRACE);

		mNonLHSCount = nonLHSCount;

		return context.CreateObjectLiteral(properties);
	}

	Expression* Parser::ParseObjectLiteralStrict(ASTContext& context)
	{
		ConsumeOrFail(TOK_OPEN_BRACE);

		mo_int32 nonLHSCount = mNonLHSCount;

		if(Match(TOK_CLOSE_BRACE))
		{
			Next();
			return context.CreateObjectLiteral();
		}
		
		PropertyNode* prop = ParseProperty(context, mo_true);
		FailIfNull(prop);

		typedef Poco::HashMap<const UTF8String*, mo_unsigned, UTF8String::Hasher> ObjectValidationMap;
		ObjectValidationMap objectValidator;

		if(!mHasSyntaxBeenValidated)
			objectValidator.insert(std::make_pair(&prop->GetName(), prop->GetType()));

		PropertyListNode* properties = context.CreatePropertyList(prop);
		PropertyListNode* next = properties;

		while(Match(TOK_COMMA))
		{
			Next();

			if(Match(TOK_CLOSE_BRACE))
				break;

			prop = ParseProperty(context, mo_true);
			FailIfNull(prop);

			if(!mHasSyntaxBeenValidated)
			{
				std::pair<ObjectValidationMap::Iterator, mo_bool> propEntry = objectValidator.insert(std::make_pair(&prop->GetName(), prop->GetType()));

				if(!propEntry.second)
				{
					ObjectValidationMap::Iterator it = propEntry.first;
					FailIfTrue(it->second == PropertyTypeConstant);
					FailIfTrue(prop->GetType() == PropertyTypeConstant);
					FailIfTrue(prop->GetType() & it->second);

					it->second |= prop->GetType();
				}
			}

			next = context.CreatePropertyList(prop, next);
		}

		ConsumeOrFail(TOK_CLOSE_BRACE);
		mNonLHSCount = nonLHSCount;

		return context.CreateObjectLiteral(properties);
	}

	Expression* Parser::ParseVarDeclarationList(ASTContext& context, mo_int32& declarations, const UTF8String*& lastId, Expression*& lastInitializer, mo_int32& idBegin, mo_int32& initBegin, mo_int32& initEnd)
	{
		Expression* varDecls = mo_null;

		do
		{
			mo_int32 varBegin = 0;
			mo_int32 varDivot = 0;
			mo_int32 initialAssignmentCount = 0;
			mo_bool hasInitializer = mo_false;
			const UTF8String* name = mo_null;
			Expression* initializer = mo_null;
			Expression* node = mo_null;

			declarations++;
			Next();
			MatchOrFail(TOK_IDENTIFIER);
			
			varBegin = GetTokenBegin();
			idBegin = varBegin;
			name = mToken.data.id;
			lastId = name;

			Next();

			hasInitializer = Match(TOK_EQUAL);

			FailIfFalseIfStrictWithNameAndMessage(DeclareVariable(*name), "Cannot declare a variable named", name->Get(), "in strict mode");
			context.AddVariable(*name, (hasInitializer || (!mAllowsIn && Match(TOK_IN))) ? VarAttributeHasInitializer : VarAttributeNone);

			if(hasInitializer)
			{
				varDivot = GetTokenBegin() + 1;
				initBegin = GetTokenBegin();

				Next();
				initialAssignmentCount = mAssignmentCount;

				initializer = ParseAssignment(context);
				initEnd = GetLastTokenEnd();
				lastInitializer = initializer;
				FailIfNull(initializer);

				node = context.CreateAssignResolve(*name, initializer, initialAssignmentCount != mAssignmentCount, varBegin, varDivot, GetLastTokenEnd());

				if(varDecls == mo_null)
					varDecls = node;
				else
					varDecls = context.CombineCommaExpressions(varDecls, node);
			}
		}
		while(Match(TOK_COMMA));

		return varDecls;
	}

	ConstDeclExpression* Parser::ParseConstDeclarationList(ASTContext& context)
	{
		FailIfTrue(GetStrictMode());

		ConstDeclExpression* decls = mo_null;
		ConstDeclExpression* next = mo_null;

		do
		{
			const UTF8String* name = mo_null;
			mo_bool hasInitializer = mo_false;
			Expression* initializer = mo_null;
			
			Next();
			MatchOrFail(TOK_IDENTIFIER);

			name = mToken.data.id;
			Next();

			hasInitializer = Match(TOK_EQUAL);
			DeclareVariable(*name);

			context.AddVariable(*name, VarAttributeIsConstant | (hasInitializer ? VarAttributeHasInitializer : VarAttributeNone));

			if(hasInitializer)
			{
				Next();
				initializer = ParseAssignment(context);
			}

			next = context.CreateConstDecl(*name, initializer, next);

			if(decls == mo_null)
				decls = next;
		}
		while(Match(TOK_COMMA));

		return decls;
	}

	ClauseListNode* Parser::ParseSwitchClauses(ASTContext& context)
	{
		if(!Match(TOK_CASE))
			return mo_null;

		Expression* condition = mo_null;
		StatementList* statements = mo_null;
		CaseClauseNode* clause = mo_null;
		ClauseListNode* clauses = mo_null;
		ClauseListNode* next = mo_null;

		Next();

		condition = ParseExpression(context);
		FailIfNull(condition);
		
		ConsumeOrFail(TOK_COLON);

		statements = ParseStatements(context, mo_false);
		FailIfNull(statements);

		clause = context.CreateClause(condition, statements);
		clauses = context.CreateClauseList(clause);
		next = clauses;

		while(Match(TOK_CASE))
		{
			Next();

			condition = ParseExpression(context);
			FailIfNull(condition);

			ConsumeOrFail(TOK_COLON);

			statements = ParseStatements(context, mo_false);
			FailIfNull(statements);

			clause = context.CreateClause(condition, statements);
			next = context.CreateClauseList(clause, next);
		}

		return clauses;
	}

	CaseClauseNode* Parser::ParseSwitchDefaultClause(ASTContext& context)
	{
		if(!Match(TOK_DEFAULT))
			return mo_null;

		Next();
		ConsumeOrFail(TOK_COLON);

		StatementList* statements = ParseStatements(context, mo_false);
		FailIfNull(statements);

		return context.CreateClause(mo_null, statements);
	}

	ArgumentsNode* Parser::ParseArguments(ASTContext& context)
	{
		ConsumeOrFail(TOK_OPEN_PAREN);

		if(Match(TOK_CLOSE_PAREN))
		{
			Next();
			return context.CreateArguments();
		}

		Expression* argument = ParseAssignment(context);
		FailIfNull(argument);

		ArgumentListNode* arguments = context.CreateArgumentsList(argument);
		ArgumentListNode* next = arguments;

		while(Match(TOK_COMMA))
		{
			Next();

			argument = ParseAssignment(context);
			FailIfNull(argument);

			next = context.CreateArgumentsList(next, argument);
		}

		ConsumeOrFail(TOK_CLOSE_PAREN);

		return context.CreateArguments(arguments);
	}

	PropertyNode* Parser::ParseProperty(ASTContext& context, mo_bool strict)
	{
		mo_bool wasIdentifier = mo_false;
		mo_double numericName = 0;
		Expression* node = mo_null;
		const UTF8String* id = mo_null;
		const UTF8String* propertyName = mo_null;

		switch(mToken.type)
		{
		NAMED_PROPERTY:
			case TOK_IDENTIFIER:
				wasIdentifier = mo_true;
			case TOK_STRING:
			{
				id = mToken.data.id;
				NextIdentifier();
				
				if(Match(TOK_COLON))
				{
					Next();

					node = ParseAssignment(context);
					FailIfNull(node);

					return context.CreateProperty(*id, node, PropertyTypeConstant);
				}

				FailIfFalse(wasIdentifier);
				
				const UTF8String* accesorName = mo_null;
				ParameterNode* parameters = mo_null;
				FunctionNode* function = mo_null;
				mo_int32 openBracePosition = 0;
				mo_int32 closeBracePosition = 0;
				mo_int32 bodyBeginLine = 0;
				PropertyType type = PropertyTypeUnknown;

				if(*id == KnownIdentifiers::get)
					type = PropertyTypeGetter;
				else if(*id == KnownIdentifiers::set)
					type = PropertyTypeSetter;
				else
					Fail();

				if(mToken.type == TOK_IDENTIFIER || mToken.type == TOK_STRING)
					propertyName = mToken.data.id;
				else if(mToken.type == TOK_NUMBER)
					numericName = mToken.data.doubleValue;
				else
					Fail();

				Next();
				FailIfFalse((ParseFunction(context, mo_false, accesorName, parameters, function, openBracePosition, closeBracePosition, bodyBeginLine)));

				if(propertyName)
					return context.CreateGetterOrSetter(type, *propertyName, parameters, function, openBracePosition, closeBracePosition, bodyBeginLine, mLastLine);

				return context.CreateGetterOrSetter(type, numericName, parameters, function, openBracePosition, closeBracePosition, bodyBeginLine, mLastLine);
			}
			case TOK_NUMBER:
			{
				numericName = mToken.data.doubleValue;

				Next();
				ConsumeOrFail(TOK_COLON);
				
				node = ParseAssignment(context);
				FailIfNull(node);

				return context.CreateProperty(numericName, node, PropertyTypeConstant);
			}
			default:
				FailIfFalse(mToken.type & KeywordToken);
				goto NAMED_PROPERTY;
		}

		return mo_null;
	}

	ParameterNode* Parser::ParseParameters(ASTContext& context)
	{
		MatchOrFail(TOK_IDENTIFIER);

		FailIfFalseIfStrictWithNameAndMessage(DeclareParameter(*mToken.data.id), "Cannot declare a parameter named", mToken.data.id->Get(), "in strict mode");

		ParameterNode* parameters = context.CreateParameterList(*mToken.data.id);
		ParameterNode* next = parameters;

		Next();

		while(Match(TOK_COMMA))
		{
			const UTF8String* id = mo_null;

			Next();
			MatchOrFail(TOK_IDENTIFIER);
			
			id = mToken.data.id;
			FailIfFalseIfStrictWithNameAndMessage(DeclareParameter(*id), "Cannot declare a parameter named", id->Get(), "in strict mode");

			Next();
			next = context.CreateParameterList(*id, next);
		}

		return parameters;
	}

	FunctionNode* Parser::ParseFunctionBody(ASTContext& context)
	{
		if(Match(TOK_CLOSE_BRACE))
			return context.CreateFunction(GetStrictMode());

		ParserScopeRef scope = GetCurrentScope();
		ASTContext functionContext(const_cast<GlobalContext*>(mGlobalContext), const_cast<SourceCode*>(mSource), mLexer);
		DepthManager depth(&mStatementDepth);
		mStatementDepth = 0;

		mo_int32 bodyBeginLine = mLastLine;
		mo_int32 openBracePosition = mLastTokenBegin;
		StatementList* statements = ParseStatements(functionContext, mo_true);
		mo_int32 closeBracePosition = GetTokenBegin();
		IdentifierSet capturedVariables;
		CodeFeatures features = functionContext.GetFeatures();
		mo_int32 constants = functionContext.GetConstantCount();

		scope->GetCapturedVariables(capturedVariables);

		if(scope->GetStrictMode())
			features |= CodeFeaturesStrictMode;
		if(scope->GetShadowsArguments())
			features |= CodeFeaturesShadowsArguments;

		return context.CreateFunction(
			statements,
			functionContext.GetVariableDeclarations(),
			functionContext.GetFunctions(),
			capturedVariables,
			features,
			constants,
			openBracePosition,
			closeBracePosition,
			bodyBeginLine,
			scope->GetDoesFunctionReturn());
	}

	mo_bool Parser::ParseFunction(ASTContext& context, mo_bool needsName, const UTF8String*& name, ParameterNode*& parameters, FunctionNode*& function, mo_int32& openBracePosition, mo_int32& closeBracePosition, mo_int32& bodyBeginLine)
	{
		AutoPopScopeRef functionScope(this, PushScope());
		functionScope->SetIsFunction();

		if(Match(TOK_IDENTIFIER))
		{
			name = mToken.data.id;
			Next();

			if(!needsName)
				FailIfFalseIfStrict(functionScope->DeclareVariable(*name));
		}
		else
		{
			if(needsName)
				return mo_false;
		}

		ConsumeOrFail(TOK_OPEN_PAREN);

		if(!Match(TOK_CLOSE_PAREN))
		{
			parameters = ParseParameters(context);
			FailIfNull(parameters);
		}

		ConsumeOrFail(TOK_CLOSE_PAREN);
		MatchOrFail(TOK_OPEN_BRACE);

		openBracePosition = mToken.data.intValue;
		bodyBeginLine = GetTokenLine();

		Next();

		function = ParseFunctionBody(context);
		FailIfNull(function);

		if(functionScope->GetStrictMode() && name != mo_null)
		{
			FailIfTrueWithNameAndMessage(KnownIdentifiers::arguments == *name, "Function name", name->Get(), "is not valid in strict mode");
			FailIfTrueWithNameAndMessage(KnownIdentifiers::eval == *name, "Function name", name->Get(), "is not valid in strict mode");
		}

		closeBracePosition = mToken.data.intValue;

		PopScope(functionScope, mo_true);

		MatchOrFail(TOK_CLOSE_BRACE);
		Next();

		return mo_true;
	}

	mo_int32 Parser::GetBinaryOperatorPrecedence(TokenType type)
	{
		if(mAllowsIn)
			return type & (BinaryOpTokenPrecedenceMask << BinaryOpTokenAllowsInPrecedenceAdditionalShift);

		return type & BinaryOpTokenPrecedenceMask;
	}

	void Parser::SetErrorMessageForNameWithMessage(UTF8String name, const mo_utf8stringptr msgBefore, const mo_utf8stringptr msgAfter)
	{
		mo_utf8string prefix(msgBefore);
		mo_utf8string postfix(msgAfter);

		mHasError = mo_true;

		prefix += " '";
		prefix += name.Get();
		prefix += "' ";
		prefix += postfix;

		mError = prefix;
	}

	void Parser::SetErrorMessageForSpecialCase(TokenType expected)
	{
		mo_utf8string message;

		switch(expected)
		{
			case TOK_RESERVED_IF_STRICT:
				message  = "The keyword '";
				message += GetToken().Get();
				message += "' is reserved and cannot be used in strict mode";
				break;
			case TOK_RESERVED:
				message  = "The keyword '";
				message += GetToken().Get();
				message += "' is reserved and cannot be used";
				break;
			case TOK_NUMBER:
				message  = "Unexpected number '";
				message += GetToken().Get();
				message += "'";
				break;
			case TOK_IDENTIFIER:
				message  = "Identifier expected, but found '";
				message += GetToken().Get();
				message += "' instead";
				break;
			case TOK_STRING:
				message  = "Unexpected string ";
				message += GetToken().Get();
				break;
			case TOK_ERROR:
				message  = "The token '";
				message += GetToken().Get();
				message += "' was not recognized";
				break;
			case TOK_EOF:
				message = "Unexpected end of file";
				break;
			case TOK_RETURN:
				message = "Return statements are only valid inside a function";
				break;
			default:
				message = "Internal Error";
				break;
		}

		mError = message;
	}

	void Parser::SetErrorMessage(TokenType expected)
	{
		mo_utf8string name = GetTokenName(expected);

		mHasError = mo_true;

		if(!name.empty())
			mError = "Expected token: " + name;
		else
		{
			if(GetTokenName(mToken.type).empty())
				SetErrorMessageForSpecialCase(mToken.type);
			else
				SetErrorMessageForSpecialCase(expected);
		}
	}

	void Parser::SetErrorMessage(const mo_utf8stringptr msg)
	{
		mHasError = mo_true;
		mError = UTF8String(msg);
	}

	void Parser::SetErrorMessage()
	{
		mo_utf8string name = GetTokenName(mToken.type);

		mHasError = mo_true;

		if(!name.empty())
			mError = "Unexpected token: " + name;
		else
			SetErrorMessageForSpecialCase(mToken.type);
	}

	ParserScopeRef Parser::GetCurrentScope()
	{
		return ParserScopeRef(&mScopeStack, mScopeStack.size() - 1);
	}

	ParserScopeRef Parser::PushScope()
	{
		mo_bool isFunction = false;
		mo_bool isStrict = false;

		if(!mScopeStack.empty())
		{
			isFunction = mScopeStack.back().IsFunction();
			isStrict = mScopeStack.back().GetStrictMode();
		}

		mScopeStack.push_back(ParserScope(mGlobalContext, isFunction, isStrict));

		return GetCurrentScope();
	}

	void Parser::PopScope(ParserScopeRef& scope, mo_bool shouldTrackClosedVariables)
	{
		PopScopeImpl(scope, shouldTrackClosedVariables);
	}

	void Parser::PopScope(AutoPopScopeRef& scope, mo_bool shouldTrackClosedVariables)
	{
		scope.Pop();

		PopScopeImpl(scope, shouldTrackClosedVariables);
	}

	void Parser::PopScopeImpl(ParserScopeRef& scope, mo_bool shouldTrackClosedVariables)
	{
		MO_ASSERT(mScopeStack.size() > 1);

		mScopeStack[mScopeStack.size() - 2].CollectFreeVariables(&mScopeStack.back(), shouldTrackClosedVariables);
		mScopeStack.pop_back();
	}

	mo_bool Parser::DeclareVariable(const UTF8String& id)
	{
		MO_ASSERT(mScopeStack.size() > 0);

		mo_uint32 i = mScopeStack.size() - 1;

		while(!mScopeStack[i].AllowsNewDeclarations())
		{
			i--;
			MO_ASSERT(i < mScopeStack.size());
		}
		
		return mScopeStack[i].DeclareVariable(id);
	}

	mo_bool Parser::DeclareParameter(const UTF8String& id)
	{
		return GetCurrentScope()->DeclareParameter(id);	
	}

	void Parser::DeclareWrite(const UTF8String& id)
	{
		if(!mHasSyntaxBeenValidated)
			mScopeStack.back().DeclareWrite(id);
	}

	void Parser::PushLabel(const UTF8String& label, mo_bool isLoop)
	{
		GetCurrentScope()->PushLabel(label, isLoop);
	}

	void Parser::PopLabel()
	{
		GetCurrentScope()->PopLabel();
	}

	ParserScope::LabelInfo* Parser::GetLabel(const UTF8String& label)
	{
		ParserScopeRef current = GetCurrentScope();
		ParserScope::LabelInfo* result = mo_null;

		while(!(result = current->GetLabel(label)))
		{
			if(!current.HasContainingScope())
				return mo_null;

			current = current.GetContainingScope();
		}

		return result;
	}

	mo_bool Parser::AllowAutomaticSemicolon()
	{
		return (Match(TOK_CLOSE_BRACE) || Match(TOK_EOF) || mLexer->HasLineTerminator());
	}

	mo_bool Parser::InsertSemicolon()
	{
		if(mToken.type == TOK_SEMICOLON)
		{
			Next();
			return true;
		}

		return AllowAutomaticSemicolon();
	}

	mo_bool Parser::CanRecurse()
	{
		// TODO : need to determine whether or not we can keep parsing
		//        by checking the current stack and bailing out in case
		//        of an overflow
		return true;
	}

	mo_bool Parser::CanBreak()
	{
		ParserScopeRef current = GetCurrentScope();

		while(!current->CanBreak())
		{
			if(!current.HasContainingScope())
				return false;

			current = current.GetContainingScope();
		}

		return true;
	}

	mo_bool Parser::CanContinue()
	{
		ParserScopeRef current = GetCurrentScope();

		while(!current->CanContinue())
		{
			if(!current.HasContainingScope())
				return false;

			current = current.GetContainingScope();
		}

		return true;
	}

	const mo_utf8string Parser::GetTokenName(TokenType token)
	{
        switch (token) 
		{
			case TOK_NULL: 
				return "null";
			case TOK_TRUE:
				return "true";
			case TOK_FALSE: 
				return "false";
			case TOK_BREAK: 
				return "break";
			case TOK_CASE: 
				return "case";
			case TOK_DEFAULT: 
				return "default";
			case TOK_FOR: 
				return "for";
			case TOK_NEW: 
				return "new";
			case TOK_VAR: 
				return "var";
			case TOK_CONST: 
				return "const";
			case TOK_CONTINUE: 
				return "continue";
			case TOK_FUNCTION: 
				return "function";
			case TOK_IF: 
				return "if";
			case TOK_THIS: 
				return "this";
			case TOK_DO: 
				return "do";
			case TOK_WHILE: 
				return "while";
			case TOK_SWITCH: 
				return "switch";
			case TOK_WITH: 
				return "with";
			case TOK_THROW: 
				return "throw";
			case TOK_TRY: 
				return "try";
			case TOK_CATCH: 
				return "catch";
			case TOK_FINALLY: 
				return "finally";
			case TOK_DEBUGGER: 
				return "debugger";
			case TOK_ELSE: 
				return "else";
			case TOK_OPEN_BRACE: 
				return "{";
			case TOK_CLOSE_BRACE: 
				return "}";
			case TOK_OPEN_PAREN: 
				return "(";
			case TOK_CLOSE_PAREN: 
				return ")";
			case TOK_OPEN_BRACKET: 
				return "[";
			case TOK_CLOSE_BRACKET: 
				return "]";
			case TOK_COMMA: 
				return ",";
			case TOK_QUESTION: 
				return "?";
			case TOK_SEMICOLON: 
				return ";";
			case TOK_COLON: 
				return ":";
			case TOK_DOT: 
				return ".";
			case TOK_EQUAL: 
				return "=";
			case TOK_PLUS_EQUAL: 
				return "+=";
			case TOK_MINUS_EQUAL: 
				return "-=";
			case TOK_MULT_EQUAL: 
				return "*=";
			case TOK_DIV_EQUAL: 
				return "/=";
			case TOK_LSHIFT_EQUAL: 
				return "<<=";
			case TOK_RSHIFT_EQUAL: 
				return ">>=";
			case TOK_URSHIFT_EQUAL: 
				return ">>>=";
			case TOK_AND_EQUAL: 
				return "&=";
			case TOK_MOD_EQUAL: 
				return "%=";
			case TOK_XOR_EQUAL: 
				return "^=";
			case TOK_OR_EQUAL: 
				return "|=";
			case TOK_AUTO_PLUSPLUS: 
			case TOK_PLUSPLUS: 
				return "++";
			case TOK_AUTO_MINUSMINUS: 
			case TOK_MINUSMINUS: 
				return "--";
			case TOK_EXCLAMATION: 
				return "!";
			case TOK_TILDE: 
				return "~";
			case TOK_TYPEOF: 
				return "typeof";
			case TOK_VOID: 
				return "void";
			case TOK_DELETE: 
				return "delete";
			case TOK_OR: 
				return "||";
			case TOK_AND: 
				return "&&";
			case TOK_BIT_OR: 
				return "|";
			case TOK_BIT_XOR: 
				return "^";
			case TOK_BIT_AND: 
				return "&";
			case TOK_EQEQ: 
				return "==";
			case TOK_NE: 
				return "!=";
			case TOK_STREQ: 
				return "===";
			case TOK_STRNEQ: 
				return "!==";
			case TOK_LT: 
				return "<";
			case TOK_GT: 
				return ">";
			case TOK_LE: 
				return "<=";
			case TOK_GE: 
				return ">=";
			case TOK_INSTANCEOF: 
				return "instanceof";
			case TOK_IN: 
				return "in";
			case TOK_LSHIFT: 
				return "<<";
			case TOK_RSHIFT: 
				return ">>";
			case TOK_URSHIFT: 
				return ">>>";
			case TOK_PLUS: 
				return "+";
			case TOK_MINUS: 
				return "-";
			case TOK_TIMES: 
				return "*";
			case TOK_DIVIDE: 
				return "/";
			case TOK_MOD: 
				return "%";
			case TOK_RETURN: 
			case TOK_RESERVED_IF_STRICT:
			case TOK_RESERVED: 
			case TOK_NUMBER:
			case TOK_IDENTIFIER: 
			case TOK_STRING: 
			case TOK_ERROR:
			case TOK_EOF: 
				return "";
			case TOK_LAST_UNTAGGED: 
				break;
        }

        return "Internal Error";
	}
}