#ifndef MOENJIN_AST_LOGICALNOTEXPRESSION_H
#define MOENJIN_AST_LOGICALNOTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/UnaryExpression.h"

namespace MoEnjin
{
	class LogicalNotExpression : public UnaryExpression
	{
		public:
			LogicalNotExpression(mo_int32 lineNumber, Expression* expression);

			virtual const NodeType GetType() const { return NODE_LOGICAL_NOT; }
	};
}

#endif