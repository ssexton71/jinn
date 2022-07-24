
#pragma once

#include "futil/util/filelist.h"
#include "../model/project.h"

inline futil::util::FileList buildFileList(jinn::model::Project* p) {
	futil::util::FileList list;
	std::vector<std::string> files = p->findAllParams("files").vals;
	for (auto& filespec : files) {
		std::string dir, pat;
		auto pos = filespec.find_last_of('/');
		if (pos == filespec.npos)
			pat = filespec;
		else 
			dir = filespec.substr(0,pos), pat = filespec.substr(pos + 1);
		list.add(dir, pat);
	}
	return list;
}
