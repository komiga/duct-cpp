/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief const-correct unique_ptr.
*/

#pragma once

#include <duct/config.hpp>

#include <type_traits>
#include <utility>
#include <memory>

namespace duct {

// Forward declarations
template<class T_, class D_ = std::default_delete<T_>>
class cc_unique_ptr;

/**
	@addtogroup utils
	@{
*/

/**
	const-correct @c std::unique_ptr.

	This smart pointer is exactly the same as @c std::unique_ptr,
	except that all const property observers return the
	object-immutable <code>element_type const</code> instead of the
	object-mutable <code>element_type</code> (in pointer-to or
	reference-to forms).

	This prevents a client from modifying an object wrapped in a
	@c cc_unique_ptr if it was acquired from an immutable context.

	@note See the C++ Standard or a C++ reference for documentation
	on @c std::unique_ptr.

	@par
	@note The same free-function @c operator@()s defined
	for @c std::unique_ptr are defined for @c cc_unique_ptr, but
	hidden from Doxygen to avoid clutter.
*/
template<class T_, class D_>
class cc_unique_ptr final {
private:
	using base = std::unique_ptr<T_, D_>;

public:
	using pointer = typename base::pointer;
	using element_type = typename base::element_type;
	using deleter_type = typename base::deleter_type;

	using const_pointer = element_type const*;
	using reference = typename std::add_lvalue_reference<
		element_type
	>::type;
	using const_reference = typename std::add_lvalue_reference<
		element_type const
	>::type;

private:
	using d1ctor_deleter_type
	= typename std::conditional<
		std::is_reference<deleter_type>::value,
		deleter_type,
		const deleter_type&
	>::type;

	base m_base;

public:
// destructor
	// NB: The Standard specifies no noexcept, which suggests the
	// unique_ptr implementation is allowed to throw.
	// But it also says:
	//   The expression get_deleter()(get()) shall be well formed,
	//   shall have well-defined behavior, and shall not throw
	//   exceptions.
	// libc++ concurs with the specification, but libstdc++ does not.
	~cc_unique_ptr() noexcept(noexcept(m_base.~base())) = default;

// constructors
	constexpr
	cc_unique_ptr() noexcept = default;

	constexpr
	cc_unique_ptr(
		std::nullptr_t
	) noexcept
		: m_base(nullptr)
	{}

	explicit
	cc_unique_ptr(
		pointer ptr
	) noexcept
		: m_base(std::move(ptr))
	{}

	cc_unique_ptr(
		pointer ptr,
		d1ctor_deleter_type deleter
	) noexcept
		: m_base(std::move(ptr), std::forward<d1ctor_deleter_type>(deleter))
	{}

	cc_unique_ptr(
		pointer ptr,
		typename std::remove_reference<deleter_type>::type&& deleter
	) noexcept
		: m_base(std::move(ptr), std::move(deleter))
	{}

	cc_unique_ptr(cc_unique_ptr const&) = delete;

	cc_unique_ptr(
		cc_unique_ptr&& upc
	) noexcept
		: m_base(std::move(upc.m_base))
	{}

	template<class U_, class E_>
	cc_unique_ptr(
		cc_unique_ptr<U_, E_>&& up
	) noexcept
		: m_base(std::forward<std::unique_ptr<U_, E_>>(up))
	{}

// assignment
	cc_unique_ptr& operator=(cc_unique_ptr const&) = delete;
	cc_unique_ptr& operator=(cc_unique_ptr&&) noexcept = default;

	template<class U_, class E_>
	cc_unique_ptr&
	operator=(
		cc_unique_ptr<U_, E_>&& up
	) noexcept {
		m_base.operator=(std::forward<std::unique_ptr<U_, E_>>(up));
		return *this;
	}

	cc_unique_ptr&
	operator=(
		std::nullptr_t
	) noexcept {
		m_base.operator=(nullptr);
		return *this;
	}

// observers
	pointer
	get() noexcept {
		return m_base.get();
	}

	const_pointer
	get() const noexcept {
		return m_base.get();
	}

	deleter_type&
	get_deleter() noexcept {
		return m_base.get_deleter();
	}

	deleter_type const&
	get_deleter() const noexcept {
		return m_base.get_deleter();
	}

	reference
	operator*() {
		return m_base.operator*();
	}

	const_reference
	operator*() const {
		return m_base.operator*();
	}

	pointer
	operator->() noexcept {
		return m_base.operator->();
	}

	const_pointer
	operator->() const noexcept {
		return m_base.operator->();
	}

	explicit
	operator bool() const noexcept {
		return m_base.operator bool();
	}

// modifiers
	pointer
	release() noexcept {
		return m_base.release();
	}

	void
	reset(
		pointer ptr = pointer()
	) noexcept {
		m_base.reset(std::forward<pointer>(ptr));
	}

	void
	swap(
		cc_unique_ptr& other
	) noexcept {
		m_base.swap(other.m_base);
	}
};

// specialized algorithms

// NB: The Standard does not give T1-T2 comparison operators noexcept,
// but get() is specified as having noexcept, so all of these
// operators do _not_ follow the standard (because they have
// noexcept).

/** @cond INTERNAL */
// operator==

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator==(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	return x_.get() == y_.get();
}

template<class T_, class D_>
inline bool
operator==(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return !x_;
}

template<class T_, class D_>
inline bool
operator==(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return !x_;
}

// operator!=

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator!=(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	return x_.get() != y_.get();
}

template<class T_, class D_>
inline bool
operator!=(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return static_cast<bool>(x_);
}

template<class T_, class D_>
inline bool
operator!=(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return static_cast<bool>(x_);
}

// operator<

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator<(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	using _CT = typename std::common_type<
		typename cc_unique_ptr<T1_, D1_>::pointer,
		typename cc_unique_ptr<T2_, D2_>::pointer
	>::type;
	return std::less<_CT>(x_.get(), y_.get());
}

template<class T_, class D_>
inline bool
operator<(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return std::less<typename cc_unique_ptr<T_, D_>::pointer>(
		x_.get(), nullptr
	);
}

template<class T_, class D_>
inline bool
operator<(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return std::less<typename cc_unique_ptr<T_, D_>::pointer>(
		nullptr, x_.get()
	);
}

// operator<=

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator<=(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	return !(y_ < x_);
}

template<class T_, class D_>
inline bool
operator<=(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return !(nullptr < x_);
}

template<class T_, class D_>
inline bool
operator<=(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return !(x_ < nullptr);
}

// operator>

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator>(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	return y_ < x_;
}

template<class T_, class D_>
inline bool
operator>(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return nullptr < x_;
}

template<class T_, class D_>
inline bool
operator>(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return x_ < nullptr;
}

// operator>=

template<class T1_, class D1_, class T2_, class D2_>
inline bool
operator>=(
	cc_unique_ptr<T1_, D1_> const& x_,
	cc_unique_ptr<T2_, D2_> const& y_
) noexcept {
	return !(x_ < y_);
}

template<class T_, class D_>
inline bool
operator>=(
	cc_unique_ptr<T_, D_> const& x_,
	std::nullptr_t
) noexcept {
	return !(x_ < nullptr);
}

template<class T_, class D_>
inline bool
operator>=(
	std::nullptr_t,
	cc_unique_ptr<T_, D_> const& x_
) noexcept {
	return !(nullptr < x_);
}
/** @endcond */ // INTERNAL (shoo, Doxygen, shoo)

/** @} */ // end of doc-group utils

} // namespace duct
