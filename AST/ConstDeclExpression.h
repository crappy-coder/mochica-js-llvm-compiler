#ifndef MOENJIN_AST_CONSTDECLEXPRESSION_H
#define MOENJIN_AST_CONSTDECLEXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class ConstDeclExpression : public Expression
	{
		public:
			ConstDeclExpression(mo_int32 lineNumber, const UTF8String& id, Expression* initExpr, ConstDeclExpression* next = mo_null);

			virtual const NodeType GetType() const { return NODE_CONST_DECL; }

			ConstDeclExpression* GetNext() const { return mNext; }
			Expression* GetInitializer() const { return mInitExpr; }
			mo_bool HasInitializer() const { return !!mInitExpr; }
			const UTF8String& GetId() const { return mId; }

		private:
			Expression* mInitExpr;
			ConstDeclExpression* mNext;
			UTF8String mId;
	};
}

#endif