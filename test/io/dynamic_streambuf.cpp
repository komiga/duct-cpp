
#include <duct/IO/dynamic_streambuf.hpp>

#include <cassert>
#include <iostream>

using duct::IO::dynamic_streambuf;
using Sequence = duct::IO::dynamic_streambuf::Sequence;

std::ostream&
operator<<(
	std::ostream& stream,
	Sequence const seq
) {
	switch (seq) {
	case Sequence::input:
		stream << "input";
		break;

	case Sequence::output:
		stream << "output";
		break;
	}
	return stream;
}

template<
	typename CharT,
	typename TraitsT
>
bool
test_stream_states(
	std::basic_ios<CharT, TraitsT> const& ios,
	std::ios_base::iostate const states
) {
	return states == (states & ios.rdstate());
}

template<
	typename CharT,
	typename TraitsT
>
bool
is_fail(
	std::basic_ios<CharT, TraitsT> const& ios
) {
	return test_stream_states(ios, std::ios_base::failbit);
}

void
inspect(
	dynamic_streambuf const& buf,
	std::string const name = "streambuf"
) {
	std::cout
		<< name << ":\n"
		<< "sequence     : " << buf.get_sequence() << '\n'
		<< "sequence_size: " << buf.get_sequence_size() << '\n'

		<< "max_size   : " << buf.get_max_size() << '\n'
		<< "buffer size: " << buf.get_buffer().size() << '\n'
		<< "data       : \""
			<< std::string{
				buf.get_buffer().data(),
				buf.get_buffer().data() + buf.get_sequence_size()
			}
		<< "\"\n"
	;
}

signed
main() {
	// invariants
	{
		dynamic_streambuf buf{0u};
		assert(Sequence::output == buf.get_sequence());
		assert(0u == buf.get_sequence_size());

		inspect(buf, "invariants -> initial");

		buf.commit();
		assert(Sequence::input == buf.get_sequence());
		assert(0u == buf.get_sequence_size());

		buf.reset(10u);
		assert(Sequence::output == buf.get_sequence());
		assert(0u == buf.get_sequence_size());
		assert(10u == buf.get_buffer().size());

		{
			std::ostream os{&buf};
			os << 'a';
			assert(os.good());
			assert(1u == buf.get_sequence_size());
		}

		buf.commit();
		assert(Sequence::input == buf.get_sequence());
		assert(1u == buf.get_sequence_size());

		{
			std::istream is{&buf};
			char c = '\0';
			is >> c;
			assert('a' == c);

			is.read(&c, 1u);
			assert('a' == c);
			assert(is.eof());
			assert(!is.bad());
			assert(is_fail(is));
		}

		buf.reset(10u);
		assert(Sequence::output == buf.get_sequence());
		assert(0u == buf.get_sequence_size());

		inspect(buf, "invariants -> final");
	}

	// seek & overflow
	{
		std::cout << '\n';
		dynamic_streambuf buf{
			0u,
			1u,
			0u
		};

		inspect(buf, "seek & overflow -> initial");

		{
			std::ostream os{&buf};
			os.write("abcd", 4u);
			assert(4u == buf.get_sequence_size());
			assert(4u == buf.get_buffer().size());

			inspect(buf, "seek & overflow -> after write");

			os.seekp(-1, std::ios_base::end);
			assert(!os.fail());
			assert(3 == os.tellp());
			os << 'D';

			os.seekp(-2, std::ios_base::cur);
			assert(!os.fail());
			assert(2 == os.tellp());
			os << 'C';

			os.seekp(1, std::ios_base::beg);
			assert(!os.fail());
			assert(1 == os.tellp());
			os << 'B';

			os.seekp(0, std::ios_base::beg);
			assert(!os.fail());
			assert(0 == os.tellp());
			os << 'A';

			inspect(buf, "seek & overflow -> after seek-overwrite");

			// FIXME: libc++ (svn) defect: seekp() appears to truncate
			// an invalid offset before passing to the implementation
			// *without* setting failbit.
			os.clear();
			os.seekp(-1, std::ios_base::beg);
			assert(is_fail(os));

			os.clear();
			os.seekp(1, std::ios_base::end);
			assert(os.good());

			// FIXME: ^
			os.clear();
			os.seekp(-6, std::ios_base::cur);
			assert(is_fail(os));

			inspect(buf, "seek & overflow -> after seeks");

			// overflow from seek
			assert(5u == buf.get_sequence_size());

			//   4 (size after write)
			// + 1 (overflow from seek)
			// + 1 (growth rate)
			assert(6u == buf.get_buffer().size());
		}

		{
			std::string const match{"ABCD"};
			char read[4u]{};

			buf.commit(4u);
			std::istream is{&buf};
			is.read(read, 4u);
			assert(0 == match.compare(0u, 4u, read, 4u));
			assert(is.good());

			is.read(read, 1u);
			assert(is.eof());
			assert(!is.bad());
			assert(is_fail(is));
		}

		inspect(buf, "seek & overflow -> after read");
	}

	return 0;
}
