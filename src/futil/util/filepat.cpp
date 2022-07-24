
#include "filepat.h"

futil::util::FilePattern::FilePattern(const std::string& pattern) {
	std::string::size_type ppos = pattern.find('*');
	if (ppos == pattern.npos)
		head = pattern;
	else {
		if (ppos != 0)
			head = pattern.substr(0, ppos);
		tail = pattern.substr(pattern.rfind('*') + 1);
	}
}

bool futil::util::FilePattern::match(const std::string& s) {
	if (!head.empty())
		if (s.find(head) != 0) return false;
	if (tail.empty()) return true;
	if (tail.size() > s.size()) return false;
	std::string::size_type tpos = s.size() - tail.size();
	std::string::size_type apos = s.rfind(tail);
	return (tpos == apos);
}

std::string futil::util::FilePattern::extract(const std::string& s) {
	return s.substr(head.size(), s.size() - tail.size() - head.size());
}

