#ifndef MOENJIN_AST_GREATEREXPRESSION_H
#define MOENJIN_AST_GREATEREXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class GreaterExpression : public BinaryExpression
	{
		public:
			GreaterExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_GREATER; }
	};
}

#endif