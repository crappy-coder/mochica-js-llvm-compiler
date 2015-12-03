#ifndef MOENJIN_CORE_TEESTREAM_H
#define MOENJIN_CORE_TEESTREAM_H

#include "Platform/Configuration.h"
#include "Core/BasicStreamBuffer.h"

namespace MoEnjin
{
	class TeeStreamBuffer : public UTF8BasicStreamBuf
	{
		typedef std::vector<std::ostream*> StreamCollection;

		public:
			TeeStreamBuffer();
			TeeStreamBuffer(std::istream& istr);
			TeeStreamBuffer(std::ostream& ostr);

			~TeeStreamBuffer();

			void Add(std::ostream& ostr);

		protected:
			mo_int32 Read();
			mo_int32 Write(mo_utf8char c);

		private:	
			std::istream* mInStream;
			StreamCollection mStreams;
	};

	class TeeStreamIOS : public virtual std::ios
	{
		public:
			TeeStreamIOS();
			TeeStreamIOS(std::istream& istr);
			TeeStreamIOS(std::ostream& ostr);

			~TeeStreamIOS();

			void Add(std::ostream& ostr);

			TeeStreamBuffer* rdbuf();

		protected:
			TeeStreamBuffer mBuf;
	};

	class TeeInputStream: public TeeStreamIOS, public std::istream
	{
		public:
			TeeInputStream(std::istream& istr);

			~TeeInputStream();
	};

	class TeeOutputStream: public TeeStreamIOS, public std::ostream
	{
		public:
			TeeOutputStream();
			TeeOutputStream(std::ostream& ostr);

			~TeeOutputStream();
	};
}

#endif