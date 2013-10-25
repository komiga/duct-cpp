
#include <duct/debug.hpp>
#include <duct/cc_unique_ptr.hpp>

#include <utility>

using U = duct::cc_unique_ptr<int>;

void
destroy(
	U::pointer const p
) {
	DUCT_ASSERTE(nullptr != p);
	delete p;
}

signed
main() {
	U u1{new int(42)};
	DUCT_ASSERTE(nullptr != u1.get());
	DUCT_ASSERTE(true == static_cast<bool>(u1));
	DUCT_ASSERTE(42 == *u1);

	u1.reset();
	DUCT_ASSERTE(nullptr == u1.get());
	DUCT_ASSERTE(false == static_cast<bool>(u1));

	u1.reset(new int(3));
	DUCT_ASSERTE(nullptr != u1.get());
	DUCT_ASSERTE(true == static_cast<bool>(u1));
	DUCT_ASSERTE(3 == *u1);

	U u2{std::move(u1)};
	DUCT_ASSERTE(nullptr == u1.get());
	DUCT_ASSERTE(false == static_cast<bool>(u1));

	DUCT_ASSERTE(nullptr != u2.get());
	DUCT_ASSERTE(true == static_cast<bool>(u2));
	DUCT_ASSERTE(3 == *u2);

	destroy(u2.release());
	DUCT_ASSERTE(nullptr == u2.get());
	DUCT_ASSERTE(false == static_cast<bool>(u2));

	return 0;
}
