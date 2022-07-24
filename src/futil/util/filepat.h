
#pragma once

#include <string>

namespace futil {

namespace util {

class FilePattern {
	std::string head,tail;
public:
	FilePattern(const std::string& pattern);

	bool match(const std::string& s);

	std::string extract(const std::string& s);
};

}}

