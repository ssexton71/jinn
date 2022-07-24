
#include <fstream>
#include <string>
#include <iostream>
#include <map>

#include "model/gscope.h"
#include "generator/generator.h"
#include "generator/xmlgen.h"
#include "generator/vsgen.h"
#include "generator/gmakegen.h"

int main(int argc, const char** argv) {

	jinn::model::GlobalScope g;
	g.parseFile("jinnfile.jinn");

	std::map<std::string, jinn::generator::Generator*> gens;
	gens["xml"] = new jinn::generator::XmlGenerator;

	struct {
		const char* key;
		const char* platformToolset;
	} vsVers[] = {
		{ "vs2017", "v141" },
		{ "vs2019", "v142" },
	};

	for(auto vsVer : vsVers) {
		std::string key(vsVer.key);
		jinn::generator::VsGenerator* vsGen;
		vsGen = new jinn::generator::VsGenerator(false);
		vsGen->vsVersion = vsVer.key;
		vsGen->platformToolset = vsVer.platformToolset;
		gens[key] = vsGen;

		key += ".test";
		vsGen = new jinn::generator::VsGenerator(true);
		vsGen->vsVersion = vsVer.key;
		vsGen->platformToolset = vsVer.platformToolset;
		gens[key] = vsGen;
	}

	gens["gmake"] = new jinn::generator::GMakeGenerator(false);
	gens["gmake.test"] = new jinn::generator::GMakeGenerator(true);

	if (!argv[1]) {
		std::cout << "Available Generators:" << std::endl;
		for(auto& gen : gens)
			std::cout << gen.first << std::endl;
	}

	for (int i = 1; argv[i]; i++) {
		jinn::generator::Generator* gen = gens[argv[i]];
		if (gen) {
			std::cout << argv[i] << "..." << std::endl;
			gen->generate(&g);
		} else
			std::cerr << argv[i] << ": not found" << std::endl;
	}

	return 0;
}
