#ifndef MOENJIN_RUNTIME_JSSTACKENTRY_H
#define MOENJIN_RUNTIME_JSSTACKENTRY_H

#include "Platform/Configuration.h"
#include "Runtime/JSValue.h"

namespace MoEnjin
{
	class JSCallFrame;
	class JSStackBuffer;
	class JSStackEntry
	{
		public:
			enum 
			{ 
				kEntrySize = 4,
				kArgumentsOffset = -5,
				kThisValue = -4,
				kArgumentCount = -3,
				kCallerFrame = -2,
				kCallee = -1
			};

			JSStackEntry();
			JSStackEntry(const JSValue&);	
			JSStackEntry& operator=(const JSValue&);
			JSStackEntry& operator=(JSCallFrame*);

			JSCallFrame* ToCallFrame() const;
			JSValue ToValue() const;
			JSValueImpl ToValueImpl() const;

			mo_int32 Payload() const;
			mo_int32& Payload();

			mo_int32 Tag() const;
			mo_int32& Tag();

		private:
			union {
				JSValueImpl value;
				JSValueData valueData;
				JSCallFrame* callFrame;
			} u;
	};

	mo_inline_always JSStackEntry::JSStackEntry()
	{
		*this = JSValue();
	}

	mo_inline_always JSStackEntry::JSStackEntry(const JSValue& v)
	{
		u.value = JSValue::Encode(v);
	}

	mo_inline_always JSStackEntry& JSStackEntry::operator=(const JSValue& v)
	{
		u.value = JSValue::Encode(v);
		return *this;
	}

	mo_inline_always JSStackEntry& JSStackEntry::operator=(JSCallFrame* callFrame)
	{
		u.callFrame = callFrame;
		return *this;
	}

	mo_inline_always JSValue JSStackEntry::ToValue() const
	{
		return JSValue::Decode(u.value);
	}

	mo_inline_always JSValueImpl JSStackEntry::ToValueImpl() const
	{
		return u.value;
	}

	mo_inline_always JSCallFrame* JSStackEntry::ToCallFrame() const
	{
		return u.callFrame;
	}

	mo_inline_always mo_int32 JSStackEntry::Payload() const
	{
		return u.valueData.asBits.payload;
	}

	mo_inline_always mo_int32& JSStackEntry::Payload()
	{
		return u.valueData.asBits.payload;
	}

	mo_inline_always mo_int32 JSStackEntry::Tag() const
	{
		return u.valueData.asBits.tag;
	}

	mo_inline_always mo_int32& JSStackEntry::Tag()
	{
		return u.valueData.asBits.tag;
	}

	#define JS_STACK_ENTRY static_cast<JSStackEntry*>(this)
	#define JS_STACK_ENTRY_AT(idx) JS_STACK_ENTRY[idx]

	class JSCallFrame : private JSStackEntry
	{
		static const mo_intptr HostFlag = 1;

		public:
			static JSCallFrame* Create(JSStackEntry* base) { return mo_cast<JSCallFrame*>(base); }
			static JSCallFrame* NoCaller() { return mo_cast_type<JSCallFrame*>(HostFlag); }

			static mo_int32 GetArgumentOffset(mo_int32 index) { return JSStackEntry::kArgumentsOffset - index; }
			static mo_int32 GetOffsetFor(mo_uint32 argumentCount) { return argumentCount + JSStackEntry::kEntrySize; }

		public:
			JSValue GetCalleeAsValue() const { return this[JSStackEntry::kCallee].ToValue(); }
			JSValue GetThisValue() const { return this[JSStackEntry::kThisValue].ToValue(); }
			JSCallFrame* GetCallerFrame() const { return this[JSStackEntry::kCallerFrame].ToCallFrame(); }
			mo_uint32 GetArgumentCount() const { return this[JSStackEntry::kArgumentCount].Payload(); }

			JSValue GetArgument(mo_uint32 index)
			{
				if(index >= GetArgumentCount())
					return jsUndef();

				return this[GetArgumentOffset(index)].ToValue();
			}

			JSStackEntry* GetLocals() { return this; }
			JSStackEntry* GetExtent() { return GetLocals(); }

			// TODO
			JSStackBuffer* GetSourceBuffer() { return 0; }

			void SetArgument(mo_uint32 index, JSValue value) { this[GetArgumentOffset(index)] = value; }
			void SetArgumentCount(mo_uint32 value) { JS_STACK_ENTRY_AT(JSStackEntry::kArgumentCount).Payload() = value; }
			void SetThisValue(JSValue value) { JS_STACK_ENTRY_AT(JSStackEntry::kThisValue) = value; }
			void SetCallee(JSObject* value) { JS_STACK_ENTRY_AT(JSStackEntry::kCallee) = jsObject(value); }
			void SetCallerFrame(JSCallFrame* value) { JS_STACK_ENTRY_AT(JSStackEntry::kCallerFrame) = value; }

			mo_bool HasHostFlag() const { return !!(mo_cast_type<mo_intptr>(this) & HostFlag); }
			JSCallFrame* AddHostFlag() const { return mo_cast_type<JSCallFrame*>(mo_cast_type<mo_intptr>(this) | HostFlag); }
			JSCallFrame* RemoveHostFlag() const { return mo_cast_type<JSCallFrame*>(mo_cast_type<mo_intptr>(this) & ~HostFlag); }

			JSCallFrame& operator=(const JSStackEntry& x) { *JS_STACK_ENTRY = x; return *this; }

			mo_inline_always void Initialize(JSCallFrame* callerFrame, mo_int32 argumentCount, JSObject* callee)
			{
				MO_ASSERT(callerFrame);
				//MO_ASSERT(callerFrame == NoCaller() || callerFrame->RemoveHostFlag()->GetSourceBuffer()->GetEnd() >= this);

				SetCallee(callee);
				SetCallerFrame(callerFrame);
				SetArgumentCount(argumentCount);
			}
	};
}

#endif