
#include <duct/archive.hpp>
#include <duct/filestream.hpp>

#include <list>
#include <unicode/unistr.h>

class MyEntry;
class MyArchive;

typedef std::list<MyEntry*> MyEntryList;

class MyArchive : public duct::Archive {
public:
	MyArchive(const icu::UnicodeString& path);
	~MyArchive();
	
	const char* getIdentifier() const;
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
	MyEntryList _list;
};

// This'll handle its own data
class MyEntry : public duct::Entry {
public:
	MyEntry();
	MyEntry(const icu::UnicodeString& path);
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
	void load(const icu::UnicodeString& path);
	void save();
	
private:
	icu::UnicodeString _path;
	char* _data;
};

