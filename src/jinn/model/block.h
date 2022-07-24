
#pragma once

#include "param.h"

#include <iostream>

namespace jinn {

namespace model {

class Block {

public:
	void parseFile(const char*);
	void parse(std::istream&);

	std::vector<Param> params;

	inline std::string find(const char* k) {
		const jinn::model::Param* p = findParam(k);
		if (p && !p->vals.empty()) return p->vals[0];
		return "";
	}

	inline const Param* findParam(const char* k) {
		for(auto& p : params)
			if (p.key == k) return &p;
		return 0;
	}

	inline Param findAllParams(const char* k) {
		Param pp; pp.key = k;
		for(auto& p : params)
			if (p.key == k) pp.append(p);
		return pp;
	}

	inline std::vector<std::string> findVals(const char* k) {
		const jinn::model::Param* p = findParam(k);
		if (p) return p->vals;
		return std::vector<std::string>();
	}

};

class ValuesBlock : public Block {
public:
	std::vector<std::string> vals;
};

class KeyBlock : public Block {
public:
	std::string key;
};

}}
