
#include <duct/archive.hpp>
#include <duct/filestream.hpp>

#include <list>
#include <unicode/unistr.h>

class MyEntry;
class MyArchive;

typedef std::list<MyEntry*> MyEntryList;

class MyArchive : public duct::Archive {
public:
	MyArchive(icu::UnicodeString const& path);
	~MyArchive();
	
	char const* getIdentifier() const;
	size_t getMetadataSize() const;
	size_t getHeaderSize() const;
	unsigned int getCount() const;
	MyEntryList& getList();
	
	void clear();
	bool deserializeUserspace();
	bool serializeUserspace();
	bool readEntries();
	bool writeEntries();
	void add(MyEntry* e);
	
private:
	MyEntryList m_list;
};

// This'll handle its own data
class MyEntry : public duct::Entry {
public:
	MyEntry();
	MyEntry(icu::UnicodeString const& path);
	~MyEntry();
	
	void freeData();
	char* getData();
	icu::UnicodeString& getPath();
	unsigned int getMetadataSize();
	
	duct::Stream* open(duct::Stream* stream);
	void close();
	bool deserializeUserspace(duct::Stream* stream);
	bool serializeUserspace(duct::Stream* stream);
	bool read(duct::Stream* stream);
	bool write(duct::Stream* stream);
	void load(icu::UnicodeString const& path);
	void save();
	
private:
	icu::UnicodeString m_path;
	char* m_data;
};

