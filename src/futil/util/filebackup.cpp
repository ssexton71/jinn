
#include "filebackup.h"

#include <string>
#include <ctime>
#include <algorithm>

#include "fileutil.h"
#include "futil/util/filelist.h"

static inline void replaceAll(std::string& s, char from, char to) {
	for (std::string::iterator it = s.begin(); it != s.end(); it++) {
		if (from == *it) *it = to;
	}
}

static void prunebackups(const std::string& pat, int count) {
	std::string::size_type pos = pat.rfind('\\');
	std::string dir(pat.substr(0, pos + 1));
	std::string ppat = pat.substr(pos + 1);
	replaceAll(dir, '\\', '/');
	futil::util::FileList filelist;
	filelist.add(dir, ppat);
	std::sort(filelist.files.begin(), filelist.files.end());
	int rcount = int(filelist.files.size()) - count;
	auto it = filelist.files.begin();
	while (rcount > 0) {
		remove(it->c_str());
		it++;
		rcount--;
	}
}

void futil::util::backupFile(const char* src) {
	time_t now; 
	time(&now);
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);
	char buffer [80];
	strftime(buffer,80,"%Y%m%d",&timeinfo);

	std::string dest(src), ext;
	auto p = dest.rfind('.');
	if (p != dest.npos) {
		ext = dest.substr(p);
		dest.erase(p);
	}
	dest += "-";
	dest += buffer;
	dest += ".db";

	if (0 == futil::util::fileSize(dest.c_str())) {
		futil::util::copyFile(src, dest.c_str());
		std::string pat(src);
		pat.resize(pat.size() - ext.size());
		pat += "-*"; pat += ext;
		prunebackups(pat, 7);
	}
}
