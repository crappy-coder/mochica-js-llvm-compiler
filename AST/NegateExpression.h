#ifndef MOENJIN_AST_NEGATEEXPRESSION_H
#define MOENJIN_AST_NEGATEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/UnaryExpression.h"

namespace MoEnjin
{
	class NegateExpression : public UnaryExpression
	{
		public:
			NegateExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_NEGATE; }
	};
}

#endif