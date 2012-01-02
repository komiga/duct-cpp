
#include "example.hpp"
#include <duct/debug.hpp>
#include <duct/archive.hpp>
#include <duct/filesystem.hpp>

#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <unicode/ustream.h>

MyArchive::MyArchive(const icu::UnicodeString& path) : duct::Archive(path) {
}

MyArchive::~MyArchive() {
	close();
	clear();
}

const char* MyArchive::getIdentifier() const {
	return "TEST";
}

size_t MyArchive::getMetadataSize() const {
	return duct::Archive::getMetadataSize()+4; // +entry_count
}

size_t MyArchive::getHeaderSize() const {
	size_t size=getMetadataSize();
	MyEntryList::const_iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		size+=(*iter)->getMetadataSize();
	}
	return size;
}

unsigned int MyArchive::getCount() const {
	return (unsigned int)_list.size();
}

MyEntryList& MyArchive::getList() {
	return _list;
}

void MyArchive::clear() {
	MyEntryList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		delete *iter;
	}
	_list.clear();
}

bool MyArchive::deserializeUserspace() {
	// no special data
	unsigned int entrycount=_stream->readUInt32();
	for (unsigned int i=0; i<entrycount; ++i) {
		MyEntry* e=new MyEntry();
		if (!e->deserialize(_stream)) {
			delete e;
			return false;
		}
		_list.push_back(e);
	}
	return true;
}

bool MyArchive::serializeUserspace() {
	// no special data
	_stream->writeUInt32(getCount());
	MyEntryList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		if (!(*iter)->serialize(_stream)) {
			return false;
		}
	}
	return true;
}

bool MyArchive::readEntries() {
	MyEntryList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		if (!(*iter)->read(_stream)) { // we don't need to seek to the dataoffset - Entry.read() is supposed to
			return false;
		}
	}
	return true;
}

bool MyArchive::writeEntries() {
	_stream->seek(getHeaderSize());
	MyEntryList::iterator iter;
	for (iter=_list.begin(); iter!=_list.end(); ++iter) {
		if (!(*iter)->write(_stream)) {
			return false;
		}
	}
	return true;
}

void MyArchive::add(MyEntry* e) {
	_list.push_back(e);
}

// class MyEntry implementation

MyEntry::MyEntry() {
}

MyEntry::MyEntry(const icu::UnicodeString& path) {
	load(path);
}

MyEntry::~MyEntry() {
	freeData();
};

void MyEntry::freeData() {
	if (_data) {
		free(_data);
		_data=NULL;
	}
}

char* MyEntry::getData() {
	return _data;
}

icu::UnicodeString& MyEntry::getPath() {
	return _path;
}

unsigned int MyEntry::getMetadataSize() {
	std::string temp;
	_path.toUTF8String(temp);
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
	_path=icu::UnicodeString(buf, len, "utf8");
	free(buf);
	return true;
}

bool MyEntry::serializeUserspace(duct::Stream* stream) {
	std::string temp;
	_path.toUTF8String(temp);
	stream->writeUInt16((uint16_t)temp.size());
	stream->write(temp.data(), temp.size());
	return true;
}

bool MyEntry::read(duct::Stream* stream) {
	stream->seek(_dataoffset);
	freeData();
	_data=(char*)malloc(_datasize);
	stream->read(_data, _datasize);
	return true;
}

bool MyEntry::write(duct::Stream* stream) {
	_dataoffset=stream->pos();
	if (_data) {
		stream->write(_data, _datasize);
	}
	return true;
}

// load the entry's data from a file
void MyEntry::load(const icu::UnicodeString& path) {
	freeData();
	_path.setTo(path);
	duct::FileStream* stream=duct::FileStream::openFile(_path, true, false);
	if (stream) {
		_datasize=(uint32_t)stream->size();
		_data=(char*)malloc(_datasize);
		stream->read(_data, _datasize);
		delete stream;
	}
}

void MyEntry::save() {
	duct::FileStream* stream=duct::FileStream::openFile(_path+".out", false, true);
	if (stream) {
		if (_data!=NULL) {
			stream->write(_data, _datasize);
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

