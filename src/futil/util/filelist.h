
#pragma once

#ifndef FUTIL_DECLSPEC
#define FUTIL_DECLSPEC
#endif

#include <vector>
#include <string>

namespace futil {

namespace util {

class FUTIL_DECLSPEC FileList {

public:
	void add(const std::string& directory, const std::string& pattern);
	
	std::vector<std::string> files;
};

}}

