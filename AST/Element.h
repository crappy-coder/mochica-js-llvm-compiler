#ifndef MOENJIN_AST_ELEMENT_H
#define MOENJIN_AST_ELEMENT_H

#include "Platform/Configuration.h"
#include "AST/Expression.h"

namespace MoEnjin
{
	class Element
	{
		public:
			Element(mo_int32 elision, Expression* expr);
			Element(mo_int32 elision, Expression* expr, Element* next);

			Element* GetNext() const { return mNext; }
			Expression* GetExpression() const { return mExpression; }
			mo_int32 GetElision() const { return mElision; }

		private:
			Element* mNext;
			Expression* mExpression;
			mo_int32 mElision;
	};
}

#endif