
#pragma once

namespace futil {

namespace util {

bool copyFile(const char* from, const char* to);

unsigned int fileSize(const char* f);
unsigned char* fileContents(const char* f);

bool fileContentsEqual(const char* f1, const char* f2);

const char* getCurrentDirectory();

}}
