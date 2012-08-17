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
#include "./stl.hpp"
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
	@note See @c Template::validate_layout() for usage notes.
*/
enum LayoutFieldFlag : unsigned int {
	/** Optional field. */
	LAYOUT_FIELD_OPTIONAL=1<<20,
	/** Empty field signifier. */
	LAYOUT_FIELD_EMPTY=1<<21
};

/**
	Variable validator.
	@warning Every layout field after an optional field (@c LAYOUT_FIELD_OPTIONAL) is considered optional.
*/
class Template {
	DUCT_DISALLOW_COPY_AND_ASSIGN(Template);

public:
	/** Identity vector type. */
	typedef duct::stl::vector<u8string>::type identity_vector_type;

	/** Layout vector type. */
	typedef duct::stl::vector<unsigned int>::type layout_vector_type;

/** @name Constructors and destructor */ /// @{
	/**
		Construct with @c VARMASK_NONE type mask, empty identity, and empty layout.
	*/
	Template()
		: m_type_mask(VARMASK_NONE)
		, m_identity()
		, m_layout()
	{}
	/**
		Construct with type mask, empty identity, and empty layout.
		@param type_mask Type mask.
	*/
	explicit Template(unsigned int const type_mask)
		: m_type_mask(type_mask)
		, m_identity()
		, m_layout()
	{}
	/**
		Construct with type mask, identity, and empty layout.
		@param type_mask Type mask.
		@param identity Identity.
	*/
	Template(unsigned int const type_mask, identity_vector_type identity)
		: m_type_mask(type_mask)
		, m_identity(std::move(identity))
		, m_layout()
	{}
	/**
		Construct with type mask, layout, and empty identity.
		@param type_mask Type mask.
		@param layout Layout.
	*/
	Template(unsigned int const type_mask, layout_vector_type layout)
		: m_type_mask(type_mask)
		, m_identity()
		, m_layout(std::move(layout))
	{}
	/**
		Construct with type mask, identity, and layout.
		@param type_mask Type mask.
		@param identity Identity.
		@param layout Layout.
	*/
	Template(unsigned int const type_mask, identity_vector_type identity, layout_vector_type layout)
		: m_type_mask(type_mask)
		, m_identity(std::move(identity))
		, m_layout(std::move(layout))
	{}
	/**
		Destructor.
	*/
	virtual ~Template() {}
/// @}

/** @name Properties */ /// @{
	/**
		Set type mask.
		@param type_mask New type mask.
		@sa validate_type(Variable const&)
	*/
	inline void set_type_mask(unsigned int const type_mask) { m_type_mask=type_mask; }
	/**
		Get type mask.
		@returns The current type mask.
		@sa validate_type(Variable const&)
	*/
	inline unsigned int get_type_mask() const { return m_type_mask; }

	/**
		Set identity.
		@param identity New identity.
		@sa validate_identity(Variable const&)
	*/
	inline void set_identity(identity_vector_type identity) { m_identity=std::move(identity); }
	/**
		Get identity.
		@returns The current identity.
		@sa validate_identity(Variable const&)
	*/
	inline identity_vector_type& get_identity() { return m_identity; }
	/** @copydoc get_identity() */
	inline identity_vector_type const& get_identity() const { return m_identity; }

	/**
		Set layout.
		@param layout New layout.
		@sa validate_layout(Variable const&)
	*/
	inline void set_layout(layout_vector_type layout) { m_layout=std::move(layout); }
	/**
		Get layout.
		@returns The current layout.
		@sa validate_layout(Variable const&)
	*/
	inline layout_vector_type& get_layout() { return m_layout; }
	/** @copydoc get_layout() */
	inline layout_vector_type const& get_layout() const { return m_layout; }
/// @}

/** @name Validation */ /// @{
	/**
		Validate a variable.
		@returns @c true if @c var matches template in identity and layout.
		@param var Variable to validate.
		@sa validate_type(Variable const&), validate_identity(Variable const&), validate_layout(Variable const&)
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
		@returns @c true if bitwise-and of variable type and type mask is non-zero.
		@param var Variable to validate.
	*/
	virtual bool validate_type(Variable const& var) const {
		return m_type_mask&var.get_type();
	}

	/**
		Validate a variable by identity.
		@returns @c true if:
			-# identity is empty (permits any name)
			-# variable name matches any name from identity (including empty name)
		@param var Variable to validate.
	*/
	virtual bool validate_identity(Variable const& var) const {
		if (m_identity.empty()) {
			return true; // Any name is permitted with empty identity
		} else {
			for (auto iname : get_identity()) {
				if (iname==var.get_name()) {
					return true; // Variable name matches a name from identity
				}
			}
			return false; // Variable name does not match a name from identity
		}
	}

	/**
		Validate a variable by layout.
		@note The @c LAYOUT_FIELD_OPTIONAL flag will cause all succeeding fields to be considered optional.
		@note The @c LAYOUT_FIELD_EMPTY flag is only considered when layout contains a single field.
		@returns
			- @c false if:
				-# variable is not a @c VARCLASS_COLLECTION
				-# variable has more children than layout
			- @c true if:
				-# layout is empty (permits any collection)
				-# layout contains a single field with flag @c LAYOUT_FIELD_EMPTY and variable has no children
				-# children sequentially match layout fields exactly
				-# children sequentially match [0..var.size()] layout fields if a field from [0..var.size()+1] is optional (thus making all subsequent fields optional)
		@param var Variable to validate.
	*/
	virtual bool validate_layout(Variable const& var) const {
		if (var.is_class(VARCLASS_COLLECTION)) {
			if (m_layout.empty()) {
				return true; // Any collection is permitted with empty layout
			} else if (var.size()>get_layout().size()) {
				return false; // Collection cannot be larger than layout
			} else if (1==m_layout.size() && (LAYOUT_FIELD_EMPTY&m_layout[0])) {
				return var.empty();
			} else {
				auto vc_iter=var.cbegin();
				auto lo_iter=get_layout().cbegin();
				bool optional_met=false;
				for (; var.cend()!=vc_iter; ++vc_iter, ++lo_iter) {
					if (0==((*lo_iter)&(*vc_iter).get_type())) {
						return false; // Child type does not match field
					} else if (!optional_met) {
						optional_met=(LAYOUT_FIELD_OPTIONAL&(*lo_iter));
					}
				}
				if (optional_met || get_layout().cend()==lo_iter) {
					return true; // Collection sequentially matches layout fields exactly or any trailing fields are optional
				} else {
					// No optional field has been met, and there are unchecked trailing field(s)
					if (LAYOUT_FIELD_OPTIONAL&(*lo_iter)) {
						return true; // First trailing field is optional (therefore all following fields are optional)
					} else {
						return false; // First trailing field is not optional
					}
				}
			}
		} else {
			return false; // Variable is not a collection
		}
	}
/// @}

protected:
	unsigned int m_type_mask; /**< Type mask. */
	identity_vector_type m_identity; /**< Identity. */
	layout_vector_type m_layout; /**< Layout. */
};

/** @} */ // end of doc-group variable

} // namespace duct

#endif // DUCT_TEMPLATE_HPP_
