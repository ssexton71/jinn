
#pragma once

#include <string>
#include <fstream>

namespace jinn {

namespace model { class KeyBlock; }

namespace util {

class GeneratedFile {
	std::string tpath, rpath; bool testMode;
public:
	GeneratedFile(bool testMode);
	~GeneratedFile();

	std::ofstream out;

	std::string location;
	std::string name;

	void init(jinn::model::KeyBlock& k);
	bool open(std::string name, std::string ext);
	bool open(std::string ext);
	void close();

};

}}

