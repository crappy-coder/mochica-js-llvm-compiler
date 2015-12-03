#ifndef MOENJIN_PARSER_PARSER_H
#define MOENJIN_PARSER_PARSER_H

#include "Platform/Configuration.h"
#include "AST/AST.h"
#include "AST/ASTContext.h"
#include "AST/CodeFeatures.h"
#include "Parser/Lexer.h"
#include "Parser/ParserScope.h"
#include "Parser/Token.h"
#include "Runtime/GlobalContext.h"
#include "Runtime/SourceCode.h"

namespace MoEnjin
{
	class Parser
	{
		MO_NON_COPYABLE_CLASS(Parser);

		struct AllowInOverride
		{
			AllowInOverride(Parser* parser)
				: mParser(parser)
				, mPrevAllowsIn(parser->mAllowsIn)
			{
				parser->mAllowsIn = true;
			}

			~AllowInOverride()
			{
				mParser->mAllowsIn = mPrevAllowsIn;
			}

			private:
				Parser* mParser;
				mo_bool mPrevAllowsIn;
		};

		struct AutoPopScopeRef : public ParserScopeRef
		{
			AutoPopScopeRef(Parser* parser, ParserScopeRef scope)
				: ParserScopeRef(scope)
				, mParser(parser) { }

			~AutoPopScopeRef()
			{
				if(mParser)
					mParser->PopScope(*this, false);
			}

			void Pop()
			{
				mParser = mo_null;
			}

			private:
				Parser* mParser;
		};

		struct DepthManager
		{
			DepthManager(int* depth)
				: mInitialDepth(*depth)
				, mDepth(depth) { }

			~DepthManager()
			{
				*mDepth = mInitialDepth;
			}

			private:
				int mInitialDepth;
				int* mDepth;
		};

		public:
			enum ParserStrictness {
				ParserStrictnessNormal,
				ParserStrictnessStrict
			};

			enum ParserMode {
				ParserModeProgram,
				ParserModeFunction
			};

		public:
			Parser(GlobalContext* globalContext, const SourceCode& source, const UTF8StringCollection& parameters, ParserStrictness strictness, ParserMode mode);
			
			template<class TNode>
			TNode* Parse(mo_bool asFunction = mo_false);

		private:
			ParserScopeRef GetCurrentScope();
			ParserScopeRef PushScope();

			void PopScope(ParserScopeRef& scope, mo_bool shouldTrackClosedVariables);
			void PopScope(AutoPopScopeRef& scope, mo_bool shouldTrackClosedVariables);
			void PopScopeImpl(ParserScopeRef& scope, mo_bool shouldTrackClosedVariables);

			mo_bool DeclareVariable(const UTF8String& id);
			mo_bool DeclareParameter(const UTF8String& id);
			void DeclareWrite(const UTF8String& id);

			void PushLabel(const UTF8String& label, mo_bool isLoop);
			void PopLabel();
			ParserScope::LabelInfo* GetLabel(const UTF8String& label);

			void BeginLoop() { GetCurrentScope()->BeginLoop(); }
			void EndLoop() { GetCurrentScope()->EndLoop(); }

			void BeginSwitch() { GetCurrentScope()->BeginSwitch(); }
			void EndSwitch() { GetCurrentScope()->EndSwitch(); }

			void SetStrictMode() { GetCurrentScope()->SetStrictMode(); }
			mo_bool IsStrictModeValid() { return GetCurrentScope()->IsStrictModeValid(); }
			mo_bool GetStrictMode() { return GetCurrentScope()->GetStrictMode(); }

			UTF8String GetToken();
			UTF8String Parse();

			mo_int32 GetBinaryOperatorPrecedence(TokenType type);

			mo_bool AllowAutomaticSemicolon();
			mo_bool InsertSemicolon();

			mo_bool CanRecurse();
			mo_bool CanBreak();
			mo_bool CanContinue();

			const mo_utf8string GetTokenName(TokenType token);

			void SetErrorMessageForNameWithMessage(UTF8String name, const mo_utf8stringptr msgBefore, const mo_utf8stringptr msgAfter);
			void SetErrorMessageForSpecialCase(TokenType expected);
			void SetErrorMessage(TokenType expected);
			void SetErrorMessage(const mo_utf8stringptr msg);
			void SetErrorMessage();

			StatementList* ParseStatements(ASTContext& context, mo_bool checkForStrictMode = mo_false);
			Statement* ParseStatement(ASTContext& context, const UTF8String*& outDirective, mo_unsigned* outDirectiveLiteralLength = 0);
			Statement* ParseFunctionDeclaration(ASTContext& context);
			Statement* ParseVarDeclaration(ASTContext& context);
			Statement* ParseConstDeclaration(ASTContext& context);
			Statement* ParseDoWhile(ASTContext& context);
			Statement* ParseWhile(ASTContext& context);
			Statement* ParseFor(ASTContext& context);
			Statement* ParseBreak(ASTContext& context);
			Statement* ParseContinue(ASTContext& context);
			Statement* ParseReturn(ASTContext& context);
			Statement* ParseThrow(ASTContext& context);
			Statement* ParseWith(ASTContext& context);
			Statement* ParseSwitch(ASTContext& context);
			Statement* ParseTry(ASTContext& context);
			Statement* ParseDebugger(ASTContext& context);
			Statement* ParseIf(ASTContext& context);
			Statement* ParseExpressionStatement(ASTContext& context);
			Statement* ParseExpressionOrLabel(ASTContext& context);
			Statement* ParseBlock(ASTContext& context);
			Expression* ParseExpression(ASTContext& context);
			Expression* ParseAssignment(ASTContext& context);
			Expression* ParseConditional(ASTContext& context);
			Expression* ParseBinary(ASTContext& context);
			Expression* ParseUnary(ASTContext& context);
			Expression* ParseMember(ASTContext& context);
			Expression* ParsePrimary(ASTContext& context);
			Expression* ParseArrayLiteral(ASTContext& context);
			Expression* ParseObjectLiteral(ASTContext& context);
			Expression* ParseObjectLiteralStrict(ASTContext& context);

			Expression* ParseVarDeclarationList(ASTContext& context, mo_int32& declarations, const UTF8String*& lastId, Expression*& lastInitializer, mo_int32& idBegin, mo_int32& initBegin, mo_int32& initEnd);
			ConstDeclExpression* ParseConstDeclarationList(ASTContext& context);
			ClauseListNode* ParseSwitchClauses(ASTContext& context);
			CaseClauseNode* ParseSwitchDefaultClause(ASTContext& context);
			ArgumentsNode* ParseArguments(ASTContext& context);
			PropertyNode* ParseProperty(ASTContext& context, mo_bool strict = mo_false);
			ParameterNode* ParseParameters(ASTContext& context);
			FunctionNode* ParseFunctionBody(ASTContext& context);

			mo_bool ParseFunction(ASTContext& context, mo_bool needsName, const UTF8String*& name, ParameterNode*& parameters, FunctionNode*& function, mo_int32& openBracePosition, mo_int32& closeBracePosition, mo_int32& bodyBeginLine);

			mo_inline void Next()
			{
				mLastLine = mToken.line;
				mLastTokenBegin = mToken.begin;
				mLastTokenEnd = mToken.end;
		
				mLexer->SetLastLineNumber(mLastLine);
				mToken.type = mLexer->NextToken(&mToken, GetStrictMode());
			}

			mo_inline void NextIdentifier()
			{
				mLastLine = mToken.line;
				mLastTokenBegin = mToken.begin;
				mLastTokenEnd = mToken.end;

				mLexer->SetLastLineNumber(mLastLine);
				mToken.type = mLexer->NextIdentifier(&mToken, GetStrictMode());
			}

			mo_inline mo_bool Consume(TokenType expected)
			{
				mo_bool result = (mToken.type == expected);

				if(result)
					Next();

				return result;
			}

			mo_inline mo_bool Match(TokenType expected)
			{
				return (mToken.type == expected);
			}

			mo_inline mo_int32 GetTokenBegin()
			{
				return mToken.begin;
			}

			mo_inline mo_int32 GetTokenEnd()
			{
				return mToken.end;
			}

			mo_inline mo_int32 GetTokenLine()
			{
				return mToken.line;
			}

			mo_inline mo_int32 GetLastTokenBegin()
			{
				return mLastTokenBegin;
			}

			mo_inline mo_int32 GetLastTokenEnd()
			{
				return mLastTokenEnd;
			}

			mo_inline static mo_bool IsUnaryOp(TokenType token) { return ((token & UnaryOpToken) != 0); }

		private:
			GlobalContext* mGlobalContext;
			const SourceCode* mSource;
			const UTF8String* mLastIdentifier;
			ParserScopeStack mScopeStack;
			Lexer* mLexer;
			StatementList* mStatements;
			VariableList mVariableDeclarations;
			FunctionList mFunctions;
			IdentifierSet mCapturedVariables;
			CodeFeatures mFeatures;
			Token mToken;
			UTF8String mError;
			mo_bool mHasError;
			mo_bool mHasSyntaxBeenValidated;
			mo_bool mAllowsIn;
			mo_int32 mLastLine;
			mo_int32 mLastTokenBegin;
			mo_int32 mLastTokenEnd;
			mo_int32 mAssignmentCount;
			mo_int32 mNonLHSCount;
			mo_int32 mNonTrivialExpressionCount;
			mo_int32 mStatementDepth;
			mo_int32 mConstantCount;
	};

	template<class TNode>
	TNode* Parser::Parse(mo_bool asFunction)
	{
		int errorLine = -1;
		UTF8String errorMessage("");

		if(asFunction)
			mLexer->SetIsReparsing();

		mStatements = mo_null;

		errorMessage = Parse();

		mo_int32 lexLine = mLexer->GetLineNumber();
		mo_bool lexHasError = mLexer->HasError();
		UTF8String lexError = lexHasError ? mLexer->GetError() : UTF8String::MakeNull();

		MO_ASSERT(lexError.IsNull() != lexHasError);

		mLexer->Clear();

		if(!errorMessage.IsNull() || lexHasError)
		{
			errorLine = lexLine;
			errorMessage = !lexError.IsNull() ? lexError : errorMessage;

			mStatements = mo_null;
		}

		TNode* result = mo_null;

		if(mStatements)
		{
			result = new TNode(mGlobalContext, mLexer->GetLastLineNumber(), *mSource, mStatements, mVariableDeclarations, mFunctions, mCapturedVariables, mFeatures, mConstantCount);
			result->SetLocation(mSource->GetStartLine(), mLastLine);
		}
		
		return result;
	}
}

#endif