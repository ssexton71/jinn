
#pragma once

namespace jinn {

namespace model { class GlobalScope; }

namespace generator {

class Generator {

public:
	virtual ~Generator() {}

	virtual void generate(model::GlobalScope*) = 0;

};

}}
