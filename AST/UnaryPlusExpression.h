#ifndef MOENJIN_AST_UNARYPLUSEXPRESSION_H
#define MOENJIN_AST_UNARYPLUSEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/UnaryExpression.h"

namespace MoEnjin
{
	class UnaryPlusExpression : public UnaryExpression
	{
		public:
			UnaryPlusExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_UNARY_PLUS; }
	};
}

#endif