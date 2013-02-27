
#include <duct/debug.hpp>
#include <duct/EndianUtils.hpp>

#include <iomanip>
#include <iostream>

template<typename FP, std::size_t const s=sizeof(FP)>
struct int_fp_type_impl;
template<typename FP>
struct int_fp_type_impl<FP, 4> { typedef uint32_t type; };
template<typename FP>
struct int_fp_type_impl<FP, 8> { typedef uint64_t type; };

template<typename FP>
using int_fp_type=typename int_fp_type_impl<FP>::type;

auto constexpr ENSURE_SWAP
	=(duct::Endian::LITTLE==duct::Endian::SYSTEM)
		? duct::Endian::BIG
		: duct::Endian::LITTLE
;
constexpr uint16_t INTEGRAL16=0x1122;
constexpr uint32_t INTEGRAL32=0x11223344;
constexpr uint64_t INTEGRAL64=0x1122334455667788;
constexpr float FLOAT32=1.0f;
constexpr double FLOAT64=1.0f;

template<typename T>
void do_test_integral(T const value) {
	auto x=duct::byte_swap   (value);
	auto y=duct::byte_swap_if(value, ENSURE_SWAP);
	auto xr=value; duct::byte_swap_ref(xr);
	auto yr=value; duct::byte_swap_ref_if(yr, ENSURE_SWAP);
	std::cout<<std::hex<<std::showbase
		<<"integral byte_swap   : "<<value<<" -> "<<x<<std::endl
		<<"integral byte_swap_if: "<<value<<" -> "<<y<<std::endl
		<<"integral byte_swap_ref   : "<<value<<" -> "<<xr<<std::endl
		<<"integral byte_swap_ref_if: "<<value<<" -> "<<yr<<std::endl
	;
	DUCT_ASSERT(x==y && xr==yr, "something has gone deliciously wrong");
}

template<typename T>
void do_test_floating(T const value) {
	using cref_type=int_fp_type<T> const&;
	auto x=duct::byte_swap   (value);
	auto y=duct::byte_swap_if(value, ENSURE_SWAP);
	auto xr=value; duct::byte_swap_ref   (xr);
	auto yr=value; duct::byte_swap_ref_if(yr, ENSURE_SWAP);
	std::cout<<std::hex<<std::showbase
		<<"floating-point byte_swap   : "
			<<reinterpret_cast<cref_type>(value)
			<<" -> "<<reinterpret_cast<cref_type>(x)
		<<std::endl
		<<"floating-point byte_swap_if: "
			<<reinterpret_cast<cref_type>(value)
			<<" -> "<<reinterpret_cast<cref_type>(y)
		<<std::endl
		<<"floating-point byte_swap   : "
			<<reinterpret_cast<cref_type>(value)
			<<" -> "<<reinterpret_cast<cref_type>(xr)
		<<std::endl
		<<"floating-point byte_swap_if: "
			<<reinterpret_cast<cref_type>(value)
			<<" -> "<<reinterpret_cast<cref_type>(yr)
		<<std::endl
	;
}

signed main() {
	do_test_integral(INTEGRAL16);
	do_test_integral(INTEGRAL32);
	do_test_integral(INTEGRAL64);

	do_test_floating(FLOAT32);
	do_test_floating(FLOAT64);
	return 0;
}
