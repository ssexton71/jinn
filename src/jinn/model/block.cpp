
#include "block.h"


void jinn::model::Block::parse(std::istream& in) {
	while (in.good()) {
		std::string str;
		getline(in, str);
//		std::cout << str << std::endl;
		if (str.empty())
			continue;
		jinn::model::Param p;
		p.parse(str);
		params.push_back(p);
	}
}

#include <fstream>

void jinn::model::Block::parseFile(const char* filename) {
	std::ifstream in("jinnfile.jinn");
	parse(in);
}
