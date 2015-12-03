#ifndef MOENJIN_RUNTIME_JSCALLFRAME_H
#define MOENJIN_RUNTIME_JSCALLFRAME_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSStackBuffer;
	class JSGlobalObject;
	class JSExecutionEnvironment;
	class JSObject;

	class JSCallFrame
	{
		friend class JSGlobalObject;

		enum
		{ 
			kLocalsOffset = 0,
			kArgumentsOffset = -6,

			kThisOffset = -5,
			kArgumentCountOffset = -4,
			kCallerFrameOffset = -3,
			kCalleeOffset = -2,
			kScopeOffset = -1
		};

		enum { HeaderSize = 5 };

		public:
			static mo_int32 GetOffsetForArgument(mo_int32 index) { return kArgumentsOffset - index; }
			static mo_int32 GetOffsetFor(mo_uint32 argumentCount) { return argumentCount + HeaderSize; }

		public:
			JSGlobalObject* GetGlobalObject() const;

			JSValue GetArgument(mo_uint32 index)
			{
				if(index >= GetArgumentCount())
					return jsUndef();

				return this[GetOffsetForArgument(index)].ToValue();
			}

			void SetArgument(mo_uint32 index, JSValue value) { this[GetOffsetForArgument(index)] = value; }

			JSCallFrame* Create(mo_uint32 argumentCount, mo_uint32 variableCount, JSObject* callee, JSValue thisValue, JSExecutionEnvironment* varEnv, JSExecutionEnvironment* lexEnv);

		public:		
			JSExecutionEnvironment* GetVariableScope() const { return this[kScopeOffset].data.scope.variable; }
			void SetVariableScope(JSExecutionEnvironment* value) { this[kScopeOffset].data.scope.variable = value; }

			JSExecutionEnvironment* GetLexicalScope() const { return this[kScopeOffset].data.scope.lexical; }
			void SetLexicalScope(JSExecutionEnvironment* value) { this[kScopeOffset].data.scope.lexical = value; }

			JSObject* GetCallee() const { return this[kCalleeOffset].ToObject(); }
			JSValue GetCalleeValue() const { return this[kCalleeOffset].ToValue(); }
			void SetCallee(JSObject* value) { this[kCalleeOffset] = jsObject(value); }

			JSValue GetThisValue() const { return this[kThisOffset].ToValue(); }
			void SetThisValue(JSValue value) { this[kThisOffset] = value; }

			JSCallFrame* GetCallerFrame() const { return this[kCallerFrameOffset].ToCallFrame(); }
			void SetCallerFrame(JSCallFrame* value) { this[kCallerFrameOffset] = value; }

			mo_uint32 GetArgumentCount() const { return this[kArgumentCountOffset].ToInt32(); }
			void SetArgumentCount(mo_uint32 value) { this[kArgumentCountOffset].GetPayload() = value; }

		public:
			JSCallFrame();
			JSCallFrame(const JSValue&);

			JSCallFrame* ToCallFrame() const;
			JSValue ToValue() const;
			JSValueImpl ToValueImpl() const;
			mo_int32 ToInt32() const;
			mo_bool ToBoolean() const;
			JSObject* ToObject() const;

			mo_int32 GetPayload() const;
			mo_int32& GetPayload();

			mo_int32 GetTag() const;
			mo_int32& GetTag();

			JSCallFrame& operator=(const JSCallFrame& x);
			JSCallFrame& operator=(const JSValue&);
			JSCallFrame& operator=(JSCallFrame*);

		private:
			union {
				JSValueImpl value;
				JSValueData encodedValue;
				JSCallFrame* callFrame;
				struct {
					JSExecutionEnvironment* variable;
					JSExecutionEnvironment* lexical;
				} scope;
			} data;
	};

	mo_inline_always JSCallFrame::JSCallFrame()
	{
		*this = JSValue();
	}

	mo_inline_always JSCallFrame::JSCallFrame(const JSValue& v)
	{
		data.value = JSValue::Encode(v);
	}

	mo_inline_always JSValue JSCallFrame::ToValue() const
	{
		return JSValue::Decode(data.value);
	}

	mo_inline_always JSValueImpl JSCallFrame::ToValueImpl() const
	{
		return data.value;
	}

	mo_inline_always JSCallFrame* JSCallFrame::ToCallFrame() const
	{
		return data.callFrame;
	}

	mo_inline_always mo_int32 JSCallFrame::ToInt32() const
	{
		return GetPayload();
	}

	mo_inline_always mo_bool JSCallFrame::ToBoolean() const
	{
		return !!GetPayload();
	}

	mo_inline_always JSObject* JSCallFrame::ToObject() const
	{
		return ToValue().ToNativeObject();
	}

	mo_inline_always mo_int32 JSCallFrame::GetPayload() const
	{
		return data.encodedValue.asBits.payload;
	}

	mo_inline_always mo_int32& JSCallFrame::GetPayload()
	{
		return data.encodedValue.asBits.payload;
	}

	mo_inline_always mo_int32 JSCallFrame::GetTag() const
	{
		return data.encodedValue.asBits.tag;
	}

	mo_inline_always mo_int32& JSCallFrame::GetTag()
	{
		return data.encodedValue.asBits.tag;
	}

	mo_inline_always JSCallFrame& JSCallFrame::operator=(const JSCallFrame& other)
	{
		*this = other;
		return *this;
	}

	mo_inline_always JSCallFrame& JSCallFrame::operator=(const JSValue& v)
	{
		data.value = JSValue::Encode(v);
		return *this;
	}

	mo_inline_always JSCallFrame& JSCallFrame::operator=(JSCallFrame* callFrame)
	{
		data.callFrame = callFrame;
		return *this;
	}
}

#endif