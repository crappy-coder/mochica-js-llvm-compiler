#include "Runtime/JSStubs.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSFunctionPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSList.h"
#include "Runtime/JSPropertyDescriptor.h"
#include "Runtime/JSPropertyNameIterator.h"
#include "Runtime/JSNativeExceptionHandler.h"
#include "Runtime/JSObjectEnvironment.h"
#include "Runtime/JSScriptEnvironment.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSStrings.h"
#include "Runtime/JSStubFuncs.h"
#include "Runtime/KnownIdentifiers.h"
#include "Platform/StackInfo.h"

namespace MoEnjin
{
	// TODO: put this somewhere else so it's available to all of the runtime code
	#define RUNTIME_DEBUG_MSG(msg) (mo_debug_print("[runtime:" __FUNCTION__ "] " msg "\n"))
	#define RUNTIME_DEBUG_MSGF(msg, ...) (mo_debug_print("[runtime:" __FUNCTION__ "] " msg "\n", __VA_ARGS__))
	#define FN_ENTER() RUNTIME_DEBUG_MSG("enter")
	#define FN_LEAVE() RUNTIME_DEBUG_MSG("leave")

	#define HANDLE_EXCEPTION(exec) \
		do { \
			if(exec->HasException()) { \
				JSStackFrame::ThrowRuntimeException(exec, exec->GetGlobalObject()->GetException()); \
				return JSValue::Encode(jsUndef()); \
			} \
		} while(0)
	

	//-------------------------------------------------------------------------------------------------
	// SECTION: STARTUP/SHUTDOWN CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------
	// MO_INITIALIZE:
	//---------------------------------------------------------------
	void RUNTIME_CALL mo_runtime_initialize(NativeContext* context)
	{
		FN_ENTER();
		
		JSGlobalObject* obj = JSGlobalObject::Create();
		context->global = obj;

		FN_LEAVE();
	}


	//---------------------------------------------------------------
	// MO_INITIALIZE_ENVIRONMENT
	//
	// Returns:
	//   The top call frame for the program.
	//
	// Description:
	//   This is the first call the program code makes which 
	//   initializes and sets up the initial execution context, 
	//   standard prototypes and built-in object constructors.
	//---------------------------------------------------------------
	JSStackFrame* RUNTIME_CALL mo_runtime_initialize_environment(NativeContext* context)
	{
		FN_ENTER();

		JSGlobalObject* obj = context->global;
		return obj->InitializeExecutionState();
	}


	//---------------------------------------------------------------
	// MO_SHUTDOWN:
	//---------------------------------------------------------------
	void RUNTIME_CALL mo_runtime_shutdown(NativeContext* context)
	{
		FN_ENTER();
	}



	//-------------------------------------------------------------------------------------------------
	// SECTION: EXCEPTION CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------
	// MO_EXCEPTION_SET_GLOBAL_HANDLER:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_set_global_handler(NativeContext* context, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		// allow for multiple unhandled exception handlers to be used
		if(context->global->HasUnhandledExceptionHandler())
			handler->previous = context->global->GetUnhandledExceptionHandler();

		context->global->SetUnhandledExceptionHandler(handler);
	}


	//---------------------------------------------------------------
	// MO_EXCEPTION_REPORT:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_report(NativeContext* context, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		if(!handler)
			return;

		JSValue exValue = JSValue::Decode(handler->exceptionValue);
		JSStackFrame* stackFrame = context->global->GetCurrentStackFrame();
		UTF8String str = exValue.ToString(stackFrame)->GetValue();

		MO_REPORT_ERROR(str.GetChars());
	}


	//---------------------------------------------------------------
	// MO_EXCEPTION_ERROR_CODE:
	//---------------------------------------------------------------
	mo_int32 RUNTIME_CALL_EH mo_runtime_exception_error_code(NativeContext* context, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		return 0;
	}


	//---------------------------------------------------------------
	// MO_EXCEPTION_THROW:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_throw(JSStackFrame* exec, JSValueImpl exceptionValue)
	{
		FN_ENTER();

		// we set the exception in the global object, so it updates it's last
		// exception and such that we use for other work down the line
		exec->GetGlobalObject()->SetException(JSValue::Decode(exceptionValue));

		// throw the exception
		JSStackFrame::ThrowRuntimeException(exec, exec->GetGlobalObject()->GetException());
	}

	//---------------------------------------------------------------
	// MO_EXCEPTION_EXTRACT:
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL_EH mo_runtime_exception_extract(JSStackFrame* exec, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		return handler->exceptionValue;
	}

	//---------------------------------------------------------------
	// MO_EXCEPTION_TRY_ENTER:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_try_enter(JSStackFrame* exec, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		if(exec->HasExceptionHandler())
			handler->previous = exec->GetExceptionHandler();

		exec->SetExceptionHandler(handler);
	}

	//---------------------------------------------------------------
	// MO_EXCEPTION_TRY_EXIT:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_try_exit(JSStackFrame* exec, JSNativeExceptionHandler* handler)
	{
		FN_ENTER();

		JSNativeExceptionHandler* topHandler = exec->HasExceptionHandler() ? exec->GetExceptionHandler() : mo_null;

		if(!topHandler || topHandler != handler)
		{
			RUNTIME_DEBUG_MSG("Error: mismatched exception handlers");
			return;
		}

		RUNTIME_DEBUG_MSGF("Exiting try and removing the catch handler: %p.", topHandler);

		JSNativeExceptionHandler* previousHandler = topHandler->previous;
		topHandler->previous = mo_null;

		// pop the current handler to the previous one
		exec->SetExceptionHandler(previousHandler);
	}

	//---------------------------------------------------------------
	// MO_EXCEPTION_CATCH_ENTER:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_catch_enter(JSStackFrame* exec, const char* catchVariableName, JSValueImpl exceptionValue)
	{
		FN_ENTER();

		exec->EnterExceptionEnvironment();
		JSStubFuncs::BindExceptionVariable(exec, UTF8String(catchVariableName), JSValue::Decode(exceptionValue));
	}

	//---------------------------------------------------------------
	// MO_EXCEPTION_CATCH_EXIT:
	//---------------------------------------------------------------
	void RUNTIME_CALL_EH mo_runtime_exception_catch_exit(JSStackFrame* exec)
	{
		FN_ENTER();

		exec->ExitExceptionEnvironment();
	}


	//-------------------------------------------------------------------------------------------------
	// SECTION: DEBUGGING CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	// TODO: implement when debugging support has been added
	void RUNTIME_CALL mo_debug_break(JSStackFrame* exec, mo_int32 startLine, mo_int32 endLine)
	{
		FN_ENTER();
		
		mo_debug_print_line("BREAKPOINT HIT: line=%d", startLine);

		FN_LEAVE();
	}

	//-------------------------------------------------------------------------------------------------
	// SECTION: OP CALLS
	//-------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------
	// STUB_ENTER_CALL: isFunctionCode(bool)
	//                  isStrictCode(bool)
	//                  parameterNames(NativeStringCollection)
	//                  variableNames(NativeStringCollection)
	//                  functionBindingCount(int)
	//                  functionBindings(NativeFunctionBinding[])
	//
	// Description:
	//   ECMA 5.1, §10.5 - Declaration Binding Instantiation
	//
	//   Called for each function as part of the function prologue as
	//   well as the programs prologue, right after enter_program is
	//   executed.
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_enter_call(JSStackFrame* exec, EnterFunctionCallInitData* initData)
	{
		FN_ENTER();

		mo_int32 argumentCount = exec->GetArgumentCount();

		// get the function that called this stub (our calee), if this is global code
		// the callee will be the global object, which is not a function, though we
		// don't currently need it now.
		JSFunction* func = (initData->isFunctionCode ? JSConvert::Cast<JSFunction*>(exec->GetCallee()) : mo_null);

		// ECMA 5.1, §10.5.4
		// 
		// if this is function code then bind the parameter names
		if(initData->isFunctionCode && initData->numParameters)
		{
			for(mo_int32 i = 0; i < initData->numParameters; ++i)
			{
				UTF8String argName = initData->parameters[i];
				JSValue v = jsUndef();

				if(i < argumentCount)
					v = exec->GetArgument(i);

				JSStubFuncs::BindFunctionParameter(exec, argName, v, initData->isStrictMode);
			}
		}


		// ECMA 5.1, §10.5.5
		//
		// create and bind the functions (global and function code)
		for(mo_int32 i = 0; i < initData->numFunctions; ++i)
		{
			const NativeFunctionBinding* binding = initData->functions[i];

			JSFunction* func = JSStubFuncs::ConstructFunction(exec, exec->GetVariableEnvironment(), binding->name, binding->parameterCount, binding->function, binding->inStrictMode);
			JSStubFuncs::BindFunction(exec, func);
		}


		// ECMA 5.1, §10.5.6 - 10.5.7
		//
		// if this is function code then create the arguments object
		if(initData->isFunctionCode && initData->numParameters)
			JSStubFuncs::BindFunctionArgumentsProperty(exec, func, initData->parameters, initData->numParameters);


		// ECMA 5.1, §10.5.8
		//
		// bind the variable names
		for(mo_int32 i = 0; i < initData->numVariables; ++i)
			JSStubFuncs::BindVariable(exec, initData->variables[i], jsUndef(), initData->isStrictMode);


		return JSValue::Encode(jsBool(mo_true));
	}



	//---------------------------------------------------------------
	// STUB_END_CALL: execute function epiloque
	//---------------------------------------------------------------
	void RUNTIME_CALL mo_runtime_stub_end_call(JSStackFrame* exec)
	{
		FN_ENTER();

		//callFrame.stackFrame->Pop();
	}



	//---------------------------------------------------------------
	// STUB_CALL_FUNC: function(val[function])
	//                 baseValue(val)
	//                 argumentsCount(int)
	//                 arguments(val[argumentsCount])
	//
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_call_func(JSStackFrame* exec, JSValue functionValue, JSValue baseValue, mo_uint32 argsLen, JSValue* args)
	{
		FN_ENTER();

		JSValue thisValue = jsUndef();
		JSList arguments(args, argsLen);

		if(!functionValue.IsObject())
		{
			jsThrowNativeErrorWithMessage(kJSTypeError, "Function reference is not an object.");
			return jsUndef();
		}

		if(!functionValue.IsCallable())
		{
			jsThrowNativeErrorWithMessage(kJSTypeError, "Function reference is not a callable object.");
			return jsUndef();
		}
		
		if((baseValue.IsObject() && !baseValue.IsEnvironment()) || baseValue.IsPrimitive())
			thisValue = baseValue;

		JSObject* function = functionValue.ToObject(exec);
		JSValue result = exec->ExecuteCall(function, function->GetCallInfo(exec), thisValue, arguments);

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_CALL_FUNC_BY_NAME: functionName(string)
	//                         baseValue(val)
	//                         argumentsCount(int)
	//                         arguments(val[argumentsCount])
	//
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_call_func_by_name(JSStackFrame* exec, JSValue functionNameValue, JSValue baseValue, mo_uint32 argsLen, JSValue* args)
	{
		FN_ENTER();

		JSList arguments(args, argsLen);
		JSValue thisValue = jsUndef();
		JSValue functionValue = jsUndef();
		UTF8String functionName = JSStubFuncs::ResolvePropertyName(exec, functionNameValue);

		// an <undefined> base means we need to resolve it here, this is a slight optimization
		// instead of making a call to stub_resolve_base beforehand
		if(baseValue.IsUndefined())
			baseValue = exec->ResolveBase(functionName);

		// get the function from base
		functionValue = baseValue.Get(exec, functionName);

		// there is no function with that name in base, the Get
		// already throws a TypeError so no need to do it again
		if(functionValue.IsNullOrUndefined())
			return jsUndef();

		// make sure the function's value is what we expect
		if(!functionValue.IsObject())
		{
			jsThrowNativeErrorWithMessage(kJSTypeError, "Function reference is not an object.");
			return jsUndef();
		}

		if(!functionValue.IsCallable())
		{
			jsThrowNativeErrorWithMessage(kJSTypeError, "Function reference is not a callable object.");
			return jsUndef();
		}
		
		if((baseValue.IsObject() && !baseValue.IsEnvironment()) || baseValue.IsPrimitive())
			thisValue = baseValue;

		JSObject* function = functionValue.ToObject(exec);
		JSValue result = exec->ExecuteCall(function, function->GetCallInfo(exec), thisValue, arguments);

		HANDLE_EXCEPTION(exec);
		FN_LEAVE();

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_CONSTRUCT: construct a new object
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_construct(JSStackFrame* exec, JSValue constructorValue, mo_uint32 argsLen, JSValue* args)
	{
		FN_ENTER();

		JSList constructorArgs(args, argsLen);

		if(!constructorValue.IsObject())
		{
			jsThrowNativeError(kJSTypeError);
			return jsUndef();
		}

		JSObject* constructor = constructorValue.ToObject(exec);
		JSObject* newObj = exec->ExecuteConstruct(constructor, constructor->GetConstructInfo(exec), constructorArgs);

		if(!newObj)
			return jsUndef();

		return JSValue::Encode(JSValue(newObj));
	}


	//---------------------------------------------------------------
	// NEW_ARRAY: elements(jsval[length])
	//            length(uint)
	//
	// Description:
	// constructs a new array
	//
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_new_array(JSStackFrame* exec, JSValue* elements, mo_uint32 length)
	{
		FN_ENTER();

		for(mo_int32 i = 0; i < length; ++i)
		{
			JSValue val = elements[i];

			mo_debug_print_line("array element: %s\n", val.ToString(exec)->GetValue().GetChars());
		}

		return JSValue::Encode(jsUndef());
	}



	//---------------------------------------------------------------
	// STUB_NEW_FUNC: name(string)
	//                length(int)
	//                inStrictMode(bool)
	//                function(NativeFunctionPtr)
	//
	// Description:
	// constructs a new function object
	//
	// ECMA 5.1, §13 ( FunctionExpression : function (...) {...} )
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_new_func(JSStackFrame* exec, const mo_utf8stringptr nameData, mo_uint32 length, mo_bool inStrictMode, NativeFunctionPtr func)
	{
		FN_ENTER();

		UTF8String name = nameData;
		JSExecutionEnvironment* funcEnv = exec->GetLexicalEnvironment();

		if(!name.IsEmpty())
		{
			funcEnv = JSScriptEnvironment::Create(exec->GetGlobalObject(), funcEnv);
			funcEnv->CreateImmutableBinding(exec, name);
		}

		JSFunction* function = JSStubFuncs::ConstructFunction(exec, funcEnv, name, length, func, inStrictMode);

		if(!name.IsEmpty())
			funcEnv->InitializeImmutableBinding(exec, name, jsObject(function));

		return JSValue::Encode(jsObject(function));
	}


	//---------------------------------------------------------------
	// STUB_NEW_PROP_ITER: base(val)
	//
	// Description: 
	//
	//---------------------------------------------------------------
	JSPropertyNameIterator* RUNTIME_CALL mo_runtime_stub_new_prop_iter(JSStackFrame* exec, JSValue base)
	{
		FN_ENTER();

		if(base.IsNullOrUndefined())
			return mo_null;

		JSObject* obj = base.ToObject(exec);
		JSPropertyNameIterator* propIter = JSPropertyNameIterator::Create(exec, obj);

		return propIter;
	}



	//---------------------------------------------------------------
	// STUB_INC_PROP_ITER: propertyIterator(val)
	//                     varBaseValue(val)
	//                     varPropertyValue(val)
	//
	// Description:
	//
	//---------------------------------------------------------------
	mo_bool RUNTIME_CALL mo_runtime_stub_inc_prop_iter(JSStackFrame* exec, JSPropertyNameIterator* propIter, JSValue baseValue, JSValue propertyValue)
	{
		FN_ENTER();

		JSValue propName;

		// get the next property available, if a property was
		// deleted during enumeration, we might get an empty
		// value, in which case we just continue until there
		// is no more, or we find a non-empty name
		while(!propIter->IsDone())
		{
			propName = propIter->GetCurrent(exec);

			// found one
			if(!propName.IsEmpty())
				break;

			propIter->MoveNext();
		}

		// couldn't find any more props
		if(propName.IsEmpty())
			return mo_false;

		baseValue.Put(exec, JSStubFuncs::ResolvePropertyName(exec, propertyValue), propName);
		propIter->MoveNext();

		return mo_true;
	}



	//---------------------------------------------------------------
	// STUB_GET_VAR: baseValue(val), propertyValue(val)
	//
	// Description: 
	//   get the value of a variable
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_get_var(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);

		return JSValue::Encode(baseValue.Get(exec, propertyName));
	}



	//---------------------------------------------------------------
	// STUB_PUT_VAR: baseValue(val), propertyValue(val), value(val)
	//
	// Description: 
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_put_var(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue, JSValue propertyValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);

		baseValue.Put(exec, propertyName, propertyValue);

		return JSValue::Encode(propertyValue);
	}



	//---------------------------------------------------------------
	// STUB_PUT_VAR_BY_NAME: propertyName(string), value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_put_var_by_name(JSStackFrame* exec, const mo_utf8stringptr propertyNameData, JSValue propertyValue)
	{
		FN_ENTER();

		UTF8String propertyName = propertyNameData;
		JSValue baseValue = exec->ResolveBase(propertyName);

		baseValue.Put(exec, propertyName, propertyValue);

		return JSValue::Encode(propertyValue);
	}



	//---------------------------------------------------------------
	// STUB_PUT_VAR_IN_THIS: propertyValue(val), value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_put_var_in_this(JSStackFrame* exec, JSValue propertyNameValue, JSValue propertyValue)
	{
		FN_ENTER();

		JSValue baseValue = exec->GetThisValue().ToThisObject(exec);
		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);

		baseValue.Put(exec, propertyName, propertyValue);

		return JSValue::Encode(propertyValue);
	}



	//---------------------------------------------------------------
	// STUB_DELETE_VAR: baseValue(val), propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_delete_var(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);

		return JSStubFuncs::DeleteProperty(exec, baseValue, propertyName);
	}



	//---------------------------------------------------------------
	// STUB_DELETE_VAR_IN_THIS: propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_delete_var_in_this(JSStackFrame* exec, JSValue propertyNameValue)
	{
		FN_ENTER();

		JSValue baseValue = exec->GetThisValue().ToThisObject(exec);
		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);

		return JSStubFuncs::DeleteProperty(exec, baseValue, propertyName);
	}



	//---------------------------------------------------------------
	// STUB_THIS
	//
	// Description:
	//   returns the current this value as a 'this' object
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_this(JSStackFrame* exec)
	{
		FN_ENTER();

		return JSValue::Encode(exec->GetThisValue().ToThisObject(exec));
	}



	//---------------------------------------------------------------
	// STUB_RESOLVE: propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_resolve(JSStackFrame* exec, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue baseValue = exec->ResolveBase(propertyName);

		return JSValue::Encode(baseValue.Get(exec, propertyName));
	}



	//---------------------------------------------------------------
	// STUB_RESOLVE_BASE: propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_resolve_base(JSStackFrame* exec, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue baseValue = exec->ResolveBase(propertyName);

		return JSValue::Encode(baseValue);
	}



	//---------------------------------------------------------------
	// STUB_TYPEOF: value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_typeof(JSStackFrame* exec, JSValue value)
	{
		FN_ENTER();

		return JSValue::Encode(value.ToTypeString(exec));
	}



	//---------------------------------------------------------------
	// STUB_TO_NUMBER: value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_to_number(JSStackFrame* exec, JSValue value)
	{
		FN_ENTER();

		if(value.IsNumber())
			return JSValue::Encode(value);

		value = jsNumber(value.ToNumber(exec));

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(value);
	}


	//---------------------------------------------------------------
	// STUB_TO_BOOLEAN: value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_to_boolean(JSStackFrame* exec, JSValue value)
	{
		FN_ENTER();

		if(value.IsBoolean())
			return JSValue::Encode(value);

		value = jsBool(value.ToBoolean(exec));

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(value);
	}



	//---------------------------------------------------------------
	// STUB_NEGATE: value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_negate(JSStackFrame* exec, JSValue value)
	{
		FN_ENTER();

		if(value.IsInt32() && (value.GetAsInt32() & 0x7fffffff))
			return JSValue::Encode(jsNumber(-value.GetAsInt32()));

		value = jsNumber(-value.ToNumber(exec));

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(value);
	}


	//---------------------------------------------------------------
	// STUB_PREFIX_INC: baseValue(val), propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_prefix_inc(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue oldValue;

		return JSValue::Encode(JSStubFuncs::IncrementValue(exec, baseValue, propertyName, 1, oldValue));
	}



	//---------------------------------------------------------------
	// STUB_PREFIX_DEC: baseValue(val), propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_prefix_dec(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue oldValue;

		return JSValue::Encode(JSStubFuncs::IncrementValue(exec, baseValue, propertyName, -1, oldValue));
	}


	//---------------------------------------------------------------
	// STUB_POSTFIX_INC: baseValue(val), propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_postfix_inc(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue oldValue;

		JSStubFuncs::IncrementValue(exec, baseValue, propertyName, 1, oldValue);

		return JSValue::Encode(oldValue);
	}



	//---------------------------------------------------------------
	// STUB_POSTFIX_DEC: baseValue(val), propertyValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_postfix_dec(JSStackFrame* exec, JSValue baseValue, JSValue propertyNameValue)
	{
		FN_ENTER();

		UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, propertyNameValue);
		JSValue oldValue;

		JSStubFuncs::IncrementValue(exec, baseValue, propertyName, -1, oldValue);

		return JSValue::Encode(oldValue);
	}


	//---------------------------------------------------------------
	// STUB_BIT_OP: op(int), xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_bit_op(JSStackFrame* exec, BitOp op, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result = JSStubFuncs::BitOpValue(exec, x, y, op);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_NOT: value(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_not(JSStackFrame* exec, JSValue value)
	{
		FN_ENTER();
		
		if(value.IsBoolean())
			value = jsBool(!value.GetAsBoolean());
		else
			value = jsBool(!value.ToBoolean(exec));

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(value);
	}



	//---------------------------------------------------------------
	// STUB_ADD: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_add(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result = JSStubFuncs::Add(exec, x, y);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_SUBTRACT: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_subtract(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsInt32() && y.IsInt32() && !((x.GetAsInt32() | y.GetAsInt32()) & 0xc0000000))
			result = jsNumber(x.GetAsInt32() - y.GetAsInt32());
		else
		{
			result = jsNumber(x.ToNumber(exec) - y.ToNumber(exec));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_MULTIPLY: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_multiply(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsInt32() && y.IsInt32() && !(x.GetAsInt32() | y.GetAsInt32()) >> 15)
			result = jsNumber(x.GetAsInt32() * y.GetAsInt32());
		else
		{
			result = jsNumber(x.ToNumber(exec) * y.ToNumber(exec));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}


	//---------------------------------------------------------------
	// STUB_DIVIDE: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_divide(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result = jsNumber(x.ToNumber(exec) / y.ToNumber(exec));

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(result);
	}


	//---------------------------------------------------------------
	// STUB_MODULUS: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_modulus(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsInt32() && y.IsInt32() && y.GetAsInt32() != 0)
			result = jsNumber(x.GetAsInt32() % y.GetAsInt32());
		else
		{
			mo_double a = x.ToNumber(exec);
			mo_double b = y.ToNumber(exec);

			result = jsNumber(fmod(a, b));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}


	//---------------------------------------------------------------
	// STUB_LSHIFT: value(val), shift(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_lshift(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsInt32() && y.IsInt32())
			result = jsNumber(x.GetAsInt32() << (y.GetAsInt32() & 0x1F));
		else
		{
			result = jsNumber((x.ToInt32(exec)) << (y.ToUInt32(exec) & 0x1F));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_RSHIFT: value(val), shift(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_rshift(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsInt32() && y.IsInt32())
			result = jsNumber(x.GetAsInt32() >> (y.GetAsInt32() & 0x1F));
		else
		{
			result = jsNumber((x.ToInt32(exec)) >> (y.ToUInt32(exec) & 0x1F));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}



	//---------------------------------------------------------------
	// STUB_URSHIFT: value(val), shift(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_urshift(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		JSValue result;

		if(x.IsUInt32() && y.IsInt32())
			result = jsNumber(x.GetAsInt32() >> (y.GetAsInt32() & 0x1F));
		else
		{
			result = jsNumber((x.ToUInt32(exec)) >> (y.ToUInt32(exec) & 0x1F));
			HANDLE_EXCEPTION(exec);
		}

		return JSValue::Encode(result);
	}


	//---------------------------------------------------------------
	// STUB_IS_LESS: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_less(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::IsLess(exec, x, y, mo_true);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_IS_LESS_OR_EQUAL: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_less_or_equal(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::IsLessOrEqual(exec, x, y, mo_true);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_IS_GREATER: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_greater(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::IsLess(exec, y, x, mo_false);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_IS_GREATER_OR_EQUAL: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_greater_or_equal(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::IsLessOrEqual(exec, y, x, mo_false);

		HANDLE_EXCEPTION(exec);

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_IS_INSTANCEOF: value(val), constructor(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_instanceof(JSStackFrame* exec, JSValue value, JSValue constructor)
	{
		FN_ENTER();

		JSValue proto = constructor.Get(exec, KnownIdentifiers::prototype);
		mo_bool result = mo_false;

		if(constructor.IsObject() && constructor.ToNativeObject()->ImplementsHasInstance())
			result = constructor.ToNativeObject()->HasInstance(exec, value, proto);

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_IS_IN: property(val|string), base(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_is_in(JSStackFrame* exec, JSValue property, JSValue baseValue)
	{
		FN_ENTER();

		mo_bool result = mo_false;

		if(!baseValue.IsObject())
			jsThrowNativeErrorWithMessage(kJSTypeError, "Constructor is not an object.");
		else
		{
			JSObject* obj = baseValue.ToNativeObject();
			mo_uint32 i = 0;

			if(property.TryGetAsUInt32(i))
				result = obj->HasProperty(exec, i);
			else
			{
				UTF8String propertyName = JSStubFuncs::ResolvePropertyName(exec, property);
				HANDLE_EXCEPTION(exec);
				result = obj->HasProperty(exec, propertyName);
			}
		}

		return JSValue::Encode(jsBool(result));
	}


	//---------------------------------------------------------------
	// STUB_EQUAL: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_equal(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::AreEqual(exec, x, y);

		return JSValue::Encode(jsBool(result));
	}



	//---------------------------------------------------------------
	// STUB_EQUAL_STRICT: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_equal_strict(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = JSStubFuncs::AreStrictEqual(exec, x, y);

		return JSValue::Encode(jsBool(result));
	}



	//---------------------------------------------------------------
	// STUB_NOT_EQUAL: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_not_equal(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = !JSStubFuncs::AreEqual(exec, x, y);

		return JSValue::Encode(jsBool(result));
	}



	//---------------------------------------------------------------
	// STUB_NOT_EQUAL_STRICT: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_not_equal_strict(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		mo_bool result = !JSStubFuncs::AreStrictEqual(exec, x, y);

		return JSValue::Encode(jsBool(result));
	}



	//---------------------------------------------------------------
	// STUB_LOGICAL_AND: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_logical_and(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();
		
		if(!x.ToBoolean(exec))
			return JSValue::Encode(x);

		return JSValue::Encode(y);
	}



	//---------------------------------------------------------------
	// STUB_LOGICAL_OR: xValue(val), yValue(val)
	//
	// Description:
	//   ***
	//---------------------------------------------------------------
	JSValueImpl RUNTIME_CALL mo_runtime_stub_logical_or(JSStackFrame* exec, JSValue x, JSValue y)
	{
		FN_ENTER();

		if(x.ToBoolean(exec))
			return JSValue::Encode(x);

		return JSValue::Encode(y);
	}
}