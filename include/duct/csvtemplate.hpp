/**
@file csvtemplate.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2011 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

@section DESCRIPTION

Extension to component parts:
<ul>
	<li>ductCSV</li>
</ul>
*/

#ifndef _DUCT_CSVTEMPLATE_HPP
#define _DUCT_CSVTEMPLATE_HPP

#include <duct/config.hpp>
#include <duct/csv.hpp>
#include <duct/variables.hpp>

#include <map>

namespace duct {

/**
	CSVTemplate record.
*/
typedef struct {
	/** Types the record can be. */
	unsigned int type;
	/** Whether the record can be null. */
	bool nullable;
} CSVTRecord;

/**
	CSV template.
*/
class DUCT_API CSVTemplate {
public:
	/**
		Constructor.
		@param count The number of records in the layout.
		@param layout The template's layout.
	*/
	CSVTemplate(size_t count, const CSVTRecord* layout);
	/**
		Set the template's layout.
		@returns Nothing.
		@param count The number of records in the layout.
		@param layout The new layout.
	*/
	void setLayout(size_t count, const CSVTRecord* layout);
	/**
		Get the template's layout.
		@returns The template's layout.
	*/
	const CSVTRecord* getLayout() const;
	/**
		Get the number of records in the layout.
		@returns The number of records in the layout.
	*/
	size_t getCount() const;
	/**
		Validate the given row.
		Records are checked in consecutive order from <em>startindex</em>.
		Template records that are nullable will match null <em>or</em> unset records.
		@returns true on success, or false on failure.
		@param row The row to validate.
		@param startindex The index to start from.
	*/
	bool validate(const CSVRow& row, int startindex=0) const;
	/**
		Remove matching or non-matching rows from the given map.
		@returns The number of removed rows.
		@param map The map to modify.
		@param matching Whether to remove matching or non-matching rows. Default is to remove non-matching rows.
	*/
	size_t removeRows(CSVMap& map, bool matching=false) const;
	
protected:
	size_t _count;
	const CSVTRecord* _layout;
};

} // namespace duct

#endif // _DUCT_CSVTEMPLATE_HPP

