
#pragma once

#include <string>

inline std::string fileExtension(const std::string& file) {
	std::string::size_type pos = file.rfind(".");
	return (pos == file.npos)? "" : file.substr(pos);
}
