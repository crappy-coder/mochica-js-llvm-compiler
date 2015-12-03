#ifndef MOENJIN_AST_THISEXPRESSION_H
#define MOENJIN_AST_THISEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ThisExpression : public Expression
	{
		public:
			ThisExpression(int lineNumber)
				: Expression(lineNumber) { }

			virtual const NodeType GetType() const { return NODE_THIS; }
	};
}

#endif