#ifndef MOENJIN_AST_STATEMENT_H
#define MOENJIN_AST_STATEMENT_H

#include "Platform/Configuration.h"
#include "AST/Node.h"

namespace MoEnjin
{
	class Statement : public Node
	{
		public:
			void SetLocation(mo_int32 startLine, mo_int32 endLine);

			const mo_int32 GetStartLine() const { return GetLineNumber(); }
			const mo_int32 GetEndLine() const { return mEndLine; }

		protected:
			Statement(mo_int32 lineNumber);

		private:
			mo_int32 mEndLine;
	};
}

#endif