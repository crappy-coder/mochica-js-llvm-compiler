#include "AST/ForStatement.h"

namespace MoEnjin
{
	ForStatement::ForStatement(mo_int32 lineNumber, Expression* initExpr, Expression* condExpr, Expression* incExpr, Statement* statment, mo_bool firstExpressionIsVarDecl)
		: Statement(lineNumber)
		, mInitializeExpr(initExpr)
		, mConditionExpr(condExpr)
		, mIncrementExpr(incExpr)
		, mStatement(statment)
		, mFirstExpressionIsVarDecl(initExpr && firstExpressionIsVarDecl) { }
}