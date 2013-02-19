/**
@file Template.hpp
@brief Variable validation.

@author Tim Howard
@copyright 2010-2012 Tim Howard under the MIT license; see @ref index or the accompanying LICENSE file for full text.
*/

// TODO: infinitism

#ifndef DUCT_TEMPLATE_HPP_
#define DUCT_TEMPLATE_HPP_

#include "./config.hpp"
#include "./aux.hpp"
#include "./string.hpp"
#include "./Variable.hpp"

#include <utility>

namespace duct {

/**
	@addtogroup variable
	@{
*/

/**
	Layout field flags.
	@note See Template::validate_layout() for usage notes.
*/
enum LayoutFieldFlag : unsigned {
	/** Optional field. */
	LAYOUT_FIELD_OPTIONAL=1<<20,
	/** Empty field signifier. */
	LAYOUT_FIELD_EMPTY=1<<21
};

/**
	Variable validator.
	@warning Every layout field after an optional field (@c LAYOUT_FIELD_OPTIONAL) is
	considered optional.
*/
class Template {
public:
/** @name Types */ /// @{
	/** Identity vector type. */
	typedef duct::aux::vector<u8string> identity_vector_type;
	/** Layout vector type. */
	typedef duct::aux::vector<unsigned> layout_vector_type;
/// @}

protected:
	unsigned m_type_mask{VARMASK_NONE}; /**< Type mask. */
	identity_vector_type m_identity{}; /**< Identity. */
	layout_vector_type m_layout{}; /**< Layout. */

public:
/** @name Constructors and destructor */ /// @{
	/**
		Construct with @c VARMASK_NONE type mask, empty identity, and empty layout.
	*/
	Template()=default;
	/**
		Construct with type mask, empty identity, and empty layout.
		@param type_mask Type mask.
	*/
	explicit Template(unsigned const type_mask)
		: m_type_mask{type_mask}
	{}
	/**
		Construct with type mask, identity, and empty layout.
		@param type_mask Type mask.
		@param identity Identity.
	*/
	Template(unsigned const type_mask, identity_vector_type identity)
		: m_type_mask{type_mask}
		, m_identity{std::move(identity)}
	{}
	/**
		Construct with type mask, layout, and empty identity.
		@param type_mask Type mask.
		@param layout Layout.
	*/
	Template(unsigned const type_mask, layout_vector_type layout)
		: m_type_mask{type_mask}
		, m_layout{std::move(layout)}
	{}
	/**
		Construct with type mask, identity, and layout.
		@param type_mask Type mask.
		@param identity Identity.
		@param layout Layout.
	*/
	Template(unsigned const type_mask, identity_vector_type identity, layout_vector_type layout)
		: m_type_mask{type_mask}
		, m_identity{std::move(identity)}
		, m_layout{std::move(layout)}
	{}
	/** Copy constructor (deleted). */
	Template(Template const&)=delete;
	/** Move constructor. */
	Template(Template&&)=default;
	/** Destructor. */
	virtual ~Template()=default;
/// @}

/** @name Operators */ /// @{
	/** Copy assignment operator (deleted). */
	Template& operator=(Template const&)=delete;
	/** Move assignment operator. */
	Template& operator=(Template&&)=default;
/// @}

/** @name Properties */ /// @{
	/**
		Set type mask.
		@param type_mask New type mask.
		@sa validate_type(Variable const&) const
	*/
	void set_type_mask(unsigned const type_mask) { m_type_mask=type_mask; }
	/**
		Get type mask.
		@returns The current type mask.
		@sa validate_type(Variable const&) const
	*/
	unsigned get_type_mask() const { return m_type_mask; }

	/**
		Set identity.
		@param identity New identity.
		@sa validate_identity(Variable const&) const
	*/
	void set_identity(identity_vector_type identity) { m_identity=std::move(identity); }
	/**
		Get identity.
		@returns The current identity.
		@sa validate_identity(Variable const&) const
	*/
	identity_vector_type& get_identity() { return m_identity; }
	/** @copydoc get_identity() */
	identity_vector_type const& get_identity() const { return m_identity; }

	/**
		Set layout.
		@param layout New layout.
		@sa validate_layout(Variable const&) const
	*/
	void set_layout(layout_vector_type layout) { m_layout=std::move(layout); }
	/**
		Get layout.
		@returns The current layout.
		@sa validate_layout(Variable const&) const
	*/
	layout_vector_type& get_layout() { return m_layout; }
	/** @copydoc get_layout() */
	layout_vector_type const& get_layout() const { return m_layout; }
/// @}

/** @name Validation */ /// @{
	/**
		Validate a variable.
		@note If @c validate_type(var) is @c true, layout validation is only done
		if @a var is a collection.
		@returns @c true iff @a var matches template in type, identity, and layout.
		@param var Variable to validate.
		@sa validate_type(Variable const&) const,
		validate_identity(Variable const&) const,
		validate_layout(Variable const&) const
	*/
	bool validate(Variable const& var) const {
		return
			validate_type(var) &&
			validate_identity(var) &&
			(!var.is_class(VARCLASS_COLLECTION) || validate_layout(var))
		;
	}

	/**
		Validate a variable by type.
		@returns @c true iff bitwise-and of @a var type and template type mask is
		non-zero.
		@param var Variable to validate.
	*/
	virtual bool validate_type(Variable const& var) const {
		return m_type_mask&var.get_type();
	}

	/**
		Validate a variable by identity.
		@returns @c true iff:
		-# identity is empty (permits any name),
		-# variable name matches any name from identity (including empty name).
		@param var Variable to validate.
	*/
	virtual bool validate_identity(Variable const& var) const {
		if (m_identity.empty()) {
			// Any name is permitted with empty identity
			return true;
		} else {
			for (auto const& iname : get_identity()) {
				if (var.get_name()==iname) {
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
		@note The @c LAYOUT_FIELD_OPTIONAL flag will cause all succeeding fields to be
		considered optional.
		@note The @c LAYOUT_FIELD_EMPTY flag is only considered when layout contains a
		single field.
		@returns
		- @c false iff:
			-# variable is not a @c VARCLASS_COLLECTION,
			-# variable has more children than layout;
		- @c true iff:
			-# layout is empty (permits any collection),
			-# layout contains a single field with flag @c LAYOUT_FIELD_EMPTY and
			   variable has no children,
			-# children sequentially match layout fields exactly,
			-# children sequentially match [0..var.size()] layout fields if a field from
			   [0..var.size()+1] is optional (thus making all subsequent fields optional).
		@param var Variable to validate.
	*/
	virtual bool validate_layout(Variable const& var) const {
		if (var.is_class(VARCLASS_COLLECTION)) {
			if (m_layout.empty()) {
				// Any collection is permitted with empty layout
				return true;
			} else if (var.size()>get_layout().size()) {
				// Collection cannot be larger than layout
				return false;
			} else if (1==m_layout.size() && (LAYOUT_FIELD_EMPTY&m_layout[0])) {
				return var.empty();
			} else {
				auto vc_iter=var.cbegin();
				auto lo_iter=get_layout().cbegin();
				bool optional_met=false;
				for (; var.cend()!=vc_iter; ++vc_iter, ++lo_iter) {
					if (0==((*lo_iter)&(*vc_iter).get_type())) {
						// Child type does not match field
						return false;
					} else if (!optional_met) {
						optional_met=(LAYOUT_FIELD_OPTIONAL&(*lo_iter));
					}
				}
				if (optional_met || get_layout().cend()==lo_iter) {
					// Collection sequentially matches layout fields exactly or any
					// trailing fields are optional
					return true;
				} else {
					// No optional field has been met, and there are unchecked trailing
					// field(s)
					if (LAYOUT_FIELD_OPTIONAL&(*lo_iter)) {
						// First trailing field is optional (therefore all following
						// fields are optional)
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
