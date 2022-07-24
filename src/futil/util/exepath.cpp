
#include "exepath.h"

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::string futil::util::get_exe_path(bool dirOnly)
{
	// Get path to executable:
	char szDllName[_MAX_PATH];
	::GetModuleFileNameA(0, szDllName, _MAX_PATH);

	if (dirOnly) {
		char szDrive[_MAX_DRIVE];
		char szDir[_MAX_DIR];
		char szFilename[256];
		char szExt[256];
		_splitpath_s(szDllName, szDrive, sizeof(szDrive), szDir, sizeof(szDir), szFilename, sizeof(szFilename), szExt, sizeof(szExt));
		return std::string(szDrive) + std::string(szDir);
	} else
		return std::string(szDllName);
}

#elif defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>

// see http://developer.apple.com/technotes/tn/tn2015.html

std::string futil::util::get_exe_path(bool dirOnly)
{
	char exe_file[PATH_MAX + 1];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if (mainBundle)
	{
		CFURLRef mainURL;

		if (dirOnly)
			mainURL = CFBundleCopyBundleURL(mainBundle);
		else
			mainURL = CFBundleCopyExecutableURL(mainBundle);

		if (mainURL)
		{
			int ok = CFURLGetFileSystemRepresentation (
				mainURL, (Boolean) true, (UInt8*)exe_file, PATH_MAX
			);

			CFRelease(mainURL);

			if (ok)
			{
				if (dirOnly)
					return std::string(exe_file) + "/";
				else
					return std::string(exe_file);
			}
		}
	}

	throw "get_exe_path failed";
}

#else  /* linux */

#include <errno.h>    // for errno
#include <libgen.h>   // for dirname
#include <limits.h>   // for PATH_MAX
#include <string.h>   // for strerror
#include <unistd.h>  // for readlink

std::string futil::util::get_exe_path(bool dirOnly)
{
	char exe_file[PATH_MAX + 1];
	int size;
	size = readlink("/proc/self/exe", exe_file, PATH_MAX);
	if (size < 0)
	{
		throw strerror(errno);
	}
	else
	{
		exe_file[size] = '\0';
		if (dirOnly)
			return std::string(dirname(exe_file)) + "/";
		else
			return std::string(exe_file);
	}
}
#endif

