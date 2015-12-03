#ifndef MOENJIN_AST_MULTIPLYEXPRESSION_H
#define MOENJIN_AST_MULTIPLYEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class MultiplyExpression : public BinaryExpression
	{
		public:
			MultiplyExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_MULTIPLY; }
	};
}

#endif