#include "AST/ScopedStatement.h"
#include "AST/FunctionNode.h"
#include "AST/StatementList.h"
#include "Runtime/GlobalContext.h"

namespace MoEnjin
{
	ScopedStatement::ScopedStatement(GlobalContext* globalContext, mo_int32 lineNumber, mo_bool inStrictContext)
		: Statement(lineNumber)
		, mGlobalContext(globalContext)
		, mFeatures(inStrictContext ? CodeFeaturesStrictMode : CodeFeaturesNone)
		, mConstantCount(0)
		, mStatements(0) { }

	ScopedStatement::ScopedStatement(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount)
		: Statement(lineNumber)
		, mGlobalContext(globalContext)
		, mFeatures(features)
		, mSource(source)
		, mConstantCount(constantCount)
		, mStatements(statements) 
		, mVariables(variables)
		, mFunctions(functions)
	{
		mCapturedVariables.swap(capturedVariables);
	}
}