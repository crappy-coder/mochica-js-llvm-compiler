#ifndef MOENJIN_AST_BITWISEOREXPRESSION_H
#define MOENJIN_AST_BITWISEOREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class BitwiseOrExpression : public ThrowableBinaryExpression
	{
		public:
			BitwiseOrExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_BIT_OR; }
	};
}

#endif