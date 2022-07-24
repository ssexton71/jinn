
#include "xmlemit.h"

futil::util::XmlEmitter::XmlEmitter(std::ostream& out) : TextEmitter(out, ' ', 2) {}

void futil::util::XmlEmitter::begin(const char* tag) {
	out << indent.c_str() << "<" << tag << ">" << std::endl;
	TextEmitter::begin();
}

void futil::util::XmlEmitter::end(const char* tag) {
	TextEmitter::end();
	out << indent.c_str() << "</" << tag << ">" << std::endl;
}

void futil::util::XmlEmitter::emit(const char* tag, const char* value) {
	out << indent.c_str() << "<" << tag << ">" << value << "</" << tag << ">" << std::endl;
}
