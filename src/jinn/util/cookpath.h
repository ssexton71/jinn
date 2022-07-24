
#pragma once

#include <string>

inline std::string makeRelativePath(const std::string& path, std::string root) {

	// generic approach (inspired by https://stackoverflow.com/questions/5772992/get-relative-path-from-two-absolute-paths)
	// 1.  check if path starts with root
	// 2.  if it does not, backtrack the root one level (rfind /), accumulate a ../, and repeat step 1
	// 3.  if it does, return the remainder of the path (everything after the root) plus any accumulated ../

	// root = a/b/c/  path = a/b/d, accum =
	// root = a/b/, path = a/b/d, accum = ../  --> return ../d

	// root = a/b/c/ path = f, accum = 
	// root = a/b/, path = f, accum = ../
	// root = a/, path = f, accum = ../../
	// root = , path = f, accum = ../../..  -> return ../../../f  (rroot case)

	// root = a/b/c/, path = a/b/c/d, accum =  --> return d (base case, above)

	// root = a/b/, path = a/c/d, accum = 

	std::string accum;

	for (;;) {

//		if (root == "/") root = "";

		if (/*root.empty() ||*/ (0 == path.compare(0, root.size(), root))) {
			std::string leaf = path.substr(root.size()); 
			if (!accum.empty())
				if (leaf.empty() || ('/' == leaf[0]))
					accum.resize(accum.size() - 1);
			return accum + leaf;
		}

		if ('/' == *root.rbegin())
			root.resize(root.size() - 1);

		std::string::size_type pos = root.rfind('/');
		if (pos == root.npos)
			root.clear();
		else
			root.erase(pos); // + 1);

		accum += "../";

	}
}

inline bool isAbsolutePath(const std::string& path) {
	if (path.empty()) return false;
	if ('/' == path[0]) return true;
#if _WIN32
	// check for drive letter prefix followed by absolute path
	if (path.size() >= 3)
		if (':' == path[1]) 
			if ('/' == path[2])
				return true;
#endif
	return false;
}

inline void cookPath_(std::string& path, const std::string& root) {
	if (!isAbsolutePath(path))
		//          If relative -> absolute are wanted, one way to do it would
		//          be to convert root to an absolute path (possibly by just
		//          prepending the current directory) and then calling 
		//          makeRelativePath as usual on the two absolute paths
		path = makeRelativePath(path, root);
#if 0
	else {
		// convert absolute paths to relative paths like GENie does
		// there is no real value in this, other than trying to match
		// GENie's output
		std::string aroot(jinn::util::getCurrentDirectory()); 
#ifdef _WIN32
		if (tolower(aroot[0]) == tolower(path[0])) {
			replaceAll(aroot, '\\', '/');
			aroot = aroot.substr(2); path = path.substr(2);
			if ('/' != *aroot.rbegin())
				aroot += "/";
			aroot += root;
			path = makeRelativePath(path, aroot);
		}
#else
		aroot += "/";
		aroot += root;
		path = makeRelativePath(path, aroot);
#endif
	}
#endif
//	replaceAll(path, '/', '\\');
}
