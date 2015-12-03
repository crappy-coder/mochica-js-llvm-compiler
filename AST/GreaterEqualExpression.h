#ifndef MOENJIN_AST_GREATEREQUALEXPRESSION_H
#define MOENJIN_AST_GREATEREQUALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class GreaterEqualExpression : public BinaryExpression
	{
		public:
			GreaterEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_GREATER_EQ; }
	};
}

#endif