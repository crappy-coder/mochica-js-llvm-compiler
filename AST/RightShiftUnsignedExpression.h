#ifndef MOENJIN_AST_RIGHTSHIFTUNSIGNEDEXPRESSION_H
#define MOENJIN_AST_RIGHTSHIFTUNSIGNEDEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class RightShiftUnsignedExpression : public BinaryExpression
	{
		public:
			RightShiftUnsignedExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_RIGHT_SHIFT_UNSIGNED; }
	};
}

#endif