
#pragma once

#include "generator.h"

namespace futil {
	namespace util { class FileList; }
}

namespace jinn {

namespace model { class Project; class Solution; }

namespace generator {

class GMakeGenerator : public Generator {
	bool testMode;
	inline bool generateProject(jinn::model::Project* p, jinn::model::Solution* s, futil::util::FileList&);
	inline bool generate(jinn::model::Project* p, jinn::model::Solution* s);
	inline bool generateProjects(jinn::model::Solution* s);
	inline bool generate(jinn::model::Solution* s);
	inline bool generateSolutions(jinn::model::GlobalScope*);

public:
	GMakeGenerator(bool testMode) : testMode(testMode) {}

	virtual void generate(model::GlobalScope*);

};

}}
