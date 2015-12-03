#ifndef MOENJIN_AST_NOTEQUALSTRICTEXPRESSION_H
#define MOENJIN_AST_NOTEQUALSTRICTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class NotEqualStrictExpression : public ThrowableBinaryExpression
	{
		public:
			NotEqualStrictExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_NOT_EQUAL_STRICT; }
	};
}

#endif