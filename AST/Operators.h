#ifndef MOENJIN_AST_OPERATORS_H
#define MOENJIN_AST_OPERATORS_H

namespace MoEnjin
{
	enum Operator
	{
		OpEqual,
		OpPlusEqual,
		OpMinusEqual,
		OpMultiplyEqual,
		OpDivideEqual,
		OpPlusPlus,
		OpMinusMinus,
		OpAndEqual,
		OpOrEqual,
		OpXOrEqual,
		OpModEqual,
		OpLShiftEqual,
		OpRShiftEqual,
		OpRShiftUnsignedEqual
	};

	enum LogicalOperator
	{
		OpLogicalAnd,
		OpLogicalOr
	};
}

#endif