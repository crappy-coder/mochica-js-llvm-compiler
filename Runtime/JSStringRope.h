#ifndef MOENJIN_RUNTIME_JSSTRINGROPE_H
#define MOENJIN_RUNTIME_JSSTRINGROPE_H

#include "Platform/Configuration.h"
#include "Runtime/JSString.h"

namespace MoEnjin
{
	class JSStringRope : public JSString
	{
		MO_NON_COPYABLE_CLASS(JSStringRope);

		friend class JSString;

		static const mo_unsigned MaximumFiberCount = 3;

		public:
			class Builder
			{
				public:
					Builder(JSGlobalObject* globalObject)
						: mGlobalObject(globalObject)
						, mRope(mo_null)
						, mIndex(0) { }

					~Builder() 
					{
						mGlobalObject = mo_null;
						mRope = mo_null;
						mIndex = 0;
					}

					mo_unsigned GetLength() const { return (mRope ? mRope->GetLength() : 0); }

					JSString* ToString()
					{
						// nothing was appended either ever or since the
						// last call to this ToString method, just return
						// an empty string instead of null
						if(!mRope)
							return JSString::FromString(mGlobalObject, UTF8String::MakeEmpty());

						// reset the current state and return the rope
						// back to the caller
						JSStringRope* t = mRope;
						mRope = mo_null;
						mIndex = 0;

						return t;
					}

					void Append(JSString* str)
					{
						// make sure there is enough slots in the rope
						// for this new string
						Grow();

						// update the fiber slot
						mRope->UpdateFiber(str, mIndex++);
					}

				private:
					void Grow()
					{
						if(!mRope)
						{
							mRope = JSStringRope::Create(mGlobalObject);
							return;
						}

						// no growth is needed
						if(mIndex != JSStringRope::MaximumFiberCount)
							return;

						Append(ToString());
					}

					JSStringRope* mRope;
					JSGlobalObject* mGlobalObject;
					mo_uint32 mIndex;
			};

		public:
			static JSStringRope* Create(JSGlobalObject* globalObject);

			static JSStringRope* FromStrings(JSGlobalObject* globalObject, JSString* a, JSString* b);
			static JSStringRope* FromStrings(JSGlobalObject* globalObject, JSString* a, JSString* b, JSString* c);

		private:
			JSStringRope(JSGlobalObject* globalObject);
			JSStringRope(JSGlobalObject* globalObject, JSString* a, JSString* b);
			JSStringRope(JSGlobalObject* globalObject, JSString* a, JSString* b, JSString* c);

		private:
			JSString* GetCharInRope(mo_unsigned index);

			void Initialize();
			void InitializeFibers(JSString* a, JSString* b, JSString* c = mo_null);
			void Resolve() const;
			void Resolve(const JSString* ropeOrFiber) const;
			void UpdateFiber(JSString* fiber, mo_unsigned index);

			mutable JSString* mFibers[MaximumFiberCount];
	};
}

#endif