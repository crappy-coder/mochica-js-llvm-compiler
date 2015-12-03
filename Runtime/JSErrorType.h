#ifndef MOENJIN_RUNTIME_JSERRORTYPE_H
#define MOENJIN_RUNTIME_JSERRORTYPE_H

namespace MoEnjin
{
	enum JSErrorType
	{
		kJSNotImplementedError,
		kJSDefaultError =  0,
		kJSRuntimeError,
		kJSEvalError,
		kJSRangeError,
		kJSReferenceError,
		kJSSyntaxError,
		kJSTypeError,
		kJSUriError
	};
}

#endif