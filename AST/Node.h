#ifndef MOENJIN_AST_NODE_H
#define MOENJIN_AST_NODE_H

#include "Platform/Configuration.h"
#include "AST/NodeType.h"

namespace MoEnjin
{
	class Node
	{
		public:
			virtual ~Node() { }
			virtual const NodeType GetType() const { return NODE_UNKNOWN; }

			const mo_int32 GetLineNumber() const { return mLineNumber; }

		protected:
			Node(mo_int32 lineNumber)
				: mLineNumber(lineNumber) { }

			mo_int32 mLineNumber;
	};
}

#endif