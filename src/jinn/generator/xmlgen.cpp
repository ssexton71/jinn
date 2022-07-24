
#include "xmlgen.h"

#include "futil/util/xmlemit.h"
#include "../model/gscope.h"

#include <fstream>

static inline void emit(futil::util::XmlEmitter& e, std::string& key) {
	e.emit("k", key.c_str());
}

static inline void emit(futil::util::XmlEmitter& e, std::vector<std::string>& vals) {
	switch (vals.size()) {
		case 0: e.emit("v", ""); break;
		default: for (auto& v : vals) e.emit("v", v.c_str()); break;
	}
}

static inline void emit(futil::util::XmlEmitter& e, jinn::model::Param* p) {
	switch (p->vals.size()) {
		case 0: e.emit(p->key.c_str(), ""); break;
		case 1: e.emit(p->key.c_str(), p->vals[0].c_str()); break;
		default:
			e.begin(p->key.c_str());
			emit(e, p->vals);
			e.end(p->key.c_str());
			break;
	}
}

static inline void emit(futil::util::XmlEmitter& e, jinn::model::Block& b) {
	for(auto& p : b.params)
		emit(e, &p);
}

static inline void emit(futil::util::XmlEmitter& e, jinn::model::Configuration* c) {
	e.begin("configuration");
	emit(e, c->vals);
	emit(e, *c);
	e.end("configuration");
}

static inline void emit(futil::util::XmlEmitter& e, std::vector<jinn::model::Configuration>& configurations) {
	for(auto& c : configurations) 
		emit(e, &c);
}

static inline void emit(futil::util::XmlEmitter& e, jinn::model::Project* p) {
	e.begin("project");
	emit(e, p->key);
	emit(e, *p);
	emit(e, p->configurations);
	e.end("project");
}

static inline void emit(futil::util::XmlEmitter& e, std::vector<jinn::model::Project>& projects) {
	for(auto& p : projects) 
		emit(e, &p);
}

static inline void emit(futil::util::XmlEmitter& e, jinn::model::Solution* s) {
	e.begin("solution");
	emit(e, s->key);
	emit(e, *s);
	emit(e, s->configurations);
	emit(e, s->projects);
	e.end("solution");
}

static inline void emit(futil::util::XmlEmitter& e, std::vector<jinn::model::Solution>& solutions) {
	for(auto& s : solutions) 
		emit(e, &s);
}

void jinn::generator::XmlGenerator::generate(model::GlobalScope* g) {
	std::ofstream out("jinnfile.xml");
	if (!out.good()) {
		std::cerr << "Failed to open: jinnfile.xml" << std::endl;
		return;
	}

	futil::util::XmlEmitter e(out);

	e.begin("Global");

	emit(e, g->solutions);
	emit(e, g->projects);
	emit(e, g->configurations);
	emit(e, *g );

	e.end("Global");
	
}
