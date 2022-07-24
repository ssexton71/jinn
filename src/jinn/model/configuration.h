
#pragma once

#include "block.h"

namespace jinn {

namespace model {

class Configuration : public ValuesBlock {

public:

	inline bool matchesConfig(const std::vector<std::string>& selectors) {
		for (auto& val : vals) {
			bool found = false;
			for (auto& sel : selectors)
				if (sel == val) { found = true; break; }
			if (!found) return false;
		}
		return true;
	}

};

}}
