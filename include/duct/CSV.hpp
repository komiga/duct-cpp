/**
@file CSV.hpp
@author Tim Howard

@section LICENSE
Copyright (c) 2010-2012 Tim Howard

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

Implements component parts:
<ul>
	<li>ductCSV</li>
</ul>
*/

#ifndef DUCT_CSV_HPP_
#define DUCT_CSV_HPP_

#include <duct/config.hpp>
#include <duct/Parser.hpp>
#include <duct/Variables.hpp>
#include <duct/CharacterSet.hpp>
#include <duct/Stream.hpp>

#include <map>
#include <string>
#include <exception>

namespace duct {

// Forward declarations
class CSVRow;
class CSVMap;
//enum CSVTokenType;
//enum CSVParserError;
class CSVParser;
class CSVParserException;
class CSVParserHandler;
class CSVFormatter;

/**
	int-key ValueVariable pointer map.
*/
typedef std::map<int, ValueVariable*> CSVRecordMap;
/**
	int-key CSVRow pointer map.
*/
typedef std::map<int, CSVRow*> CSVRowMap;

/**
	CSV row.
*/
class DUCT_API CSVRow {
public:
	/**
		Constructor with index.
		@param index The row's index.
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
		This method is intended to be used by an owner, and does not update its index in said owner (e.g. CSVMap).
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
		Get the start record iterator.
		@returns Start record iterator.
	*/
	CSVRecordMap::iterator begin();
	CSVRecordMap::const_iterator begin() const;
	/**
		Get the end record iterator.
		@returns End record iterator.
	*/
	CSVRecordMap::iterator end();
	CSVRecordMap::const_iterator end() const;
	/**
		Get the reverse start record iterator.
		@returns Reverse start record iterator.
	*/
	CSVRecordMap::reverse_iterator rbegin();
	CSVRecordMap::const_reverse_iterator rbegin() const;
	/**
		Get the reverse end record iterator.
		@returns Reverse end record iterator.
	*/
	CSVRecordMap::reverse_iterator rend();
	CSVRecordMap::const_reverse_iterator rend() const;
	/**
		Find the iterator for the given record index.
		@returns The iterator for the index.
		@param index The record index to get.
	*/
	CSVRecordMap::iterator find(int index);
	CSVRecordMap::const_iterator find(int index) const;
	/**
		Erase the given iterator.
		This will destroy the iterator's value if it is valid.
		@returns Nothing.
		@param iter The iterator to erase.
	*/
	void erase(CSVRecordMap::iterator iter);
	void erase(CSVRecordMap::reverse_iterator iter);
	/**
		Set the record at the given index.
		If the index is already set, its value is destroyed and the given value replaces it.
		The row owns any value inserted, and is responsible for destroying them.
		@returns Nothing.
		@param index The index to set.
		@param value The value to use. Setting an index to NULL is different from removing a record. It will be interpreted as null record rather than no record.
	*/
	void set(int index, ValueVariable* value);
	/**
		Get the record at the given index.
		To distinguish between <em>NULL</em> and <em>unset</em> records, see has().
		@returns The value at the index, or NULL if there was no record at the index.
		@param index The index to retrieve.
	*/
	ValueVariable* get(int index);
	ValueVariable const* get(int index) const;
	/**
		Get the integer at the given position.
		@returns The IntVariable at the position, or NULL if either the index was invalid or the variable at the position wasn't the correct type.
		@param index The index to retrieve.
	*/
	IntVariable* getInt(int index);
	IntVariable const* getInt(int index) const;
	/**
		Get the integer value at the given position.
		@returns true on success, or false if either the index was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getIntValue(int& result, int index) const;
	/**
		Get the string at the given position.
		@returns The StringVariable at the position, or NULL if either the index was invalid or the variable at the position wasn't the correct type.
		@param index The index to retrieve.
	*/
	StringVariable* getString(int index);
	StringVariable const* getString(int index) const;
	/**
		Get the string value at the given position.
		@returns The string at the position, or NULL if either the index was invalid or the variable at the position wasn't the correct type.
		@param index The index to retrieve.
	*/
	icu::UnicodeString const* getStringValue(int index) const;
	/**
		Get the string value at the given position.
		@returns true on success, or false if either the index was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getStringValue(icu::UnicodeString& result, int index) const;
	/**
		Get the float at the given position.
		@returns The FloatVariable at the position, or NULL if either the index was invalid or the variable at the position wasn't the correct type.
		@param index The index to retrieve.
	*/
	FloatVariable* getFloat(int index);
	FloatVariable const* getFloat(int index) const;
	/**
		Get the float value at the given position.
		@returns true on success, or false if either the index was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getFloatValue(float& result, int index) const;
	/**
		Get the bool at the given position.
		@returns The BoolVariable at the position, or NULL if either the index was invalid or the variable at the position wasn't the correct type.
		@param index The index to retrieve.
	*/
	BoolVariable* getBool(int index);
	BoolVariable const* getBool(int index) const;
	/**
		Get the bool value at the given position.
		@returns true on success, or false if either the index was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getBoolValue(bool& result, int index) const;
	/**
		Get the variable at the given position as a string.
		@returns true on success, or false if either the index was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param index The index to retrieve.
	*/
	bool getAsString(icu::UnicodeString& result, int index) const;
	/**
		Remove the record at the given index.
		If the record at the index was removed, its value is destroyed.
		@returns true on success, or false if there was no record at the index (unset record).
		@param index The index to remove.
	*/
	bool remove(int index);
	/**
		Clear the row.
		@returns Nothing.
	*/
	void clear();
	/**
		Check if the given index is valid (set or NULL).
		@returns true if the index was valid, or false if there was no record at the index (unset record).
		@param index The index to check.
	*/
	bool has(int index) const;
	/**
		Get the number of records in the row.
		@returns The number of records in the row.
		@param nulls Whether to count null records.
	*/
	size_t getCount(bool nulls=true) const;
	/**
		Get the number of records in the given range (inclusive).
		@returns The number of records matching the given parameters in the range.
		@param nulls Whether to count null records.
	*/
	size_t inRange(int start, int end, bool nulls=true) const;
	
protected:
	int m_index;
	CSVRecordMap m_values;
};

/**
	Collection of CSVRows.
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
	/**
		Get the number of rows in the map (includes header rows if they are present).
		@returns The number of rows in the map.
	*/
	size_t getRowCount() const;
	/**
		Get the number of header fields for the given header index.
		@returns The number of header fields at the header index.
		@param index The header index to check.
		@param nulls Whether to count null records.
	*/
	size_t getHeaderCount(int index=-1, bool nulls=true) const;
	/**
		Get the number of values in the map.
		@returns The number of values in the map.
	*/
	size_t getValueCount() const;
	/**
		Get the start row iterator.
		@returns Start row iterator.
	*/
	CSVRowMap::iterator begin();
	CSVRowMap::const_iterator begin() const;
	/**
		Get the end row iterator.
		@returns End row iterator.
	*/
	CSVRowMap::iterator end();
	CSVRowMap::const_iterator end() const;
	/**
		Get the reverse start row iterator.
		@returns Reverse start row iterator.
	*/
	CSVRowMap::reverse_iterator rbegin();
	CSVRowMap::const_reverse_iterator rbegin() const;
	/**
		Get the reverse end row iterator.
		@returns Reverse end row iterator.
	*/
	CSVRowMap::reverse_iterator rend();
	CSVRowMap::const_reverse_iterator rend() const;
	/**
		Find the iterator for the given row index.
		@returns The iterator for the index.
		@param index The row index to get.
	*/
	CSVRowMap::iterator find(int index);
	CSVRowMap::const_iterator find(int index) const;
	/**
		Erase the given iterator.
		This will destroy the iterator's value if it is valid.
		@returns Nothing.
		@param iter The iterator to erase.
	*/
	void erase(CSVRowMap::iterator iter);
	void erase(CSVRowMap::reverse_iterator iter);
	/**
		Set the given row at the row's index.
		The map owns the given row and is responsible for destroying it.
		@returns true on success, or false if the given row was NULL (ownership unchanged).
		@param row The row to set.
	*/
	bool set(CSVRow* row);
	/**
		Set the given row at the given index.
		If the row was set, its index is set to the given index.
		The map owns the given row and is responsible for destroying it.
		@returns true on success, or false if the given row was NULL (ownership unchanged).
		@param index The index to set.
		@param row The row to use.
	*/
	bool set(int index, CSVRow* row);
	/**
		Get the row at the given index.
		@returns The row at the index, or NULL if the index is unset.
		@param index The index to retrieve.
	*/
	CSVRow* get(int index);
	CSVRow const* get(int index) const;
	/**
		Move the row at the given index.
		If <em>swap</em> is false, the destination row is destroyed and replaced with the row at the source index.
		@returns true on success, or false if the source index was empty.
		@param src The source index.
		@param dest The dest index.
		@param swap Whether to swap the source and destination rows or to replace the destination row with the source row (default).
	*/
	bool moveRow(int src, int dest, bool swap=false);
	/**
		Remove the row at the given index.
		@returns true on success, or false if the index was empty.
		@param index The index to remove.
	*/
	bool remove(int index);
	/**
		Check if the given row index is valid.
		@returns true if the index was set, or false if the index is unset.
		@param row The row index to check.
	*/
	bool has(int row) const;
	/**
		Clear the rows from the map.
		@returns Nothing.
	*/
	void clear();
	/**
		Set the value at the given position.
		@returns true on success, or false if the row index was unset (only possible when <em>autocreate</em> is false).
		@param row The row index.
		@param column The column index.
		@param value The value to use.
		@param autocreate Whether to automatically create the row if it is NULL.
	*/
	bool setValue(int row, int column, ValueVariable* value, bool autocreate=false);
	/**
		Get the value at the given position.
		To destinguish between unset row, unset record, and null record, see has() and hasValue().
		@returns The value at the position, or NULL if the row was unset.
		@param row The row index.
		@param column The column index.
	*/
	ValueVariable* getValue(int row, int column);
	ValueVariable const* getValue(int row, int column) const;
	/**
		Get the integer at the given position.
		@returns The IntVariable at the position, or NULL if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	IntVariable* getInt(int row, int column);
	IntVariable const* getInt(int row, int column) const;
	/**
		Get the integer value at the given position.
		@returns true on success, or false if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	bool getIntValue(int& result, int row, int column) const;
	/**
		Get the string at the given position.
		@returns The StringVariable at the position, or NULL if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	StringVariable* getString(int row, int column);
	StringVariable const* getString(int row, int column) const;
	/**
		Get the string value at the given position.
		@returns The string at the position, or NULL if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	icu::UnicodeString const* getStringValue(int row, int column) const;
	/**
		Get the string value at the given position.
		@returns true on success, or false if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	bool getStringValue(icu::UnicodeString& result, int row, int column) const;
	/**
		Get the float at the given position.
		@returns The FloatVariable at the position, or NULL if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	FloatVariable* getFloat(int row, int column);
	FloatVariable const* getFloat(int row, int column) const;
	/**
		Get the float value at the given position.
		@returns true on success, or false if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	bool getFloatValue(float& result, int row, int column) const;
	/**
		Get the bool at the given position.
		@returns The BoolVariable at the position, or NULL if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	BoolVariable* getBool(int row, int column);
	BoolVariable const* getBool(int row, int column) const;
	/**
		Get the bool value at the given position.
		@returns true on success, or false if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	bool getBoolValue(bool& result, int row, int column) const;
	/**
		Get the variable at the given position as a string.
		@returns true on success, or false if either the row or column was invalid or the variable at the position wasn't the correct type.
		@param result The result. This is not modified if the retrieval failed.
		@param row The row to retrieve from.
		@param column The column to retrieve.
	*/
	bool getAsString(icu::UnicodeString& result, int row, int column) const;
	/**
		Remove the value at the given position.
		@returns true on success, or false if either the row index was empty or the column was empty.
		@param row The row index.
		@param column The column index.
	*/
	bool removeValue(int row, int column);
	/**
		Check if the given row and column are valid.
		@returns true if the position is set, or false if either the row or value is unset.
		@param row The row index.
		@param column The record index.
	*/
	bool hasValue(int row, int column) const;
	/**
		Clear all the values from the map.
		This does not remove the rows which own the values.
		@returns Nothing.
	*/
	void clearValues();
	
protected:
	CSVRowMap m_rows;
};

/**
	Token types for CSVParser.
*/
enum CSVTokenType {
	StringToken=1,
	QuotedStringToken,
	NumberToken,
	DoubleToken,
	SeparatorToken,
	EOFToken,
	EOLToken
};

/**
	CSVParser errors.
*/
enum CSVParserError {
	/** Unknown error. */
	PARSERERROR_UNKNOWN=0,
	/** Parser error. */
	PARSERERROR_PARSER,
	/** Memory allocation error (e.g. out of memory). */
	PARSERERROR_MEMALLOC
};

/**
	ductCSV parser.
*/
class DUCT_API CSVParser : public Parser {
public:
	friend class CSVParserException;
	friend class CSVParserHandler;
	
	/**
		Constructor.
	*/
	CSVParser();
	/**
		Constructor with stream.
		The user is responsible for closing the stream.
		@param stream The stream to read from.
	*/
	CSVParser(Stream* stream);
	/**
		Destructor.
	*/
	~CSVParser();
	/**
		Set the separator character.
		@returns Nothing.
		@param c The separator character to use.
	*/
	void setSeparator(UChar32 c);
	/**
		Get the separator character.
		@returns The parser's separator character.
	*/
	UChar32 getSeparator() const;
	void setHandler(ParserHandler* handler);
	ParserHandler* getHandler();
	Token& nextToken();
	void readToken();
	bool parse();
	/**
		Read a number token.
		@returns Nothing.
	*/
	void readNumberToken();
	/**
		Read a floating-point number token.
		@returns Nothing.
	*/
	void readDoubleToken();
	/**
		Read a string token.
		@returns Nothing.
	*/
	void readStringToken();
	/**
		Read a quoted-string token.
		@returns Nothing.
	*/
	void readQuotedStringToken();
	
protected:
	static CharacterSet s_numberset;
	static CharacterSet s_numeralset;
	static CharacterSet s_signset;
	static CharacterSet s_whitespaceset;
	
	CSVParserHandler* m_handler;
	unsigned int m_rowbegin;
	UChar32 m_sepchar;
};

/**
	A CSVParser exception.
*/
class DUCT_API CSVParserException : public std::exception {
public:
	/**
		Constructor with values.
	*/
	CSVParserException(CSVParserError error, char const* reporter, Token const* token, CSVParser const* parser, char const* fmt, ...);
	/**
		Get the exception's message.
		@returns The exception's message.
	*/
	virtual char const* what() const throw();
	/**
		Convert an exception error to a NUL-terminated string.
		@returns The error as a string.
		@param error The error to convert.
	*/
	static char const* errorToString(CSVParserError error);
	
protected:
	char m_message[512];
	CSVParserError m_error;
	char const* m_reporter;
	Token const* m_token;
	CSVParser const* m_parser;
};

/**
	Standard CSVParser handler.
*/
class DUCT_API CSVParserHandler : public ParserHandler {
public:
	/**
		Constructor.
	*/
	CSVParserHandler(CSVParser& parser);
	void setParser(Parser& parser);
	Parser& getParser();
	/**
		Throw an exception.
		@returns Nothing.
	*/
	void throwex(CSVParserException e);
	/**
		Clean the handler.
		@returns Nothing.
	*/
	void clean();
	/**
		Run the parser.
		@returns Nothing.
	*/
	bool process();
	/**
		Handle the given token.
		@returns Nothing.
		@param token The token to handle.
	*/
	void handleToken(Token& token);
	/**
		Finish processing.
		@returns Nothing.
	*/
	void finish();
	/**
		Process the given stream.
		The user is responsible for closing the stream.
		Calls made by this function may throw a CSVParserException.
		@returns The map processed from the stream.
		@param stream The stream to process.
	*/
	CSVMap* processFromStream(Stream* stream);
	/**
		Setup the handler/parser's options.
		@returns Nothing.
		@param sepchar The separator character to use.
		@param headercount The number of header rows.
	*/
	void setup(UChar32 sepchar, unsigned int headercount);
	/**
		Free any state data.
		NOTE: Only call this when an exception is to be thrown, or when the handler does not return a valid map.
		@returns Nothing.
	*/
	void freeData();
	/**
		Add the given value to the current row.
		NOTE: This does not increment m_columnindex.
		@returns Nothing.
		@param val The value to add to the row.
	*/
	void addToRow(ValueVariable* val);
	/**
		Create a new row and set the current row.
		NOTE: This resets handler states.
		@returns Nothing.
	*/
	void newRow();
	
protected:
	CSVParser& m_parser;
	CSVMap* m_map;
	CSVRow* m_currentrow;
	int m_strow, m_row, m_column;
	bool m_gtoken;
};

/**
	CSV formatter.
*/
class DUCT_API CSVFormatter {
public:
	/**
		Format the given row.
		@returns Nothing.
		@param row The row to format.
		@param result The result string.
		@param sepchar The separator character.
		@param varformat The format for records.
	*/
	static void formatRow(CSVRow const& row, icu::UnicodeString& result, UChar32 sepchar=',', unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Load the given file path as a CSVMap.
		The user owns the returned map.
		Calls made by this function may throw CSVParserExceptions.
		@returns The map for the CSV file, or NULL if the file could not be opened/read.
		@param path The file path to read.
		@param sepchar The separator character.
		@param headercount The number of header rows.
		@param encoding File character encoding.
	*/
	static CSVMap* loadFromFile(char const* path, UChar32 sepchar=',', unsigned int headercount=0, char const* encoding="utf8");
	static CSVMap* loadFromFile(std::string const& path, UChar32 sepchar=',', unsigned int headercount=0, char const* encoding="utf8");
	static CSVMap* loadFromFile(icu::UnicodeString const& path, UChar32 sepchar=',', unsigned int headercount=0, char const* encoding="utf8");
	/**
		Load the given stream as a CSVMap.
		The user owns the returned map.
		Calls made by this function may throw CSVParserExceptions.
		@returns The map for the file, or NULL if the given stream was NULL.
		@param stream The stream to read from.
		@param sepchar The separator character.
		@param headercount The number of header rows.
	*/
	static CSVMap* loadFromStream(Stream* stream, UChar32 sepchar=',', unsigned int headercount=0);
	/**
		Write the given map to the given file path.
		@returns true on success, or false if either the given map was NULL or the given path could not be opened for writing.
		@param map The map to write.
		@param path The file path.
		@param sepchar The separator character.
		@param encoding The character encoding to use.
		@param varformat The format for values.
	*/
	static bool writeToFile(CSVMap const* map, char const* path, UChar32 sepchar=',', char const* encoding="utf8", unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(CSVMap const* map, std::string const& path, UChar32 sepchar=',', char const* encoding="utf8", unsigned int varformat=FMT_ALL_DEFAULT);
	static bool writeToFile(CSVMap const* map, icu::UnicodeString const& path, UChar32 sepchar=',', char const* encoding="utf8", unsigned int varformat=FMT_ALL_DEFAULT);
	/**
		Write the given map to the given stream.
		@returns true on success, or false if either the map or stream was NULL.
		@param map The map to write.
		@param stream The stream to write to.
		@param sepchar The separator character.
		@param varformat The format for values.
	*/
	static bool writeToStream(CSVMap const* map, Stream* stream, UChar32 sepchar=',', unsigned int varformat=FMT_ALL_DEFAULT);
	
protected:
	static CSVParser s_parser;
	static CSVParserHandler s_handler;
};

} // namespace duct

#endif // DUCT_CSV_HPP_
