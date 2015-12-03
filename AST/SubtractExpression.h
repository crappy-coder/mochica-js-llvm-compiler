#ifndef MOENJIN_AST_SUBTRACTEXPRESSION_H
#define MOENJIN_AST_SUBTRACTEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class SubtractExpression : public BinaryExpression
	{
		public:
			SubtractExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_SUBTRACT; }
	};
}

#endif