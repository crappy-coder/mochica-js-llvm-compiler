#ifndef MOENJIN_AST_LEFTSHIFTEXPRESSION_H
#define MOENJIN_AST_LEFTSHIFTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class LeftShiftExpression : public BinaryExpression
	{
		public:
			LeftShiftExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_LEFT_SHIFT; }
	};
}

#endif