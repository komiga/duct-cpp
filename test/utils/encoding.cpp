
#include <duct/config.hpp>
#include <duct/debug.hpp>
#include <duct/EncodingUtils.hpp>

#include <iterator>

using E = duct::UTF8Utils;

template<
	std::size_t const N
>
constexpr std::size_t
len(
	char const (&)[N]
) noexcept {
	return N;
}

signed
main() {
	char const str[]{'a', 'b', 'c'};
	auto const
		begin = std::begin(str),
		end = begin + len(str)
	;
	auto step = begin, from = step;
	DUCT_ASSERTE(from < E::next(from, end));

	std::size_t count = 0u;
	while ((step = E::next(from, end)) > from) {
		++count;
		from = step;
	}
	DUCT_ASSERTE(3u == count);

	step = E::prev(end - 1, begin);
	DUCT_ASSERTE(end - 1 != step);

	DUCT_ASSERTE(!DUCT_UTF8_IS_LEAD  ('a'));
	DUCT_ASSERTE( DUCT_UTF8_IS_SINGLE('a'));
	DUCT_ASSERTE( DUCT_UTF8_IS_HEAD  ('a'));

	return 0;
}
