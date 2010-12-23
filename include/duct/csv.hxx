/**
@file csv.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010 Tim Howard

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

duct++ CSV reading/writing.
*/

#ifndef _DUCT_CSV_HPP
#define _DUCT_CSV_HPP

#include <duct/config.hpp>
#include <duct/variables.hpp>
#include <map>

namespace duct {

/**
	CSV record.
*/
class DUCT_API CSVRecord {
public:
	/**
		Constructor.
		@param index The record's index. Used for positioning in the CSVRow class.
		@param value The record's value. NOTE: The record owns the given value, and is responsible for destroying it.
	*/
	CSVRecord(signed int index, ValueVariable* value);
	/**
		Destructor.
	*/
	~CSVRecord();
	
	/**
		Set the record's index.
		@returns Nothing.
		@param index The record's new index.
	*/
	void setIndex(signed int index);
	/**
		Get the record's index.
		@returns The record's index.
	*/
	signed int getIndex() const;
	
	/**
		Set the record's value.
		@returns Nothing.
		@param variable The record's new value.
	*/
	void setValue(ValueVariable* value);
	/**
		Get the record's value.
		@returns The record's value.
	*/
	ValueVariable* getValue();
	/**
		Get the record's value (const).
		@returns The record's value.
	*/
	const ValueVariable* getValue() const;
	
	/**
		Get the record's header.
		The record's header is the name of its ValueVariable.
		@returns The record's header.
	*/
	const UnicodeString& getHeader() const;
	
	/**
		Get the record's value as a string with the given format.
		@returns Nothing.
		@param result The result string.
		@param format The format flags.
	*/
	void getValueFormatted(UnicodeString& result, unsigned int format = FMT_ALL_DEFAULT) const;
	
	/**
		Serialize the record to the given stream.
		@returns Nothing.
		@param stream The stream to write to.
	*/
	void serialize(Stream* stream) const;
	
	/**
		Get the record's value as a string.
		@returns Nothing.
		@param result The result string.
	*/
	void valueAsString(UnicodeString& result) const;
	
protected:
	signed int _index;
	ValueVariable* _variable;
};

typedef std::map<int, CSVRecord*> CSVRecordMap;

/**
	CSV row.
*/
class DUCT_API CSVRow {
public:
	/**
		Constructor with index.
	*/
	CSVRow(int index);
	/**
		Constructor.
	*/
	CSVRow();
	/**
		Destructor.
	*/
	~CSVRow();
	
	/**
		Set the row's index.
		@returns Nothing.
		@param index The row's new index.
	*/
	void setIndex(int index);
	/**
		Get the row's index.
		@returns The row's index.
	*/
	int getIndex() const;
	
	/**
		Get the const start record iterator.
		@returns Start of record iterator.
	*/
	CSVRecordMap::const_iterator begin() const;
	/**
		Get the const end record iterator.
		@returns End of record iterator.
	*/
	CSVRecordMap::const_iterator end() const;
	/**
		Get the start record iterator.
		@returns Start of record iterator.
	*/
	CSVRecordMap::iterator begin();
	/**
		Get the end record iterator.
		@returns End of record iterator.
	*/
	CSVRecordMap::iterator end();
	
	/**
		Insert a record.
		The row owns any record inserted, and is responsible for destroying them.
		@returns true if the record was inserted, or false if the given record was NULL.
		@param record The record to add.
	*/
	bool insertRecord(CSVRecord* record);
	
	/**
		Get the record with the given index.
		@returns The record with the given index, or NULL if the row does not have a record with the given index.
		@param index The index of the record to retrieve.
	*/
	CSVRecord* getRecord(int index);
	
	/**
		Remove the record with the given index.
		@returns true if the record with the given index was removed, or false if the row does not have a record with the given index.
		@param index The index of the record to remove.
	*/
	bool removeRecord(int index);
	
	/**
		Check if the row has a record with the given index.
		@returns true if the given index was found, or false if the row does not have a record with the given index.
		#param index The index to search for.
	*/
	bool hasRecord(int index) const;
	
	/**
		Get the number of records in the row.
		@returns The number of records in the row.
		@param blanks Whether to count blanks or ignore blanks. This will return the last record index if passed as true (default).
	*/
	unsigned int getCount(bool blanks = true) const;
	
	/**
		Deserialize the row from the given stream.
		@returns Itself.
		@param stream The stream to read from.
	*/
	CSVRow& deserialize(Stream* stream, UChar separator = ',');
	/**
		Serialize the row to the given stream.
		@returns Nothing.
		@param stream The stream to write to.
	*/
	void serialize(Stream* stream, UChar separator = ',', unsigned int format = FMT_ALL_DEFAULT) const;
	
protected:
	int _index;
	CSVRecordMap* _records;
	
};

typedef std::map<int, CSVRow*> CSVRowMap;

/**
	CSV row/record map.
*/
class DUCT_API CSVMap {
public:
	/**
		Constructor.
	*/
	CSVMap();
	/**
		Destructor.
	*/
	~CSVMap();
	
protected:
	
	
};

} // namespace duct

#endif // _DUCT_CSV_HPP

