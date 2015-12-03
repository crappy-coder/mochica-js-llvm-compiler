#ifndef MOENJIN_AST_BITWISEXOREXPRESSION_H
#define MOENJIN_AST_BITWISEXOREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class BitwiseXOrExpression : public ThrowableBinaryExpression
	{
		public:
			BitwiseXOrExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_BIT_XOR; }
	};
}

#endif