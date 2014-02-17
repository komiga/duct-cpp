/**
@file Template.hpp
@brief Variable validation.

@author Tim Howard
@copyright 2010-2013 Tim Howard under the MIT license;
see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: infinitism

#ifndef DUCT_TEMPLATE_HPP_
#define DUCT_TEMPLATE_HPP_

#include "./config.hpp"
#include "./aux.hpp"
#include "./string.hpp"
#include "./StateStore.hpp"
#include "./Variable.hpp"

#include <utility>

namespace duct {

/**
	@addtogroup variable
	@{
*/

/**
	Variable validator.
*/
class Template {
public:
/** @name Types */ /// @{
	/**
		Layout field.
	*/
	struct Field {
	/** @name Types */ /// @{
		/**
			Field flags.

			@note See Template::validate_layout() for usage notes.
		*/
		enum class Flags : unsigned {
			/** No flags. */
			none = 0,

			/** Optional field. */
			optional = 1 << 0,
		};
	/// @}

	/** @name Properties */ /// @{
		/** Type mask. */
		VarMask mask;

		/** Flags. */
		StateStore<Flags> flags;

		/**
			Check if Flags::optional is enabled.
		*/
		bool
		optional() const noexcept {
			return flags.test(Flags::optional);
		}
	/// @}

	/** @name Constructors and destructor */ /// @{
		/** Destructor. */
		~Field() = default;
		/** Copy constructor. */
		Field(Field const&) = default;
		/** Move constructor. */
		Field(Field&&) = default;

		/**
			Construct with flags.

			@note Mask is VarMask::none.

			@param flags %Flags.
		*/
		Field(
			Flags const flags = Flags::none
		) noexcept
			: mask(VarMask::none)
			, flags(flags)
		{}

		/**
			Construct with type and flags.

			@param type Single-type mask.
			@param flags %Flags.
		*/
		Field(
			VarType const type,
			Flags const flags = Flags::none
		) noexcept
			: mask(var_mask(type))
			, flags(flags)
		{}

		/**
			Construct with mask and flags.

			@param mask Mask.
			@param flags %Flags.
		*/
		Field(
			VarMask const mask,
			Flags const flags = Flags::none
		) noexcept
			: mask(mask)
			, flags(flags)
		{}
	/// @}

	/** @name Operators */ /// @{
		/** Copy assignment operator. */
		Field& operator=(Field const&) = default;
		/** Move assignment operator. */
		Field& operator=(Field&&) = default;
	/// @}
	};

	/** Identity vector type. */
	using identity_vector_type = duct::aux::vector<u8string>;
	/** Layout vector type. */
	using layout_vector_type = duct::aux::vector<Field>;
/// @}

protected:
	/**
		%Flags.
	*/
	enum class Flags {
		/**
			Whether to permit empty collections in layout validation.
		*/
		permit_empty = 1 << 0
	};

	StateStore<Flags> m_flags{Flags::permit_empty};
	VarMask m_type_mask{VarMask::none}; /**< Type mask. */
	identity_vector_type m_identity{}; /**< Identity. */
	layout_vector_type m_layout{}; /**< Layout. */

public:
/** @name Constructors and destructor */ /// @{
	/**
		Construct with @c VarMask::none type mask, empty identity,
		and empty layout.
	*/
	Template() = default;

	/**
		Construct with type mask, empty identity, and empty layout.

		@param type_mask Type mask.
	*/
	explicit
	Template(
		VarMask const type_mask
	)
		: m_type_mask(type_mask)
	{}

	/**
		Construct with type mask, identity, and empty layout.

		@param type_mask Type mask.
		@param identity Identity.
	*/
	Template(
		VarMask const type_mask,
		identity_vector_type identity
	)
		: m_type_mask(type_mask)
		, m_identity(std::move(identity))
	{}

	/**
		Construct with type mask, layout, and empty identity.

		@param type_mask Type mask.
		@param layout Layout.
	*/
	Template(
		VarMask const type_mask,
		layout_vector_type layout
	)
		: m_type_mask(type_mask)
		, m_layout(std::move(layout))
	{}

	/**
		Construct with type mask, identity, and layout.

		@param type_mask Type mask.
		@param identity Identity.
		@param layout Layout.
	*/
	Template(
		VarMask const type_mask,
		identity_vector_type identity,
		layout_vector_type layout
	)
		: m_type_mask(type_mask)
		, m_identity(std::move(identity))
		, m_layout(std::move(layout))
	{}

	/** Copy constructor (deleted). */
	Template(Template const&) = delete;
	/** Move constructor. */
	Template(Template&&) = default;
	/** Destructor. */
	virtual ~Template() = default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	Template& operator=(Template const&) = delete;
	/** Move assignment operator. */
	Template& operator=(Template&&) = default;
/// @}

/** @name Properties */ /// @{
	/**
		Enable or disable flags.

		@param flags %Flags.
		@param enable Whether to enable or disable the flags.
	*/
	void
	set_flags(
		Flags const flags,
		bool const enable
	) noexcept {
		m_flags.set(flags, enable);
	}

	/**
		Check if Flags::permit_empty is enabled.
	*/
	bool
	permit_empty() const noexcept {
		return m_flags.test(Flags::permit_empty);
	}

	/**
		Set type mask.

		@param type_mask New type mask.
		@sa validate_type(Variable const&) const
	*/
	void
	set_type_mask(
		VarMask const type_mask
	) noexcept {
		m_type_mask = type_mask;
	}

	/**
		Set type mask (single type).

		@param type New type mask.
		@sa validate_type(Variable const&) const
	*/
	void
	set_type_mask(
		VarType const type
	) noexcept {
		m_type_mask = var_mask(type);
	}

	/**
		Get type mask.

		@returns The current type mask.
		@sa validate_type(Variable const&) const
	*/
	VarMask
	get_type_mask() const noexcept {
		return m_type_mask;
	}

	/**
		Set identity.

		@param identity New identity.
		@sa validate_identity(Variable const&) const
	*/
	void
	set_identity(
		identity_vector_type identity
	) {
		m_identity = std::move(identity);
	}

	/**
		Get identity.

		@returns The current identity.
		@sa validate_identity(Variable const&) const
	*/
	identity_vector_type&
	get_identity() noexcept {
		return m_identity;
	}

	/** @copydoc get_identity() */
	identity_vector_type const&
	get_identity() const noexcept {
		return m_identity;
	}

	/**
		Set layout.

		@param layout New layout.
		@sa validate_layout(Variable const&) const
	*/
	void
	set_layout(
		layout_vector_type layout
	) {
		m_layout = std::move(layout);
	}

	/**
		Get layout.

		@returns The current layout.
		@sa validate_layout(Variable const&) const
	*/
	layout_vector_type&
	get_layout() noexcept {
		return m_layout;
	}

	/** @copydoc get_layout() */
	layout_vector_type const&
	get_layout() const noexcept {
		return m_layout;
	}

/// @}

/** @name Validation */ /// @{
	/**
		Validate a variable.

		@note If @c validate_type(var) is @c true, layout validation
		is only done if @a var is a collection.

		@returns @c true iff @a var matches template in type, identity,
		and layout.
		@param var Variable to validate.
		@sa validate_type(Variable const&) const,
			validate_identity(Variable const&) const,
			validate_layout(Variable const&) const
	*/
	bool
	validate(
		Variable const& var
	) const noexcept {
		return
			validate_type(var) &&
			validate_identity(var) &&
			(!var.is_type_of(VarMask::collection) || validate_layout(var))
		;
	}

	/**
		Validate a variable by type.

		@returns @c true iff bitwise-and of @a var type and template
		type mask is non-zero.
		@param var Variable to validate.
	*/
	virtual bool
	validate_type(
		Variable const& var
	) const noexcept {
		return var_type_is_of(var.get_type(), m_type_mask);
	}

	/**
		Validate a variable by identity.

		@returns @c true iff:
		-# identity is empty (permits any name),
		-# variable name matches any name from identity
		   (including empty name).
		@param var Variable to validate.
	*/
	virtual bool
	validate_identity(
		Variable const& var
	) const noexcept {
		if (m_identity.empty()) {
			// Any name is permitted with empty identity
			return true;
		} else {
			for (auto const& iname : get_identity()) {
				if (var.get_name() == iname) {
					// Variable name matches a name from identity
					return true;
				}
			}
			// Variable name does not match a name from identity
			return false;
		}
	}

	/**
		Validate a variable by layout.

		@note The @c Field::Flags::optional flag will cause all succeeding
		fields to be considered optional.
		@note The @c Field::Flags::empty flag is only considered when layout
		contains a single field.

		@returns
		- @c false iff:
			-# variable is not a @c VarMask::collection,
			-# variable has more children than layout;
		- @c true iff:
			-# layout is empty (permits any collection),
			-# layout contains a single field with flag
			   @c Field::Flags::empty and variable has no children,
			-# children sequentially match layout fields exactly,
			-# children sequentially match [0..var.size()] layout fields
			   if a field from [0..var.size()+1] is optional
			   (thus making all subsequent fields optional).
		@param var Variable to validate.
	*/
	virtual bool
	validate_layout(
		Variable const& var
	) const noexcept {
		if (var.is_type_of(VarMask::collection)) {
			if (m_layout.empty()) {
				// Any collection is permitted with empty layout
				return permit_empty();
			} else if (var.size() > get_layout().size()) {
				// Collection cannot be larger than layout
				return false;
			} else {
				auto vc_iter = var.cbegin();
				auto lo_iter = get_layout().cbegin();
				bool optional_met = false;
				for (; var.cend() != vc_iter; ++vc_iter, ++lo_iter) {
					if (!vc_iter->is_type_of(lo_iter->mask)) {
						// Child type does not match field
						return false;
					} else if (!optional_met) {
						optional_met = lo_iter->optional();
					}
				}
				if (optional_met || get_layout().cend() == lo_iter) {
					// Collection sequentially matches layout fields
					// exactly or any trailing fields are optional
					return true;
				} else {
					// No optional field has been met, and there are
					// unchecked trailing field(s)
					if (lo_iter->optional()) {
						// First trailing field is optional (therefore
						// all following fields are optional)
						return true;
					} else {
						// First trailing field is not optional
						return false;
					}
				}
			}
		} else {
			// Variable is not a collection
			return false;
		}
	}
/// @}
};

/** @} */ // end of doc-group variable

} // namespace duct

#endif // DUCT_TEMPLATE_HPP_
