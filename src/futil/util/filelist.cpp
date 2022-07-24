
#include "filelist.h"

#ifdef _WIN32
#include "dirent_win32.h"
#include <io.h>
#include <windows.h>
#else
#include <dirent.h>
#endif

#include "filepat.h"

inline bool skip(const char* name) {
	if (name[0] == '.') {
		if (name[1] == 0) return true;
		if (name[1] == '.') {
			if (name[2] == 0) return true;
		}
	}
	return false;
}

void futil::util::FileList::add(const std::string& directory, const std::string& pattern) {

	FilePattern pat(pattern);

	std::string base(directory), delim("/");
	if (base.empty()) base = ".", delim = "";

	bool recurse = (pattern.find("**") != pattern.npos);

	DIR* dir = opendir(base.c_str());
	if (!dir) return;

	for(dirent* ent = 0;;) {
		ent = readdir(dir);
		if (!ent) break;
#ifdef _WIN32
		if (ent->d_info->attrib & (_A_HIDDEN|_A_SYSTEM))
			continue;
#endif
		if (skip(ent->d_name))
			continue;
#ifdef _WIN32
		bool isDirectory = ((ent->d_info->attrib & _A_SUBDIR) != 0);
#else
		bool isDirectory = (ent->d_type == DT_DIR);
#endif
		if (isDirectory) {
			if (recurse)
				add(directory + "/" + ent->d_name, pattern);
		} else {
			if (pat.match(ent->d_name)) 
				files.push_back(directory + delim + ent->d_name);
		}
	}
	closedir(dir);
}

