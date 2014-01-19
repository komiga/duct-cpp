
#include <duct/utility.hpp>
#include <duct/debug.hpp>
#include <duct/EndianUtils.hpp>

#include <iomanip>
#include <iostream>

template<
	typename FP,
	std::size_t const = sizeof(FP)
>
struct int_fp_type_impl;

template<
	typename FP
>
struct int_fp_type_impl<FP, 4u> {
	using type = std::uint32_t;
};

template<
	typename FP
>
struct int_fp_type_impl<FP, 8u> {
	using type = std::uint64_t;
};

template<
	typename FP
>
using int_fp_type = typename int_fp_type_impl<FP>::type;

constexpr auto
ENSURE_SWAP = (duct::Endian::LITTLE == duct::Endian::SYSTEM)
	? duct::Endian::BIG
	: duct::Endian::LITTLE
;

constexpr std::uint16_t INTEGRAL16 = 0x1122;
constexpr std::uint32_t INTEGRAL32 = 0x11223344;
constexpr std::uint64_t INTEGRAL64 = 0x1122334455667788;
constexpr float FLOAT32 = 1.0f;
constexpr double FLOAT64 = 1.0f;

template<
	typename T
>
void
do_test_integral(
	T const value
) {
	auto x = duct::byte_swap   (value);
	auto y = duct::byte_swap_if(value, ENSURE_SWAP);
	auto xr = value; duct::byte_swap_ref(xr);
	auto yr = value; duct::byte_swap_ref_if(yr, ENSURE_SWAP);
	std::cout
		<< std::hex << std::showbase
		<< "integral byte_swap   : " << value << " -> " << x << '\n'
		<< "integral byte_swap_if: " << value << " -> " << y << '\n'
		<< "integral byte_swap_ref   : " << value << " -> " << xr << '\n'
		<< "integral byte_swap_ref_if: " << value << " -> " << yr
	<< std::endl;
	DUCT_ASSERT(x == y && xr == yr, "something has gone deliciously wrong");
}

template<
	typename T
>
void
do_test_floating(
	T const value
) {
	using cref_type = int_fp_type<T> const&;
	auto const x = duct::byte_swap   (value);
	auto const y = duct::byte_swap_if(value, ENSURE_SWAP);
	auto xr = value; duct::byte_swap_ref   (xr);
	auto yr = value; duct::byte_swap_ref_if(yr, ENSURE_SWAP);
	std::cout
		<< std::hex << std::showbase
		<< "floating-point byte_swap   : "
			<< reinterpret_cast<cref_type>(value)
			<< " -> " << reinterpret_cast<cref_type>(x)
		<< '\n'
		<< "floating-point byte_swap_if: "
			<< reinterpret_cast<cref_type>(value)
			<< " -> "<< reinterpret_cast<cref_type>(y)
		<< '\n'
		<< "floating-point byte_swap_ref   : "
			<< reinterpret_cast<cref_type>(value)
			<< " -> "<< reinterpret_cast<cref_type>(xr)
		<< '\n'
		<< "floating-point byte_swap_ref_if: "
			<< reinterpret_cast<cref_type>(value)
			<< " -> "<< reinterpret_cast<cref_type>(yr)
	<< std::endl;
}

#define DUCT_TEST_COUT_ENDIAN_(n_, e_)						\
	<< n_ " endian: " << enum_cast(duct::Endian:: e_)		\
	<< ((duct::Endian::SYSTEM == duct::Endian:: e_)			\
		? " (system)\n"										\
		: "\n"												\
	)

signed
main() {
	std::cout
		DUCT_TEST_COUT_ENDIAN_("little", LITTLE)
		DUCT_TEST_COUT_ENDIAN_("big   ", BIG)
	;

	auto const ex = INTEGRAL16;
	std::uint8_t const* const
	ex_p = reinterpret_cast<std::uint8_t const*>(&ex);

	std::cout
		<< std::hex
		<< "value: " << ex << " (INTEGRAL16)\n"
		<< "bytes: "
		<< static_cast<unsigned>(ex_p[0])
		<< static_cast<unsigned>(ex_p[1])
		<< '\n'
	;

	duct::Endian endian;
	if (ex_p[0] == (ex & 0xFF)) {
		endian = duct::Endian::LITTLE;
	} else {
		endian = duct::Endian::BIG;
	}

	DUCT_ASSERT(
		duct::Endian::SYSTEM == endian,
		"duct is lying to you"
	);

	do_test_integral(INTEGRAL16);
	do_test_integral(INTEGRAL32);
	do_test_integral(INTEGRAL64);

	do_test_floating(FLOAT32);
	do_test_floating(FLOAT64);
	return 0;
}
