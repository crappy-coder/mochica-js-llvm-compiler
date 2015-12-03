#ifndef MOENJIN_AST_NOTEQUALEXPRESSION_H
#define MOENJIN_AST_NOTEQUALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class NotEqualExpression : public ThrowableBinaryExpression
	{
		public:
			NotEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_NOT_EQUAL; }
	};
}

#endif