#ifndef MOENJIN_AST_INEXPRESSION_H
#define MOENJIN_AST_INEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class InExpression : public ThrowableBinaryExpression
	{
		public:
			InExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_IN; }
	};
}

#endif