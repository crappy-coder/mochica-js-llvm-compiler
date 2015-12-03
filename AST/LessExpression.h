#ifndef MOENJIN_AST_LESSEXPRESSION_H
#define MOENJIN_AST_LESSEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class LessExpression : public BinaryExpression
	{
		public:
			LessExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_LESS; }
	};
}

#endif