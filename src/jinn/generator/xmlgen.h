
#pragma once

#include "generator.h"

namespace jinn {

namespace generator {

class XmlGenerator : public Generator {

public:

	virtual void generate(model::GlobalScope*);

};

}}
