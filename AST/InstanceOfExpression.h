#ifndef MOENJIN_AST_INSTANCEOFEXPRESSION_H
#define MOENJIN_AST_INSTANCEOFEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/ThrowableBinaryExpression.h"

namespace MoEnjin
{
	class InstanceOfExpression : public ThrowableBinaryExpression
	{
		public:
			InstanceOfExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_INSTANCEOF; }
	};
}

#endif