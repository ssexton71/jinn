
#include "param.h"

void jinn::model::Param::parse(const std::string& str) {
	key.clear(); vals.clear();
	if (str.empty()) return;
	std::string::const_iterator it = str.begin();
	// skip leading spaces
	while (' ' == *it) { 
		it++; if (it == str.end()) return; 
	}
	// capture all non-space characters
	while (' ' != *it) { 
		key.push_back(*it); 
		it++; if (it == str.end()) return; 
	}
	// skip leading spaces
	while (' ' == *it) { 
		it++; if (it == str.end()) return; 
	}
	while ('"' == *it) {
		it++; if (it == str.end()) return;   // leading quote
		std::string val;
		while ('"' != *it) {
			val.push_back(*it); 
			it++; if (it == str.end()) return; 
		}
		vals.push_back(val);
		it++; if (it == str.end()) return;  // trailing quote
		while (',' == *it) {
			it++; if (it == str.end()) return; 
		}
	}
}

bool jinn::model::Param::append(const Param& other) {
	if (key == other.key) {
		for (auto& v : other.vals)
			vals.push_back(v);
		return true;
	} else
		return false;
}
