
#define _CRT_SECURE_NO_WARNINGS

#include "fileutil.h"

#ifdef _WIN32
#include <windows.h>
#endif

#if _MSC_VER
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#endif

#include <memory>
#include <cstdio>
#include <cstring>

bool futil::util::copyFile(const char* from, const char* to) {
#ifdef _WIN32
	return 0 != ::CopyFileA(from, to, FALSE);
#elif defined(__linux__)
	bool ok = false;
	int rfd = open(from, O_RDONLY);
	if (rfd >= 0) {
		struct stat st;
		if (fstat(rfd, &st) >= 0) {
			int wfd = open(to, O_WRONLY|O_CREAT|O_TRUNC, st.st_mode);
			if (wfd >= 0) {
				off_t offset = 0;
				ssize_t amt = sendfile(wfd, rfd, &offset, st.st_size);
				ok = (amt == st.st_size);
				close(wfd);
			}
		}
		close(rfd);
	}
	return ok;
#else
	return false;
#endif
}

const char* futil::util::getCurrentDirectory() {
#ifdef _WIN32
	static bool once = true;
	static char buf[1024]; 
	if (once) {
		::GetCurrentDirectoryA(sizeof(buf), buf);
		once = false;
	}
	return buf;
#else
	return "";
#endif
}

unsigned int futil::util::fileSize(const char* f) {
#if _MSC_VER
	struct _stat st;
	if (0 == _stat(f, &st))
		return st.st_size;
#else
	struct stat st;
	if (0 == stat(f, &st))
		return st.st_size;
#endif
	return 0;
}

unsigned char* futil::util::fileContents(const char* f) {
	unsigned int len = fileSize(f);
	unsigned char* ptr = new unsigned char[len];
	FILE* fp = fopen(f, "rb");
	if (fp) { unsigned int amt = (unsigned int) fread(ptr, 1, len, fp); if (amt != len) amt++; fclose(fp); }
	return ptr;
}

#include <iostream>

bool futil::util::fileContentsEqual(const char* f1, const char* f2) {
	unsigned int len = fileSize(f1);
	if (len != fileSize(f2)) return false;
	std::unique_ptr<unsigned char> p1(fileContents(f1));
	std::unique_ptr<unsigned char> p2(fileContents(f2));
#if 0
	for (unsigned int i = 0; i < len; i++) {
		std::cout << i << ": " << p1.get()[i] << p2.get()[i] << std::endl;
		if (p1.get()[i] != p2.get()[i]) {
			std::cout << "FILES ARE DIFFERENT" << std::endl;
			return false;
		}
	}
	std::cout << "FILES ARE IDENTICAL" << std::endl;
	return true;
#else
	return (0 == std::memcmp(p1.get(), p2.get(), len));
#endif
}
