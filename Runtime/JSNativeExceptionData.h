#ifndef MOENJIN_RUNTIME_JSNATIVEEXCEPTIONDATA_H
#define MOENJIN_RUNTIME_JSNATIVEEXCEPTIONDATA_H

namespace MoEnjin
{
	struct JSNativeExceptionData
	{ 
		void* data; 

		JSNativeExceptionData()
			: data(0) { }

		JSNativeExceptionData(void* data)
			: data(data) { }

		JSNativeExceptionData(const JSNativeExceptionData& other)
			: data(other.data) { }
	};
}

#endif