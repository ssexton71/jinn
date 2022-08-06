
#pragma once

#include "configuration.h"

namespace jinn {

namespace model {

class ConfigurableKeyBlock : public KeyBlock {

public:
	std::vector<Configuration> configurations;

	inline Param findConfigVals(const char* k, const std::vector<std::string>& selectors) {
		Param p; p.key = k;
		const Param* p0 = findParam(k);
		if (p0) p.append(*p0);
		for (auto& config : configurations) {
			if (!config.matchesConfig(selectors))
				continue;
			const Param* pp = config.findParam(k);
			if (pp) p.append(*pp);
		}
		return p;
	}

	inline Param findAllConfigVals(const char* k, const std::vector<std::string>& selectors) {
		Param p; p = findAllParams(k);
		for (auto& config : configurations) {
			if (!config.matchesConfig(selectors))
				continue;
			Param pp = config.findAllParams(k);
			p.append(pp);
		}
		return p;
	}

	inline Param findAllConfigVals(jinn::model::ConfigurableKeyBlock& parent, const char* k, const std::vector<std::string>& selectors) {
		jinn::model::Param p1 = parent.findAllConfigVals(k, selectors);
		jinn::model::Param p2 = findAllConfigVals(k, selectors);
		p1.append(p2);
		return p1;
	}

	inline jinn::model::Param findConfigVals(jinn::model::ConfigurableKeyBlock& parent, const char* k, const std::vector<std::string>& selectors) {
		jinn::model::Param p1 = parent.findConfigVals(k, selectors);
		jinn::model::Param p2 = findConfigVals(k, selectors);
		p1.append(p2);
		return p1;
	}

	inline std::string findConfigVal(jinn::model::ConfigurableKeyBlock& parent, const char* k, const std::vector<std::string>& selectors) {
		jinn::model::Param p = findConfigVals(parent, k, selectors);
		return p.vals.empty()? "" : p.vals.back();
	}

	inline bool findAllFlags(jinn::model::ConfigurableKeyBlock& parent, const char* fl, const std::vector<std::string>& selectors) {
		jinn::model::Param p = findAllConfigVals(parent, "flags", selectors);
		for (auto& val : p.vals)
			if (val == fl) return true;
		return false;
	}


};

}}
