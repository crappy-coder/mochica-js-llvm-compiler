#ifndef MOENJIN_AST_EVALSTATEMENT_H
#define MOENJIN_AST_EVALSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/ScopedStatement.h"

namespace MoEnjin
{
	class EvalStatement : ScopedStatement
	{
		public:
			EvalStatement(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount);

			virtual const NodeType GetType() const { return NODE_EVAL; }
	};
}

#endif