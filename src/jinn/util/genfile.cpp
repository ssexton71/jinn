
#include "genfile.h"

#include "../model/block.h"
#include "futil/util/fileutil.h"

jinn::util::GeneratedFile::GeneratedFile(bool testMode) : testMode(testMode) {}
jinn::util::GeneratedFile::~GeneratedFile() { close(); }

void jinn::util::GeneratedFile::init(jinn::model::KeyBlock& k) {
	name = k.key;
	location = k.find("location");
}

bool jinn::util::GeneratedFile::open(std::string ext) {
	return open(name, ext);
}

bool jinn::util::GeneratedFile::open(std::string name, std::string ext) {
	if (ext.empty())
		rpath = location + "/" + name;
	else
		rpath = location + "/" + name + "." + ext;
	tpath = rpath  + ".jinn";
	out.open(tpath);
	if (!out.good()) {
		std::cerr << "Failed to open: " << tpath << std::endl;
		return false;
	}
//		std::cout << tpath << std::endl;
	return true;
}

void jinn::util::GeneratedFile::close() {
	out.close();
	if (!futil::util::fileContentsEqual(tpath.c_str(), rpath.c_str())) {
		if (testMode || futil::util::copyFile(tpath.c_str(), rpath.c_str())) {
			std::cout << "Update " << rpath << std::endl;
		} else {
			std::cerr << "Update Failed! " << rpath << std::endl;
		}
	} else
		std::cout << "Unchanged " << rpath << std::endl;
	if (!testMode)
		if (0 != remove(tpath.c_str()))
			std::cerr << "Delete Failed! " << tpath << std::endl;
}

