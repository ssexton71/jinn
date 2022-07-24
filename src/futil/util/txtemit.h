
#pragma once

#include <ostream>

namespace futil {

namespace util {

class TextEmitter {
	const char indentChar; const int indentAmount;
public:
	TextEmitter(std::ostream&, const char indentChar, const int indentAmount);

	void emit(const char* value);

	void begin(const char* value);
	void end(const char* value);

protected:
	std::ostream& out;
	std::string indent;
	void begin();
	void end();
};

}}
