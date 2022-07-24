
#pragma once

#include "txtemit.h"

namespace futil {

namespace util {

class XmlEmitter : public TextEmitter {
public:
	XmlEmitter(std::ostream&);

	void begin(const char* tag);
	void end(const char* tag);

	inline void emit(const char* value) { TextEmitter::emit(value); }

	void emit(const char* tag, const char* value);

};

}}
