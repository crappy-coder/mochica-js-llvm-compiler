#ifndef MOENJIN_CORE_BASICSTREAMBUFFER_H
#define MOENJIN_CORE_BASICSTREAMBUFFER_H

#include "Platform/Configuration.h"

namespace MoEnjin
{
	template <typename ChType, typename ChTraits>
	class BasicStreamBuffer : public std::basic_streambuf<ChType, ChTraits>
	{
		MO_NON_COPYABLE_CLASS(BasicStreamBuffer);

		protected:
			typedef std::basic_streambuf<ChType, ChTraits> base_type;
			typedef std::basic_ios<ChType, ChTraits> ios_type;
			typedef ChType char_type;
			typedef ChTraits char_type_traits;
			typedef typename base_type::int_type int_type;
			typedef typename base_type::pos_type pos_type;
			typedef typename base_type::off_type off_type;
			typedef typename ios_type::openmode openmode;

			static int_type ToInt(char_type ch) { return char_type_traits::to_int_type(ch); }

		public:
			BasicStreamBuffer()
				: mPB(char_type_traits::eof())
				, mIsPB(mo_false)
			{
				setg(0, 0, 0);
				setp(0, 0);
			}

			~BasicStreamBuffer() { }

			virtual int_type overflow(int_type c)
			{
				if (c != char_type_traits::eof()) 
					return Write(char_type_traits::to_char_type(c));

				return c;
			}

			virtual int_type underflow()
			{
				if (mIsPB)
    				return mPB;

    			int_type c = Read();

    			if (c != char_type_traits::eof())
    			{
    				mIsPB = true;
    				mPB   = c;
    			}

    			return c;
			}

			virtual int_type uflow()
			{
				if (mIsPB)
				{
					mIsPB = false;
					return mPB;
				}

    			int_type c = Read();

    			if (c != char_type_traits::eof())
    				mPB = c;

    			return c;
			}

			virtual int_type pbackfail(int_type c)
			{
				if (mIsPB)
    				return char_type_traits::eof();

    			mIsPB = true;
    			mPB   = c;

    			return c;
			}
	
			virtual std::streamsize xsgetn(char_type* p, std::streamsize count)
			{
				std::streamsize copied = 0;

				while (count > 0)
				{
					int_type c = uflow();

					if (c == char_type_traits::eof()) 
						break;

					*p++ = char_type_traits::to_char_type(c);
					++copied;
					--count;
				}

				return copied;
			}

		private:
			virtual int_type Read() { return char_type_traits::eof(); }
			virtual int_type Write(char_type) { return char_type_traits::eof(); }

			int_type mPB;
			mo_bool mIsPB;
	};

	typedef BasicStreamBuffer<mo_utf8char, std::char_traits<mo_utf8char> > UTF8BasicStreamBuf;
}

#endif