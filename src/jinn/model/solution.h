
#pragma once

#include "project.h"

namespace jinn {

namespace model {

class Solution : public ConfigurableKeyBlock {

public:

	std::vector<Project> projects;

	inline std::vector<Project> findProjectReferences(Project* p) {
		std::vector<jinn::model::Project> refs;
		std::vector<std::string> deps = p->findAllParams("links").vals;
		for (auto& dep : deps) {
			for (auto& pp : projects) {
				if (pp.key == dep) {
					refs.push_back(pp);
				}
			}
		}
		return refs;
	}

};

}}
