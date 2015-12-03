#ifndef MOENJIN_PLATFORM_STACKINFO_H
#define MOENJIN_PLATFORM_STACKINFO_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	class StackInfo
	{
		static const mo_uint32 AvailableGrowthSize = 4096;

		typedef void* StackPtr;

		public:
			enum GrowDirection
			{ 
				kGrowDirectionDown, 
				kGrowDirectionUp 
			};

			static StackInfo GetInfo()
			{
				StackInfo info;
				info.Initialize();

				return info;
			}

		public:
			StackInfo()
				: mOrigin(0), mSize(0) { }

			mo_int32 GetSize() const { return mSize; }
			mo_int32 GetMaxSize() const { return mSize; }
			StackPtr GetOrigin() const { return mOrigin; }
			StackPtr GetBounds() const { return mBounds; }
			StackPtr GetPosition() const { StackPtr p = &p; return p; }
			GrowDirection GetGrowthDirection() const;
			mo_bool CanGrow(mo_int32 minSize = AvailableGrowthSize) const;

		private:
			void Initialize();
			StackPtr GetGrowthLimit(mo_int32 minSize) const
			{
				return mo_cast<mo_utf8char*>(mBounds) + 
					(GetGrowthDirection() == kGrowDirectionDown ? minSize : -minSize);
			}

			StackPtr mOrigin;
			StackPtr mBounds;
			mo_uint32 mSize;
	};
}

#endif