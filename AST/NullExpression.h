#ifndef MOENJIN_AST_NULLEXPRESSION_H
#define MOENJIN_AST_NULLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class NullExpression : public Expression
	{
		public:
			NullExpression(mo_int32 lineNumber)
				: Expression(lineNumber, ResultKind::Null()) { }

			virtual const NodeType GetType() const { return NODE_NULL; }
	};
}

#endif