#ifndef MOENJIN_AST_EQUALSTRICTEXPRESSION_H
#define MOENJIN_AST_EQUALSTRICTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class EqualStrictExpression : public ThrowableBinaryExpression
	{
		public:
			EqualStrictExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_EQUAL_STRICT; }
	};
}

#endif