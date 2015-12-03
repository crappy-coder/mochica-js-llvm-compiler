#include "AST/ASTUtils.h"
#include "AST/AST.h"

namespace MoEnjin
{
	mo_bool CanExpressionFoldToBoolean(Expression* expr, mo_bool& constBoolValue)
	{
		constBoolValue = mo_false;

		if(AST_IS_BOOLEAN_NODE(expr))
		{
			constBoolValue = mo_cast<BooleanExpression*>(expr)->GetValue();
			return mo_true;
		}
		else if(AST_IS_NULL_NODE(expr))
			return mo_true;
		else if(AST_IS_NUMBER_NODE(expr))
		{
			NumberExpression* numExpr = mo_cast<NumberExpression*>(expr);
			ResultKind numResultKind = numExpr->GetResultKind();

			if(numResultKind.IsInt32())
			{
				constBoolValue = (mo_cast<mo_int64>(numExpr->GetValue()) != 0);
				return mo_true;
			}
			
			if(numResultKind.IsNumber())
			{
				constBoolValue = (numExpr->GetValue() > 0.0 || numExpr->GetValue() < 0.0);
				return mo_true;
			}
		}
		else if(AST_IS_STRING_NODE(expr))
		{
			StringExpression* strExpr = mo_cast<StringExpression*>(expr);

			constBoolValue = !strExpr->GetValue().IsEmpty();
			return mo_true;
		}

		return mo_false;
	}
}