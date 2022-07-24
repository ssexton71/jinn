
#include "gscope.h"

void jinn::model::GlobalScope::parseFile(const char* f) { 
	Block b;
	b.parseFile(f);
	organize(b);
}

void jinn::model::GlobalScope::parse(std::istream& in) {
	Block b;
	b.parse(in);
	organize(b);
}

void jinn::model::GlobalScope::organize(Block& b) {
	std::vector<Project>* projs = &projects; 
	std::vector<Configuration>* confs = &configurations;
	Block* blk = this;
	Block* cblk = this;
	Param group;

	for (auto& p : b.params) {
		if (p.key == "group") {
			group = p;
		} else if (p.key == "solution") {
			solutions.resize(solutions.size() + 1);
			Solution* soln = &solutions.back();
			soln->key = p.vals[0];
			confs = &soln->configurations;
			cblk = blk = soln;
			projs = &soln->projects;
		} else if (p.key == "project") {
			projs->resize(projs->size() + 1);
			Project* proj = &projs->back();
			proj->key = p.vals[0];
			confs = &proj->configurations;
			cblk = blk = proj;
			if (!group.key.empty()) 
				blk->params.push_back(group);
		} else if (p.key == "configuration") {
			confs->resize(confs->size() + 1);
			Configuration* conf = &confs->back();
			conf->vals = p.vals;
			if (conf->vals.empty())
				blk = cblk;
			else
				blk = conf;
		} else if (blk->params.empty() || !blk->params.back().append(p))
			blk->params.push_back(p);

	}

}
