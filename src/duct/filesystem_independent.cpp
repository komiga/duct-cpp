/**
@file filesystem_independent.cpp
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
*/

#include <algorithm>
#include <duct/debug.hpp>
#include <duct/filesystem.hpp>

namespace duct {

// FileSystem implementation

namespace FileSystem {

static const UnicodeString g_ustr_dos_slash('\\');
static const UnicodeString g_ustr_nix_slash('/');

/*bool hasLeadingPeriod(const std::string& path) {
	size_t sla_pos=path.find_last_of('/');
	if (std::string::npos==sla_pos) {
		return 0==path.find_first_of('.');
	} else {
		return sla_pos+1==path.find_first_of('.', sla_pos);
	}
}

bool hasLeadingPeriod(const UnicodeString& path) {
	int32_t sla_pos=path.lastIndexOf('/');
	if (-1==sla_pos) {
		return 0==path.indexOf('.');
	} else {
		return sla_pos+1==path.indexOf('.', sla_pos);
	}
}*/

size_t getPartSplitPos_(const std::string& path, bool allow_leading, size_t& beg_pos) {
	size_t sla_pos=path.find_last_of('/');
	beg_pos=(std::string::npos!=sla_pos) ? sla_pos+1 : 0;
	sla_pos=(std::string::npos!=sla_pos) ? sla_pos : 0;
	size_t dot_pos=path.find_first_of('.', beg_pos);
	if (allow_leading && (0==dot_pos || sla_pos+1==dot_pos)) {
		sla_pos=path.find_first_of('.', dot_pos+1);
		if (std::string::npos!=sla_pos) {
			return sla_pos;
		}
	}
	return dot_pos;
}

int32_t getPartSplitPos_(const UnicodeString& path, bool allow_leading, int32_t& beg_pos) {
	int32_t sla_pos=path.lastIndexOf('/');
	beg_pos=(-1!=sla_pos) ? sla_pos+1 : 0;
	sla_pos=(-1!=sla_pos) ? sla_pos : 0;
	int32_t dot_pos=path.indexOf('.', beg_pos);
	if (allow_leading && (0==dot_pos || sla_pos+1==dot_pos)) {
		sla_pos=path.indexOf('.', dot_pos+1);
		if (-1!=sla_pos) {
			return sla_pos;
		}
	}
	return dot_pos;
}

size_t getExtensionPos_(const std::string& path) {
	size_t dot_pos=path.find_last_of('.');
	if (std::string::npos!=dot_pos) {
		size_t sla_pos=path.find_last_of('/');
		if (std::string::npos==sla_pos || dot_pos>sla_pos) {
			return dot_pos;
		}
	}
	return std::string::npos;
}

int32_t getExtensionPos_(const UnicodeString& path) {
	int32_t dot_pos=path.lastIndexOf('.');
	if (-1!=dot_pos) {
		int32_t sla_pos=path.lastIndexOf('/');
		if (-1==sla_pos || dot_pos>sla_pos) {
			return dot_pos;
		}
	}
	return -1;
}

void normalizePath(std::string& path) {
	if (!path.empty()) {
		std::replace(path.begin(), path.end(), '\\', '/');
	}
}

void normalizePath(UnicodeString& path) {
	if (!path.isEmpty()) {
		path.findAndReplace(g_ustr_dos_slash, g_ustr_nix_slash);
	}
}

void normalizePath(std::string& path, bool trailing_slash) {
	normalizePath(path);
	// don't remove the root path slash, if there is one
	if (1<path.size()) {
		std::string::reverse_iterator it=path.rbegin();
		if (trailing_slash && '/'!=(*it)) {
			path.push_back('/');
		} else if (!trailing_slash && '/'==(*it)) {
			path.erase(--(it.base()));
		}
	}
}

void normalizePath(UnicodeString& path, bool trailing_slash) {
	normalizePath(path);
	// don't remove the root path slash, if there is one
	if (1<path.length()) {
		UChar c=path[path.length()-1];
		if (trailing_slash && '/'!=c) {
			path.append('/');
		} else if (!trailing_slash && '/'==c) {
			path.remove(path.length()-1);
		}
	}
}

void normalizePath(const std::string& path, std::string& result) {
	if (!path.empty()) {
		result.reserve(path.size()+1);
		result.assign(path);
		normalizePath(result);
	} else {
		result.clear();
	}
}

void normalizePath(const UnicodeString& path, UnicodeString& result) {
	if (!path.isEmpty()) {
		result.setTo(path);
		normalizePath(result);
	} else {
		result.remove();
	}
}

void normalizePath(const std::string& path, std::string& result, bool trailing_slash) {
	result.assign(path);
	normalizePath(result, trailing_slash);
}

void normalizePath(const UnicodeString& path, UnicodeString& result, bool trailing_slash) {
	result.setTo(path);
	normalizePath(result, trailing_slash);
}

bool pathHasTrailingSlash(const std::string& path) {
	std::string::const_reverse_iterator it=path.rbegin();
	if (path.rend()!=it && '/'==(*it)) {
		return true;
	}
	return false;
}

bool pathHasTrailingSlash(const UnicodeString& path) {
	if (!path.isEmpty() && '/'==path[path.length()-1]) {
		return true;
	}
	return false;
}

bool pathHasFilename(const std::string& path) {
	if (!path.empty()) {
		size_t sla_pos=path.find_last_of('/');
		if (path.size()-1!=sla_pos) {
			return true;
		}
	}
	return false;
}

bool pathHasFilename(const UnicodeString& path) {
	if (!path.isEmpty()) {
		int32_t sla_pos=path.lastIndexOf('/');
		if (path.length()-1!=sla_pos) {
			return true;
		}
	}
	return false;
}

bool pathHasExtension(const std::string& path) {
	if (!path.empty()) {
		size_t dot_pos=getExtensionPos_(path);
		return std::string::npos!=dot_pos;
	}
	return false;
}

bool pathHasExtension(const UnicodeString& path) {
	if (!path.isEmpty()) {
		int32_t dot_pos=getExtensionPos_(path);
		return -1!=dot_pos;
	}
	return false;
}

bool pathHasLeftPart(const std::string& path, bool allow_leading) {
	if (!path.empty()) {
		size_t beg_pos;
		size_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (std::string::npos==dot_pos) {
			return pathHasFilename(path);
		} else {
			return !(!allow_leading && dot_pos==beg_pos);
		}
	}
	return false;
}

bool pathHasLeftPart(const UnicodeString& path, bool allow_leading) {
	if (!path.isEmpty()) {
		int32_t beg_pos;
		int32_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (-1==dot_pos) {
			return pathHasFilename(path);
		} else {
			return !(!allow_leading && dot_pos==beg_pos);
		}
	}
	return false;
}

bool pathHasRightPart(const std::string& path, bool allow_leading) {
	if (!path.empty()) {
		size_t beg_pos;
		size_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		return std::string::npos!=dot_pos && !(allow_leading && dot_pos==beg_pos);
	}
	return false;
}

bool pathHasRightPart(const UnicodeString& path, bool allow_leading) {
	if (!path.isEmpty()) {
		int32_t beg_pos;
		int32_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		return -1!=dot_pos && !(allow_leading && dot_pos==beg_pos);
	}
	return false;
}

bool pathHasDirectory(const std::string& path) {
	if (!path.empty()) {
		size_t sla_pos=path.find_last_of('/');
		if (std::string::npos!=sla_pos) {
			return true;
		}
	}
	return false;
}

bool pathHasDirectory(const UnicodeString& path) {
	if (!path.isEmpty()) {
		int32_t sla_pos=path.lastIndexOf('/');
		if (-1!=sla_pos) {
			return true;
		}
	}
	return false;
}

bool extractFileExtension(const std::string& path, std::string& result, bool include_period) {
	if (pathHasExtension(path)) {
		size_t dot_pos=path.find_last_of('.');
		result.assign(path, include_period ? dot_pos : dot_pos+1, std::string::npos);
		return true;
	}
	return false;
}

bool extractFileExtension(const UnicodeString& path, UnicodeString& result, bool include_period) {
	if (pathHasExtension(path)) {
		int32_t dot_pos=path.lastIndexOf('.');
		result.setTo(path, include_period ? dot_pos : dot_pos+1);
		return true;
	}
	return false;
}

bool extractFileDirectory(const std::string& path, std::string& result, bool trailing_slash) {
	if (pathHasDirectory(path)) {
		size_t sla_pos=path.find_last_of('/');
		result.assign(path, 0, (trailing_slash || sla_pos==0) ? sla_pos+1 : sla_pos);
		return true;
	}
	return false;
}

bool extractFileDirectory(const UnicodeString& path, UnicodeString& result, bool trailing_slash) {
	if (pathHasDirectory(path)) {
		int32_t sla_pos=path.lastIndexOf('/');
		result.setTo(path, 0, (trailing_slash || sla_pos==0) ? sla_pos+1 : sla_pos);
		return true;
	}
	return false;
}

bool extractFilename(const std::string& path, std::string& result, bool with_extension) {
	if (pathHasFilename(path)) {
		size_t sla_pos=path.find_last_of('/');
		size_t dot_pos=with_extension ? std::string::npos : getExtensionPos_(path);
		sla_pos=(std::string::npos==sla_pos) ? 0 : sla_pos+1;
		result.assign(path, sla_pos, dot_pos<sla_pos ? std::string::npos : dot_pos-sla_pos);
		return true;
	}
	return false;
}

bool extractFilename(const UnicodeString& path, UnicodeString& result, bool with_extension) {
	if (pathHasFilename(path)) {
		int32_t sla_pos=path.lastIndexOf('/');
		int32_t dot_pos=with_extension ? INT32_MAX : getExtensionPos_(path);
		sla_pos=(-1==sla_pos) ? 0 : sla_pos+1;
		result.setTo(path, sla_pos, dot_pos<sla_pos ? INT32_MAX : dot_pos-sla_pos);
		return true;
	}
	return false;
}

bool extractFileLeftPart(const std::string& path, std::string& result, bool allow_leading) {
	if (!path.empty()) {
		size_t beg_pos;
		size_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (std::string::npos==dot_pos) {
			return extractFilename(path, result, false);
		} else if (dot_pos==beg_pos) {
			if (!allow_leading) {
				return false;
			} else {
				dot_pos=path.find_first_of('.', dot_pos+1);
			}
		}
		result.assign(path, beg_pos, (std::string::npos==dot_pos) ? std::string::npos : dot_pos-beg_pos);
		return true;
	}
	return false;
}

bool extractFileLeftPart(const UnicodeString& path, UnicodeString& result, bool allow_leading) {
	if (!path.isEmpty()) {
		int32_t beg_pos;
		int32_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (-1==dot_pos) {
			return extractFilename(path, result, false);
		} else if (dot_pos==beg_pos) {
			if (!allow_leading) {
				return false;
			} else {
				dot_pos=path.indexOf('.', dot_pos+1);
			}
		}
		result.setTo(path, beg_pos, (-1==dot_pos) ? INT32_MAX : dot_pos-beg_pos);
		return true;
	}
	return false;
}

bool extractFileRightPart(const std::string& path, std::string& result, bool include_period, bool allow_leading) {
	if (!path.empty()) {
		size_t beg_pos;
		size_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (std::string::npos!=dot_pos && (!allow_leading || (allow_leading && dot_pos!=beg_pos))) {
			result.assign(path, include_period ? dot_pos : dot_pos+1, std::string::npos);
			return true;
		}
	}
	return false;
}

bool extractFileRightPart(const UnicodeString& path, UnicodeString& result, bool include_period, bool allow_leading) {
	if (!path.isEmpty()) {
		int32_t beg_pos;
		int32_t dot_pos=getPartSplitPos_(path, allow_leading, beg_pos);
		if (-1!=dot_pos && (!allow_leading || (allow_leading && dot_pos!=beg_pos))) {
			result.setTo(path, include_period ? dot_pos : dot_pos+1, INT32_MAX);
			return true;
		}
	}
	return false;
}

bool getWorkingDir(std::string& result, bool trailing_slash) {
	if (getWorkingDir(result)) {
		normalizePath(result, trailing_slash);
		return true;
	}
	return false;
}

bool getWorkingDir(icu::UnicodeString& result, bool trailing_slash) {
	if (getWorkingDir(result)) {
		normalizePath(result, trailing_slash);
		return true;
	}
	return false;
}

} // namespace FileSystem

} // namespace duct

