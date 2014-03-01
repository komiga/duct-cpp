
#include <duct/debug.hpp>
#include <duct/utility.hpp>
#include <duct/StateStore.hpp>

#include <bitset>
#include <iostream>

enum class CS : std::uint8_t {
	a = duct::bit(0),
	b = duct::bit(1),
	c = duct::bit(2),

	ab = a | b,
	ac = a | c,
	all = a | b | c,
	none = 0
};

using ClassStore = duct::StateStore<CS>;
using value_type = ClassStore::value_type;

template<
	typename S,
	typename V
>
void
print(
	duct::StateStore<S, V> const& state_store
) {
	std::bitset<8> const bits{state_store.get_value()};
	std::cout
		<< "bits: " << bits
	<< '\n';
}

signed
main() {
	{
		ClassStore ss{};
		value_type const default_value = ss.get_value();
		print(ss);
		DUCT_ASSERTE(default_value == value_type(0));

		ss.enable(CS::a);
		print(ss);
		DUCT_ASSERTE(ss.test(CS::a));
		DUCT_ASSERTE(ss.test_any(CS::a));
		DUCT_ASSERTE(ss.test_any(CS::all));

		ss.clear();
		DUCT_ASSERTE(ss.get_value() == default_value);
	}
	{
		ClassStore ss{
			CS::a, CS::b, CS::c
		};
		print(ss);
		DUCT_ASSERTE(
			static_cast<value_type const>(CS::all) == ss.get_value() &&
			ss.test_any(CS::a) &&
			ss.test_any(CS::b) &&
			ss.test_any(CS::c) &&
			ss.test_any(CS::ab) &&
			ss.test_any(CS::ac) &&
			ss.test_any(CS::all) &&
			CS::all == ss.get_states(CS::all)
		);

		ss.remove(CS::ab);
		print(ss);
		DUCT_ASSERTE(
			static_cast<value_type const>(CS::c) == ss.get_value() &&
			ss.test_any(CS::c) &&
			ss.test_any(CS::all) &&
			CS::c == ss.get_states(CS::all) &&
			CS::none == ss.get_states(CS::ab)
		);

		ss.enable(CS::all);
		ss.set_masked(CS::ab, CS::a);
		print(ss);
		DUCT_ASSERTE(
			static_cast<value_type const>(CS::ac) == ss.get_value() &&
			ss.test_any(CS::a) &&
			ss.test_any(CS::c) &&
			ss.test_any(CS::ac) &&
			ss.test_any(CS::all) &&
			CS::ac == ss.get_states(CS::ac) &&
			CS::none == ss.get_states(CS::b)
		);

		ss.set(CS::c, false);
		print(ss);
		DUCT_ASSERTE(
			ss.test(CS::a) &&
			!ss.test(CS::b) &&
			!ss.test(CS::c) &&
			ss.test_any(CS::a) &&
			!ss.test_any(CS::b) &&
			!ss.test_any(CS::c)
		);
	}
	{
		constexpr ClassStore /*const*/ ss_d{};
		constexpr ClassStore /*const*/ ss_v{CS::a, CS::b};
		constexpr value_type /*const*/ v = ss_d.get_value();
		constexpr bool /*const*/ h = ss_v.test(CS::a);
		(void)(v && h);
	}
	std::cout.flush();
	return 0;
}
