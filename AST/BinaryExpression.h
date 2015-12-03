#ifndef MOENJIN_AST_BINARYEXPRESSION_H
#define MOENJIN_AST_BINARYEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"
#include "AST/OpCodes.h"

namespace MoEnjin
{
	class BinaryExpression : public Expression
	{
		public:
			BinaryExpression(mo_int32 lineNumber, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments);
			BinaryExpression(mo_int32 lineNumber, ResultKind kind, Expression* left, Expression* right, OpCodeID opcode, mo_bool rightHasAssignments);

			virtual const NodeType GetType() const { return NODE_BINARY_OP; }

			Expression* GetLeftExpression() { return mLeftExpr; }
			Expression* GetRightExpression() { return mRightExpr; }
			OpCodeID GetOpCodeID() const { return mOpcode; }

		private:
			Expression* mLeftExpr;
			Expression* mRightExpr;
			OpCodeID mOpcode;
			mo_bool mRightHasAssignments;

	};
}

#endif