
#pragma once

#include <string>
#include <vector>

namespace jinn {

namespace model {

class Param {
public:
	void parse(const std::string& str);

	std::string key;
	std::vector<std::string> vals;

	bool append(const Param&);

};

}}