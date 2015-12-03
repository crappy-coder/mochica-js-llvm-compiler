#ifndef MOENJIN_AST_SWITCHINFO_H
#define MOENJIN_AST_SWITCHINFO_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	struct SwitchInfo
	{
		enum SwitchType
		{
			SwitchTypeNone,
			SwitchTypeImmediate,
			SwitchTypeCharacter,
			SwitchTypeString
		};

		mo_uint32 offset;
		SwitchType type;
	};
}

#endif