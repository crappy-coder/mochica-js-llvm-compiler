#ifndef MOENJIN_AST_THROWABLEBINARYEXPRESSION_H
#define MOENJIN_AST_THROWABLEBINARYEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/BinaryExpression.h"
#include "AST/ThrowableExpression.h"

namespace MoEnjin
{
	class ThrowableBinaryExpression : public BinaryExpression, public ThrowableExpression
	{
		public:
			ThrowableBinaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments);
			ThrowableBinaryExpression(mo_int32 lineNumber, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments);
	};
}

#endif