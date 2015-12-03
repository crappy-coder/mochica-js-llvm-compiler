#ifndef MOENJIN_AST_ADDEXPRESSION_H
#define MOENJIN_AST_ADDEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class AddExpression : public BinaryExpression
	{
		public:
			AddExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_ADD; }
	};
}

#endif