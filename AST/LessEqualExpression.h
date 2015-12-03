#ifndef MOENJIN_AST_LESSEQUALEXPRESSION_H
#define MOENJIN_AST_LESSEQUALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class LessEqualExpression : public BinaryExpression
	{
		public:
			LessEqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_LESS_EQ; }
	};
}

#endif