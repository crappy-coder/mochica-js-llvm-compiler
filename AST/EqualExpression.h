#ifndef MOENJIN_AST_EQUALEXPRESSION_H
#define MOENJIN_AST_EQUALEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class EqualExpression : public ThrowableBinaryExpression
	{
		public:
			EqualExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_EQUAL; }
	};
}

#endif