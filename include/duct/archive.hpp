/**
@file archive.hpp
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
	<li>ductArchive</li>
	<ul>
		<li>class Archive</li>
		<li>class Entry</li>
	</ul>
</ul>
*/

#ifndef _DUCT_ARCHIVE_HPP
#define _DUCT_ARCHIVE_HPP

#include <duct/config.hpp>
#include <duct/filestream.hpp>

#include <string.h>
#include <unicode/unistr.h>

namespace duct {

/**
	Entry flag enum.
*/
enum EntryFlag {
	/** Convenience member for no-flags. */
	ENTRYFLAG_NONE=0x00,
	/** The entry is compressed. */
	ENTRYFLAG_COMPRESSED=0x01,
	/** Reserved flag 0x02. */
	ENTRYFLAG_RESERVED0=0x02,
	/** Reserved flag 0x04. */
	ENTRYFLAG_RESERVED1=0x04
};

// Forward declarations
class Archive;
class Entry;

/**
	Base archive class.
	This implements bare archive (de)serialization.
*/
class DUCT_API Archive {
public:
	/**
		Constructor.
	*/
	inline Archive()
		: m_stream(NULL), m_readable(false), m_writeable(false)
	{/* Do nothing */}
	/**
		Constructor with path.
	*/
	inline Archive(icu::UnicodeString const& path)
		: m_stream(NULL), m_path(path), m_readable(false), m_writeable(false)
	{/* Do nothing */}
	/**
		Destructor.
	*/
	inline virtual ~Archive() { close(); }
	/**
		Get the four-char string identifier for the archive's extended format.
		@returns The archive-type's four-char string identifier.
	*/
	virtual char const* getIdentifier() const=0;
	/**
		Get the archive's stream.
		@returns The archive's current stream, which may be NULL.
		@see isReadable(), isWriteable()
	*/
	inline Stream* getStream() { return m_stream; }
	/**
		Get the archive's path.
		@returns The archive's path.
	*/
	inline icu::UnicodeString const& getPath() const { return m_path; }
	/**
		Set the archive's path.
		@returns Nothing.
		@param path The archive's new path.
	*/
	inline void setPath(icu::UnicodeString const& path) { m_path.setTo(path); }
	/**
		Determine if the archive is currently open.
		@returns true if the archive is currently open, or false if it is closed.
		@see isReadable(), isWriteable()
	*/
	inline bool isOpen() const { return (m_readable || m_writeable); }
	/**
		Determine if the archive is opened for reading.
		@returns true if the archive is open for reading, or false if it is not.
		@see isWriteable(), isOpen()
	*/
	inline bool isReadable() const { return m_readable; }
	/**
		Determine if the archive is opened for writing.
		@returns true if the archive is open for writing, or false if it is not.
		@see isReadable(), isOpen()
	*/
	inline bool isWriteable() const { return m_writeable; }
	/**
		Get the size (in bytes) of the archive's metadata (includes the <em>user-space</em> section).
		The base implementation always returns 4.
		@returns The size of the archive's metadata.
	*/
	inline virtual size_t getMetadataSize() const { return 4; /* identifier */ }
	/**
		Get the size (in bytes) of the entire archive header (includes the <em>user-space</em> and <em>entry-metadata</em> sections).
		@returns The size of the archive's metadata.
	*/
	virtual size_t getHeaderSize() const=0;
	/**
		Get the number of entries in the archive.
		@returns The number of entries in the archive.
	*/
	virtual unsigned int getCount() const=0;
	/**
		Open or reopen the archive with the given access.
		@returns true if the archive has been (re)opened successfully, or false if the archive was either not found or could not be opened with the given access (always the case if both <em>readable</em> and <em>writeable</em> are false.
		@param deserialize Whether to deserialize the archive (does nothing if <em>readable</em> is false).
		@param readable Whether to open the archive with read access.
		@param writeable Whether to open the archive with write access.
		@see close()
	*/
	virtual bool open(bool deserialize=true, bool readable=true, bool writeable=false);
	/**
		Close the archive if it is opened.
		@returns Nothing.
	*/
	virtual void close();
	/**
		Save the archive and return to previous state.
		If the archive is closed ("in-memory") before this is called, the archive is returned to being closed after saving.
		@returns true on success, or false if the archive could not be opened or written.
	*/
	virtual bool save();
	/**
		Save the archive.
		This will reopen the archive with write-only access (truncates the file if it already exists, meaning you can't have read-access).
		@returns true on success, or false if the archive could not be opened or written.
		@param keepopen Whether to keep the archive open after serialization.
	*/
	virtual bool save(bool keepopen);
	/**
		Remove all entries from the archive.
		@returns Nothing.
	*/
	virtual void clear()=0;
	/**
		Deserialize the archive's header.
		This will clear() the current entries, seek to 0, read the base metadata and then call deserializeUserspace().
		@returns true on success, or false if either an error occurred or the archive was not open for reading.
		@see deserializeUserspace(), serialize()
	*/
	bool deserialize();
	/**
		Serialize the archive's header.
		This will seek to 0, write the base metadata and then call serializeUserspace().
		@returns true on success, or false if either an error occurred or the archive was not opened with write access.
	*/
	bool serialize();
	/**
		Deserialize the <em>user-space</em> section.
		This should read at the least <em>entry_count</em> and the <em>entry-metadata</em> section (both are implied sections, meaning you can do it however you want).
		@returns true on success, or false on failure.
	*/
	virtual bool deserializeUserspace()=0;
	/**
		Serialize the <em>user-space</em> section.
		This should write at the least <em>entry_count</em> and the <em>entry-metadata</em> section.
		@returns true on success, or false on failure.
	*/
	virtual bool serializeUserspace()=0;
	/**
		Read entry data from the archive.
		This should seek to the <em>DataOffset</em> (see Entry::getDataOffset()) and read all entry data. As with the Entry::read() method, this is more for non-container-type entries.
		@returns true on success, or false on failure (either the archive was not open [with read access], or a call to Entry::read() returned false).
	*/
	virtual bool readEntries()=0;
	/**
		Write entry data to the archive.
		This should seek to the beginning of the data section and call Entry::write() for each entry. Each entry's <em>DataOffset</em> is updated.
		@returns true on success, or false on failure (archive was not opened, or is opened but does not have write access).
	*/
	virtual bool writeEntries()=0;
	
protected:
	FileStream* m_stream;
	icu::UnicodeString m_path;
	bool m_readable, m_writeable;
};

/**
	Base entry class.
*/
class DUCT_API Entry {
	friend class Archive;
public:
	/**
		Constructor.
	*/
	inline Entry()
		: m_opened(false), m_flags(ENTRYFLAG_NONE), m_dataoffset(0), m_datasize(0)
	{/* Do nothing */}
	/**
		Destructor.
	*/
	inline virtual ~Entry() {/* Do nothing */}
	/**
		Determine if the entry is currently opened.
		@returns true if the entry is opened, or false if the entry is closed.
		@see open(), close()
	*/
	inline bool isOpen() const { return m_opened; }
	/**
		Get the entry's flags.
		@returns The entry's flags.
	*/
	inline uint16_t getFlags() const { return m_flags; }
	/**
		Set the entry's flags.
		@returns Nothing.
		@param flags The entry's new flags.
	*/
	inline void setFlags(uint16_t flags) { m_flags=flags; }
	/**
		Determine if the entry is compressed.
		@returns true if the entry is compressed, or false if the entry is uncompressed.
	*/
	inline bool isCompressed() const { return 0!=(m_flags&ENTRYFLAG_COMPRESSED); }
	/**
		Set the compressed flag for the entry.
		@returns Nothing.
		@param compresse Whether the flag should be on or off.
	*/
	inline void setCompressed(bool compressed) {
		if (compressed) {
			m_flags|=ENTRYFLAG_COMPRESSED;
		} else {
			m_flags&=~ENTRYFLAG_COMPRESSED;
		}
	}
	/**
		Get the entry's data offset.
		@returns the entry's data offset.
	*/
	inline uint64_t getDataOffset() const { return m_dataoffset; }
	/**
		Get the entry's data size.
		@returns The entry's data size.
	*/
	inline unsigned int getDataSize() const { return m_datasize; }
	/**
		Get the entry's const metadata size (14).
		@returns 14.
	*/
	static inline unsigned int getConstMetadataSize() { return 14; }
	/**
		Get the entry's metadata size.
		The base returns 14.
		@returns The size of the entry's <em>entry-metadata</em> section.
	*/
	inline virtual unsigned int getMetadataSize() const { return 14; }
	/**
		Open the entry for reading, using an archive stream.
		If the entry is already opened, this should just return the active stream.
		@returns The stream for the entry's data on success, or NULL if the entry couldn't be opened.
		@param stream The archive stream to read from (will be seeked).
		@see close()
	*/
	virtual Stream* open(Stream* stream)=0;
	/**
		Close the entry's read-only data stream.
		@returns Nothing.
	*/
	virtual void close()=0;
	/**
		Deserialize the entry's metadata from the given stream.
		This will call deserializeUserspace().
		@returns true on success, or false on failure.
		@param stream The archive stream to deserialize from.
	*/
	virtual bool deserialize(Stream* stream);
	/**
		Serialize the entry's metadata to the given stream.
		@returns true on success, or false on failure.
		@param stream The archive stream to serialize to.
	*/
	virtual bool serialize(Stream* stream);
	/**
		Deserialize the entry's <em>user-space</em> section.
		@returns true on success, or false on failure.
		@param stream The archive stream to deserialize from.
	*/
	virtual bool deserializeUserspace(Stream* stream)=0;
	/**
		Serialize the entry's <em>user-space</em> section.
		@returns true on success, or false on failure.
		@param stream The archive stream to serialize to.
	*/
	virtual bool serializeUserspace(Stream* stream)=0;
	/**
		Read the entry's data from the given stream.
		This should seek to the entry's data offset.
		This will likely be used only for non file-container entry types (that is, entries that actually represent and store data, instead of wrapping file data in compression and/or encryption for streamed access).
		@returns true on success, or false on failure.
		@param stream The archive stream to read from.
	*/
	virtual bool read(Stream* stream)=0;
	/**
		Write the entry's data to the given stream.
		This should not seek to the data offset, but rather set it (or it can be handled by the archive).
		@returns true on success, or false on failure.
		@param stream the archive stream to write to.
	*/
	virtual bool write(Stream* stream)=0;
	
protected:
	bool m_opened;
	uint16_t m_flags;
	uint64_t m_dataoffset;
	uint32_t m_datasize;
};

} // namespace duct

#endif // _DUCT_ARCHIVE_HPP

