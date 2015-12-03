#ifndef MOENJIN_AST_FORINSTATEMENT_H
#define MOENJIN_AST_FORINSTATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Statement.h"
#include "AST/Expression.h"
#include "AST/ThrowableExpression.h"
#include "Runtime/GlobalContext.h"

namespace MoEnjin
{
	class ForInStatement : public Statement, public ThrowableExpression
	{
		public:
			ForInStatement(GlobalContext* globalContext, mo_int32 lineNumber, Expression* left, Expression* right, Statement* statement);
			ForInStatement(GlobalContext* globalContext, mo_int32 lineNumber, Expression* initExpr, Expression* expr, Statement* statement, const UTF8String& id, mo_uint32 divot, mo_uint16 startPos, mo_uint16 endPos);

			virtual const NodeType GetType() const { return NODE_FOR_IN; }

			const UTF8String& GetId() const { return mId; }
			mo_bool IdentifierIsVarDecl() const { return mIsIdentifierVarDecl; }
			Expression* GetLeft() const { return mLeft; }
			Expression* GetRight() const { return mRight; }
			Expression* GetInitializer() const { return mInit; }
			Statement* GetStatement() const { return mStatement; }

		private:
			UTF8String mId;
			Expression* mInit;
			Expression* mLeft;
			Expression* mRight;
			Statement* mStatement;
			mo_bool mIsIdentifierVarDecl;
	};
}

#endif