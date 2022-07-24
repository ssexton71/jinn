
#pragma once

#include "solution.h"

namespace jinn {

namespace model {

class GlobalScope : public Block {

public:

	void parseFile(const char* f);
	void parse(std::istream&);

	void organize(Block&);

	std::vector<Solution> solutions;
	std::vector<Project> projects;
	std::vector<Configuration> configurations;

};

}}
