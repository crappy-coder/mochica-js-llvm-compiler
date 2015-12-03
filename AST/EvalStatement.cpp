#include "AST/EvalStatement.h"

namespace MoEnjin
{
	EvalStatement::EvalStatement(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount)
		: ScopedStatement(globalContext, lineNumber, source, statements, variables, functions, capturedVariables, features, constantCount) { }
}