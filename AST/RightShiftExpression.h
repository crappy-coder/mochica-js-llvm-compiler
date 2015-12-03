#ifndef MOENJIN_AST_RIGHTSHIFTEXPRESSION_H
#define MOENJIN_AST_RIGHTSHIFTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class RightShiftExpression : public BinaryExpression
	{
		public:
			RightShiftExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_RIGHT_SHIFT; }
	};
}

#endif