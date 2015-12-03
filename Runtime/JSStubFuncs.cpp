#include "Runtime/JSStubFuncs.h"
#include "Runtime/JSConvert.h"
#include "Runtime/JSFunction.h"
#include "Runtime/JSFunctionPrototype.h"
#include "Runtime/JSGlobalObject.h"
#include "Runtime/JSList.h"
#include "Runtime/JSPropertyDescriptor.h"
#include "Runtime/JSObjectEnvironment.h"
#include "Runtime/JSScriptEnvironment.h"
#include "Runtime/JSStackFrame.h"
#include "Runtime/JSString.h"
#include "Runtime/JSStringRope.h"
#include "Runtime/KnownIdentifiers.h"

namespace MoEnjin
{
	mo_bool JSStubFuncs::DeleteProperty(JSStackFrame* exec, JSValue baseValue, const UTF8String& propertyName, mo_bool inStrictMode)
	{
		if(baseValue.IsNullOrUndefined())
		{	
			if(inStrictMode)
			{
				jsThrowNativeError(kJSSyntaxError);
				return mo_false;
			}

			return mo_true;
		}

		if(baseValue.IsEnvironment())
		{
			if(inStrictMode)
			{
				jsThrowNativeError(kJSSyntaxError);
				return mo_false;
			}

			return mo_cast<JSExecutionEnvironment*>(baseValue.GetAsNativeObject())->DeleteBinding(exec, propertyName);
		}

		return baseValue.ToObject(exec)->DeleteProperty(exec, propertyName, inStrictMode);
	}

	UTF8String JSStubFuncs::ResolvePropertyName(JSStackFrame* exec, JSValue property)
	{
		if(property.IsNullOrUndefined())
			return UTF8String::MakeNull();

		if(property.IsNativeString())
			return property.ToUTF8String();

		return property.ToString(exec)->GetValue();
	}

	void JSStubFuncs::BindVariable(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool inStrictMode)
	{
		JSExecutionEnvironment* env = exec->GetVariableEnvironment();

		if(!env->HasBinding(exec, name))
		{
			env->CreateMutableBinding(exec, name, mo_false);
			env->SetMutableBinding(exec, name, value, inStrictMode);
		}
	}

	void JSStubFuncs::BindExceptionVariable(JSStackFrame* exec, const UTF8String& name, JSValue value)
	{
		JSExecutionEnvironment* env = exec->GetLexicalEnvironment();

		if(!env->HasBinding(exec, name))
		{
			env->CreateMutableBinding(exec, name);
			env->SetMutableBinding(exec, name, value);
		}
	}

	void JSStubFuncs::BindFunction(JSStackFrame* exec, JSFunction* function)
	{
		JSGlobalObject* globalObject = exec->GetGlobalObject();
		JSExecutionEnvironment* env = exec->GetVariableEnvironment();
		const UTF8String& name = function->GetName(exec);

		if(!env->HasBinding(exec, name))
		{
			env->CreateMutableBinding(exec, name, mo_false);
			env->SetMutableBinding(exec, name, function, function->IsStrictMode());

			return;
		}

		// binding exists, check if we can modify it
		if(globalObject->GetEnvironment() == env)
		{
			JSPropertyDescriptor desc;

			if(!globalObject->GetPropertyDescriptor(exec, name, desc))
			{
				jsThrowNativeError(kJSTypeError);
				return;
			}

			if(!desc.IsConfigurable() && (desc.IsAccessor() || !(desc.IsWritable() && desc.IsEnumerable())))
			{
				jsThrowNativeError(kJSTypeError);
				return;
			}

			// re-use descriptor for property definition
			desc.SetValue(jsUndef());
			desc.SetConfigurable(mo_false);
			desc.SetWritable(mo_true);
			desc.SetEnumerable(mo_true);

			// define the function on the global object
			globalObject->DefineOwnProperty(exec, name, desc, mo_true);

			// update the environments binding
			env->SetMutableBinding(exec, name, function, function->IsStrictMode());
		}
	}

	void JSStubFuncs::BindFunctionParameter(JSStackFrame* exec, const UTF8String& name, JSValue value, mo_bool inStrictMode)
	{
		JSExecutionEnvironment* env = exec->GetVariableEnvironment();

		if(!env->HasBinding(exec, name))
			env->CreateMutableBinding(exec, name);

		env->SetMutableBinding(exec, name, value, inStrictMode);
	}

	void JSStubFuncs::BindFunctionArgumentsProperty(JSStackFrame* exec, JSFunction* function, const mo_utf8stringptr* parameterNames, mo_uint32 parameterNamesLen)
	{

	}

	JSFunction* JSStubFuncs::ConstructFunction(JSStackFrame* exec, JSExecutionEnvironment* scope, const UTF8String& name, mo_uint32 length, NativeFunctionPtr nativeFunction, mo_bool inStrictMode)
	{
		return JSFunction::Create(
			exec, 
			exec->GetGlobalObject()->GetFunctionPrototype(), 
			name, 
			scope,
			length, 
			nativeFunction, 
			nativeFunction, 
			inStrictMode, 
			mo_false);
	}

	JSValue JSStubFuncs::IncrementValue(JSStackFrame* exec, JSValue baseValue, const UTF8String& propertyName, mo_int32 amount, JSValue& oldValue)
	{
		// TODO - need to check for conversion exceptions and throw as appropriate

		JSValue value = baseValue.Get(exec, propertyName);
		JSValue newValue;

		if(value.IsInt32() && value.GetAsInt32() < INT_MAX)
		{
			oldValue = value;
			newValue = jsNumber(value.GetAsInt32() + amount);
		}
		else
		{
			double num = value.ToNumber(exec);

			oldValue = jsNumber(num);
			newValue = jsNumber(num + mo_cast<mo_double>(amount));
		}

		baseValue.Put(exec, propertyName, newValue);

		return newValue;
	}

	JSValue JSStubFuncs::Add(JSStackFrame* exec, JSValue x, JSValue y)
	{
		if(x.IsInt32() && y.IsInt32() && !((x.GetAsInt32() | y.GetAsInt32()) & 0xc0000000))
			return jsNumber(x.GetAsInt32() + y.GetAsInt32());
		
		if(x.IsNumber() && y.IsNumber())
			return jsNumber(x.GetAsNumber() + y.GetAsNumber());

		if(x.IsString() && !y.IsObject())
			return JSStringRope::FromStrings(exec->GetGlobalObject(), x.ToString(exec), y.ToString(exec));

		JSValue px = x.ToPrimitive(exec);
		JSValue py = y.ToPrimitive(exec);

		if(px.IsString() || py.IsString())
			return JSStringRope::FromStrings(exec->GetGlobalObject(), px.ToString(exec), py.ToString(exec));

		return jsNumber(px.ToNumber(exec) + py.ToNumber(exec));
	}

	JSValue JSStubFuncs::BitOpValue(JSStackFrame* exec, JSValue x, JSValue y, BitOp op)
	{
		mo_int32 xInt = x.ToInt32(exec);
		mo_int32 yInt = y.ToInt32(exec);
		mo_int32 result = 0;

		switch(op)
		{
			case kBitOpAND:
				result = xInt & yInt;
				break;
			case kBitOpOR:
				result = xInt | yInt;
				break;
			case kBitOpXOR:
				result = xInt ^ yInt;
				break;
		}

		return jsNumber(result);
	}

	mo_bool JSStubFuncs::IsLess(JSStackFrame* exec, JSValue x, JSValue y, mo_bool leftFirstEval)
	{
		if(x.IsInt32() && y.IsInt32())
			return (x.GetAsInt32() < y.GetAsInt32());

		if(x.IsNumber() && y.IsNumber())
			return (x.GetAsNumber() < y.GetAsNumber());

		if(x.IsString() && y.IsString())
			return (x.ToString(exec)->GetValue() < y.ToString(exec)->GetValue());

		mo_double xNum = 0;
		mo_double yNum = 0;
		JSValue xPrimitive;
		JSValue yPrimitive;

		if(leftFirstEval)
		{
			xPrimitive = x.ToPrimitiveNumber(exec, xNum);
			yPrimitive = y.ToPrimitiveNumber(exec, yNum);
		}
		else
		{
			yPrimitive = y.ToPrimitiveNumber(exec, yNum);
			xPrimitive = x.ToPrimitiveNumber(exec, xNum);
		}

		if(!xPrimitive.IsString() || !yPrimitive.IsString())
			return (xNum < yNum);

		return (xPrimitive.ToString(exec)->GetValue() < yPrimitive.ToString(exec)->GetValue());
	}

	mo_bool JSStubFuncs::IsLessOrEqual(JSStackFrame* exec, JSValue x, JSValue y, mo_bool leftFirstEval)
	{
		if(x.IsInt32() && y.IsInt32())
			return (x.GetAsInt32() <= y.GetAsInt32());

		if(x.IsNumber() && y.IsNumber())
			return (x.GetAsNumber() <= y.GetAsNumber());

		if(x.IsString() && y.IsString())
			return !(y.ToString(exec)->GetValue() < x.ToString(exec)->GetValue());

		mo_double xNum = 0;
		mo_double yNum = 0;
		JSValue xPrimitive;
		JSValue yPrimitive;

		if(leftFirstEval)
		{
			xPrimitive = x.ToPrimitiveNumber(exec, xNum);
			yPrimitive = y.ToPrimitiveNumber(exec, yNum);
		}
		else
		{
			yPrimitive = y.ToPrimitiveNumber(exec, yNum);
			xPrimitive = x.ToPrimitiveNumber(exec, xNum);
		}

		if(!xPrimitive.IsString() || !yPrimitive.IsString())
			return (xNum <= yNum);

		return !(yPrimitive.ToString(exec)->GetValue() < xPrimitive.ToString(exec)->GetValue());
	}

	mo_bool JSStubFuncs::AreEqual(JSStackFrame* exec, JSValue x, JSValue y)
	{
		do
		{
			if(x.IsInt32() && y.IsInt32())
				return (x.GetAsInt32() == y.GetAsInt32());

			if(x.IsNumber() && y.IsNumber())
				return (x.GetAsNumber() == y.GetAsNumber());

			if(x.IsNativeString() && y.IsNativeString())
				return (x.ToUTF8String() == y.ToUTF8String());

			if(x.IsString() && y.IsString())
				return (x.ToString(exec)->GetValue() == y.ToString(exec)->GetValue());

			if(x.IsNullOrUndefined() || y.IsNullOrUndefined())
				return (x.IsNullOrUndefined() == y.IsNullOrUndefined());

			if(x.IsObject())
			{
				if(y.IsObject())
					return (x == y);

				JSValue xPrimitive = x.ToPrimitive(exec);

				if(exec->HasException())
					return mo_false;
				
				// next: object(x).primitive == ?(y)
				x = xPrimitive;
				continue;
			}

			if(y.IsObject())
			{
				JSValue yPrimitive = y.ToPrimitive(exec);

				if(exec->HasException())
					return mo_false;

				// next: ?(x) == object(y).primitive
				y = yPrimitive;
				continue;
			}

			if(x.IsString() || y.IsString())
			{
				mo_double xNum = x.ToNumber(exec);
				mo_double yNum = y.ToNumber(exec);

				return (xNum == yNum);
			}

			if((x.IsBoolean() && y.IsNumber()) || (y.IsBoolean() && x.IsNumber()))
			{
				mo_double xNum = mo_cast<mo_double>(x.IsBoolean() ? x.GetAsBoolean() : y.GetAsBoolean());
				mo_double yNum = x.IsNumber() ? x.GetAsNumber() : y.GetAsNumber();

				return (xNum == yNum);
			}

			break;
		} while(true);

		return (x == y);
	}

	mo_bool JSStubFuncs::AreStrictEqual(JSStackFrame* exec, JSValue x, JSValue y)
	{
		if(x.IsInt32() && y.IsInt32())
			return (x.GetAsInt32() == y.GetAsInt32());

		if(x.IsNumber() && y.IsNumber())
			return (x.GetAsNumber() == y.GetAsNumber());

		if(x.IsString() && y.IsString())
			return (x.ToString(exec)->GetValue() == y.ToString(exec)->GetValue());

		return (x == y);
	}
}