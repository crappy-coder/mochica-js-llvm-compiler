#ifndef MOENJIN_AST_BITWISEANDEXPRESSION_H
#define MOENJIN_AST_BITWISEANDEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class BitwiseAndExpression : public ThrowableBinaryExpression
	{
		public:
			BitwiseAndExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_BIT_AND; }
	};
}

#endif