#ifndef MOENJIN_AST_DIVIDEEXPRESSION_H
#define MOENJIN_AST_DIVIDEEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class DivideExpression : public BinaryExpression
	{
		public:
			DivideExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_DIVIDE; }
	};
}

#endif