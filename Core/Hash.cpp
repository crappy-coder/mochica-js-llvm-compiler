#include "Core/Hash.h"

namespace MoEnjin
{
	mo_uint32 Hash::HashString(const mo_utf8stringptr str, mo_int32 len)
	{
		return SuperFastHash(str, len);
	}

	mo_unsigned Hash::HashDouble(mo_double value)
	{
		union N {
			mo_double val;
			mo_uint32 is[2];
		} u;

		u.val = value;

		mo_unsigned x = u.is[0] ^ u.is[1];
		x = (x >> 20) ^ (x >> 8);
		return x;
	}
}