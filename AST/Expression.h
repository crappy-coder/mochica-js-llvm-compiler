#ifndef MOENJIN_AST_EXPRESSION_H
#define MOENJIN_AST_EXPRESSION_H

#include "Platform/Configuration.h"
#include "AST/Node.h"
#include "AST/ResultKind.h"

namespace MoEnjin
{
	class Expression : public Node
	{
		public:
			ResultKind GetResultKind() const { return mResultKind; }

		protected:
			Expression(mo_int32 lineNumber, ResultKind resultKind = ResultKind::Unknown())
				: Node(lineNumber)
				, mResultKind(resultKind) { }

			mo_bool IsThis() const { return (GetType() == NODE_THIS); }

		private:
			ResultKind mResultKind;
	};
}

#endif