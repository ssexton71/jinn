
#include "txtemit.h"

const int indentAmount = 2; const char indentChar = ' ';

futil::util::TextEmitter::TextEmitter(std::ostream& out, const char indentChar, const int indentAmount) : indentChar(indentChar), indentAmount(indentAmount), out(out) {}

void futil::util::TextEmitter::begin() {
	indent.resize(indent.size() + indentAmount, indentChar);
}

void futil::util::TextEmitter::end() {
	indent.resize(indent.size() - indentAmount);
}

void futil::util::TextEmitter::emit(const char* value) {
	out << indent.c_str() << value << std::endl;
}

void futil::util::TextEmitter::begin(const char* value) {
	emit(value);
	begin();
}

void futil::util::TextEmitter::end(const char* value) {
	end();
	emit(value);
}
