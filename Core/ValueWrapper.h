#ifndef MOENJIN_CORE_VALUEWRAPPER_H
#define MOENJIN_CORE_VALUEWRAPPER_H

namespace MoEnjin
{
	MO_TEMPLATE_WITH_TYPE(T) struct ValueWrapper
	{
		T value;
	};
}

#endif