#ifndef MOENJIN_CORE_HASH_H
#define MOENJIN_CORE_HASH_H

#ifndef MO_EXCLUDE_COMMON_HEADERS
#define MO_EXCLUDE_COMMON_HEADERS
#define UNDEF_EXCLUSION
#endif

#include "Platform/Configuration.h"

#ifdef UNDEF_EXCLUSION
#undef UNDEF_EXCLUSION
#undef MO_EXCLUDE_COMMON_HEADERS
#endif

namespace MoEnjin
{
	template<mo_uint32 sz> struct IntHashTypes;
	template<> struct IntHashTypes<1> { typedef mo_int8 SType; typedef mo_uint8 UType; };
	template<> struct IntHashTypes<2> { typedef mo_int16 SType; typedef mo_uint16 UType; };
	template<> struct IntHashTypes<4> { typedef mo_int32 SType; typedef mo_uint32 UType; };
	template<> struct IntHashTypes<8> { typedef mo_int64 SType; typedef mo_uint64 UType; };

	class Hash
	{
		MO_STATIC_CLASS(Hash);

		public:
			static mo_uint32 HashString(const mo_utf8stringptr str, mo_int32 len);
			static mo_unsigned HashDouble(mo_double value);

			template<typename T> 
			static mo_unsigned HashInt(T value)
			{
				return IntHash(mo_cast<typename IntHashTypes<sizeof(T)>::UType>(value));
			}

			template<typename T> 
			static mo_unsigned HashFloat(T value)
			{
				union {
					T key;
					typename IntHashTypes<sizeof(T)>::UType bits;
				} u;
				u.key = value;
				return IntHash(u.bits);
			}

		private:
			// Thomas Wang's 32 Bit Mix Function: http://www.cris.com/~Ttwang/tech/inthash.htm
			static mo_inline mo_unsigned IntHash(mo_uint8 i)
			{
				unsigned k = i;

				k += ~(k << 15);
				k ^=  (k >> 10);
				k +=  (k <<  3);
				k ^=  (k >>  6);
				k += ~(k << 11);
				k ^=  (k >> 16);

				return k;
			}

			static mo_inline mo_unsigned IntHash(mo_uint16 i)
			{
				unsigned k = i;

				k += ~(k << 15);
				k ^=  (k >> 10);
				k +=  (k <<  3);
				k ^=  (k >>  6);
				k += ~(k << 11);
				k ^=  (k >> 16);

				return k;
			}

			static mo_inline mo_unsigned IntHash(mo_uint32 i)
			{
				i += ~(i << 15);
				i ^=  (i >> 10);
				i +=  (i <<  3);
				i ^=  (i >>  6);
				i += ~(i << 11);
				i ^=  (i >> 16);

				return i;
			}

			static mo_inline mo_unsigned IntHash(mo_uint64 i)
			{
				i += ~(i << 32);
				i ^=  (i >> 22);
				i += ~(i << 13);
				i ^=  (i >>  8);
				i +=  (i <<  3);
				i ^=  (i >> 15);
				i += ~(i << 27);
				i ^=  (i >> 31);

				return static_cast<unsigned>(i);
			}

			//
			// SuperFastHash
			// © Copyright 2004-2008 by Paul Hsieh 
			//
			// http://www.azillionmonkeys.com/qed/hash.html
			//
			#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__TURBOC__)
			#define get16bits(d) (*((const mo_uint16 *) (d)))
			#endif

			#if !defined(get16bits)
			#define get16bits(d) ((((mo_uint32)(((const mo_uint8 *)(d))[1])) << 8) + (mo_uint32)(((const mo_uint8 *)(d))[0]) )
			#endif
			static mo_uint32 SuperFastHash(const mo_utf8stringptr data, int len) {
				mo_uint32 hash = len, tmp;
				int rem;

				if (len <= 0 || data == NULL) return 0;

				rem = len & 3;
				len >>= 2;

				/* Main loop */
				for (;len > 0; len--) {
					hash  += get16bits (data);
					tmp    = (get16bits (data+2) << 11) ^ hash;
					hash   = (hash << 16) ^ tmp;
					data  += 2*sizeof (mo_uint16);
					hash  += hash >> 11;
				}

				/* Handle end cases */
				switch (rem) {
					case 3: hash += get16bits (data);
							hash ^= hash << 16;
							hash ^= ((signed char)data[sizeof (mo_uint16)]) << 18;
							hash += hash >> 11;
							break;
					case 2: hash += get16bits (data);
							hash ^= hash << 11;
							hash += hash >> 17;
							break;
					case 1: hash += (signed char)*data;
							hash ^= hash << 10;
							hash += hash >> 1;
				}

				/* Force "avalanching" of final 127 bits */
				hash ^= hash << 3;
				hash += hash >> 5;
				hash ^= hash << 4;
				hash += hash >> 17;
				hash ^= hash << 25;
				hash += hash >> 6;

				return hash;
			}
	};
}

#endif