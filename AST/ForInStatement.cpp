#include "AST/ForInStatement.h"
#include "AST/AssignResolveExpression.h"
#include "AST/ResolvableExpression.h"

namespace MoEnjin
{
	ForInStatement::ForInStatement(GlobalContext* globalContext, mo_int32 lineNumber, Expression* left, Expression* right, Statement* statement)
		: Statement(lineNumber)
		, mInit(0)
		, mLeft(left)
		, mRight(right)
		, mStatement(statement)
		, mIsIdentifierVarDecl(false) { }

	ForInStatement::ForInStatement(GlobalContext* globalContext, mo_int32 lineNumber, Expression* initExpr, Expression* expr, Statement* statement, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos)
		: Statement(lineNumber)
		, mId(id)
		, mInit(0)
		, mLeft(new ResolvableExpression(lineNumber, id, divot - startPos))
		, mRight(expr)
		, mStatement(statement)
		, mIsIdentifierVarDecl(true) 
	{
		if(initExpr)
		{
			AssignResolveExpression* node = new AssignResolveExpression(lineNumber, id, initExpr, true);
			node->SetExceptionLocation(divot, divot - startPos, endPos - divot);
			mInit = node;
		}
	}
}