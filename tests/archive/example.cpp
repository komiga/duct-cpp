
#include "example.hpp"
#include <duct/debug.hpp>
#include <duct/archive.hpp>
#include <duct/filesystem.hpp>

#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <unicode/ustream.h>

MyArchive::MyArchive(icu::UnicodeString const& path)
	: duct::Archive(path)
{}

MyArchive::~MyArchive() {
	close();
	clear();
}

char const* MyArchive::getIdentifier() const {
	return "TEST";
}

size_t MyArchive::getMetadataSize() const {
	return duct::Archive::getMetadataSize()+4; // +entry_count
}

size_t MyArchive::getHeaderSize() const {
	size_t size=getMetadataSize();
	MyEntryList::const_iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		size+=(*iter)->getMetadataSize();
	}
	return size;
}

unsigned int MyArchive::getCount() const {
	return (unsigned int)m_list.size();
}

MyEntryList& MyArchive::getList() {
	return m_list;
}

void MyArchive::clear() {
	MyEntryList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		delete *iter;
	}
	m_list.clear();
}

bool MyArchive::deserializeUserspace() {
	// no special data
	unsigned int entrycount=m_stream->readUInt32();
	for (unsigned int i=0; i<entrycount; ++i) {
		MyEntry* e=new MyEntry();
		if (!e->deserialize(m_stream)) {
			delete e;
			return false;
		}
		m_list.push_back(e);
	}
	return true;
}

bool MyArchive::serializeUserspace() {
	// no special data
	m_stream->writeUInt32(getCount());
	MyEntryList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		if (!(*iter)->serialize(m_stream)) {
			return false;
		}
	}
	return true;
}

bool MyArchive::readEntries() {
	MyEntryList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		if (!(*iter)->read(m_stream)) { // we don't need to seek to the dataoffset - Entry.read() is supposed to
			return false;
		}
	}
	return true;
}

bool MyArchive::writeEntries() {
	m_stream->seek(getHeaderSize());
	MyEntryList::iterator iter;
	for (iter=m_list.begin(); iter!=m_list.end(); ++iter) {
		if (!(*iter)->write(m_stream)) {
			return false;
		}
	}
	return true;
}

void MyArchive::add(MyEntry* e) {
	m_list.push_back(e);
}

// class MyEntry implementation

MyEntry::MyEntry()
	: m_path(), m_data(NULL)
{}

MyEntry::MyEntry(icu::UnicodeString const& path)
	: m_path(), m_data(NULL)
{
	load(path);
}

MyEntry::~MyEntry() {
	freeData();
};

void MyEntry::freeData() {
	if (m_data) {
		free(m_data);
		m_data=NULL;
	}
}

char* MyEntry::getData() {
	return m_data;
}

icu::UnicodeString& MyEntry::getPath() {
	return m_path;
}

unsigned int MyEntry::getMetadataSize() {
	std::string temp;
	m_path.toUTF8String(temp);
	return duct::Entry::getMetadataSize()+2+(unsigned int)temp.size();
}

duct::Stream* MyEntry::open(duct::Stream* stream) {
	throw std::runtime_error("not implemented");
}

void MyEntry::close() {
	throw std::runtime_error("not implemented");
}

bool MyEntry::deserializeUserspace(duct::Stream* stream) {
	uint16_t len=stream->readUInt16();
	char* buf=(char*)malloc(len);
	debug_assertp(buf, this, "unable to allocate string buffer");
	stream->read(buf, len);
	m_path=icu::UnicodeString(buf, len, "utf8");
	free(buf);
	return true;
}

bool MyEntry::serializeUserspace(duct::Stream* stream) {
	std::string temp;
	m_path.toUTF8String(temp);
	stream->writeUInt16((uint16_t)temp.size());
	stream->write(temp.data(), temp.size());
	return true;
}

bool MyEntry::read(duct::Stream* stream) {
	stream->seek(m_dataoffset);
	freeData();
	m_data=(char*)malloc(m_datasize);
	stream->read(m_data, m_datasize);
	return true;
}

bool MyEntry::write(duct::Stream* stream) {
	m_dataoffset=stream->pos();
	if (m_data) {
		stream->write(m_data, m_datasize);
	}
	return true;
}

// load the entry's data from a file
void MyEntry::load(icu::UnicodeString const& path) {
	freeData();
	m_path.setTo(path);
	duct::FileStream* stream=duct::FileStream::openFile(m_path, true, false);
	if (stream) {
		m_datasize=(uint32_t)stream->size();
		m_data=(char*)malloc(m_datasize);
		stream->read(m_data, m_datasize);
		delete stream;
	}
}

void MyEntry::save() {
	duct::FileStream* stream=duct::FileStream::openFile(m_path+".out", false, true);
	if (stream) {
		if (m_data!=NULL) {
			stream->write(m_data, m_datasize);
		}
		delete stream;
	}
}

int main(int argc, char** argv) {
	// create an archive
	MyArchive archout("data/test.arc");
	archout.add(new MyEntry("data/test.txt"));
	if (!archout.save()) {
		printf("Failed to write archive to ");
		std::cout<<archout.getPath()<<std::endl;
		return 1;
	}
	// read it in
	MyArchive archin("data/test.arc");
	if (!archin.open(true, true, false)) {
		printf("Failed to write ");
		std::cout<<archin.getPath()<<std::endl;
		return 1;
	}
	if (!archin.readEntries()) {
		printf("Failed to read entry data\n");
		return 1;
	}
	MyEntryList& list=archin.getList();
	MyEntryList::iterator iter;
	for (iter=list.begin(); iter!=list.end(); ++iter) {
		MyEntry* e=*iter;
		printf("\"");
		std::cout<<e->getPath();
		printf("\" offset=%lu size=%u metadatasize=%u\n", e->getDataOffset(), e->getDataSize(), e->getMetadataSize());
		e->save();
	}
	return 0;
}

