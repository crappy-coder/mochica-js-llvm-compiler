#ifndef MOENJIN_AST_OPCODES_H
#define MOENJIN_AST_OPCODES_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
    #define FOR_EACH_OPCODE_ID(macro) \
        macro(op_eq)			\
        macro(op_neq)			\
        macro(op_stricteq)		\
        macro(op_nstricteq)		\
        macro(op_less)			\
        macro(op_lesseq)		\
        macro(op_greater)		\
        macro(op_greatereq)		\
        macro(op_add)			\
        macro(op_mul)			\
        macro(op_div)			\
        macro(op_mod)			\
        macro(op_sub)			\
        macro(op_lshift)		\
        macro(op_rshift)		\
        macro(op_urshift)		\
        macro(op_bitand)		\
        macro(op_bitxor)		\
        macro(op_bitor)			\
        macro(op_instanceof)	\
        macro(op_in)

#define OPCODE_ID_ENUM(opcode) opcode,
	typedef enum
	{
		FOR_EACH_OPCODE_ID(OPCODE_ID_ENUM)
	} OpCodeID;
#undef OPCODE_ID_ENUM
}

#endif