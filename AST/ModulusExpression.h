#ifndef MOENJIN_AST_MODULUSEXPRESSION_H
#define MOENJIN_AST_MODULUSEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"

namespace MoEnjin
{
	class ModulusExpression : public BinaryExpression
	{
		public:
			ModulusExpression(mo_int32 lineNumber, Expression* left, Expression* right, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_MODULUS; }
	};
}

#endif