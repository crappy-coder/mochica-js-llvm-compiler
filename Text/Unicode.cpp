#include "Unicode.h"

namespace MoEnjin
{
	CharCategory GetUnicodeCategory(mo_uint32 ch)
	{
		if (ch > 0xffff)
			return NoCategory;

		Poco::Unicode::CharacterProperties props;
		Poco::Unicode::properties(ch, props);

		return static_cast<CharCategory>(props.type);
	}
}