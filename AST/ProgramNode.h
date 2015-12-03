#ifndef MOENJIN_AST_PROGRAMNODE_H
#define MOENJIN_AST_PROGRAMNODE_H

#include "Platform/Configuration.h"
#include "AST/ScopedStatement.h"

namespace MoEnjin
{
	class ProgramNode : public ScopedStatement
	{
		public:
			ProgramNode(GlobalContext* globalContext, mo_int32 lineNumber, const SourceCode& source, StatementList* statements, const VariableList& variables, const FunctionList& functions, IdentifierSet& capturedVariables, CodeFeatures features, mo_int32 constantCount);

			virtual const NodeType GetType() const { return NODE_PROGRAM; }

			UTF8String GetSourceScriptName() const;
	};
}

#endif