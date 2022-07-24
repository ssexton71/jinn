# jinn - a C++ project file generator

jinn is a project file generator for C++ projects, written in C++.

## Description

jinn is a project file generator for C++ projects, written in C++.  jinn has generators for the following types of projects:

* (Windows) Visual Studio 2017
* (Windows) Visual Studio 2019
* (Linux)   Makefile using g++ 

Input to jinn is a so-called "jinnfile".  The jinnfile format is a simple machine-readable key-value format.  It is not intended to be used directly, but rather created by a preprocessor.  jinn currently supports the following preprocessors:

* (Lua)     premake4/GENie-compatible Lua DSL

It should be noted that while jinn was designed to be a drop-in replacement for premake4/GENie, it only implements a subset of the Lua DSL, and so may not be immediately suitable as a replacement for any given project.  That said, it is designed to be easily extended/modified by C++ programmers; very little knowledge of Lua is required (see lua/jinn.lua for specifics).

## Getting Started

* Download the binary distribution package.
* Modify premake4.lua to suit your needs.
* Run jgen.sh

To build from source:

* Download the source
* Modify to suit your needs
* Run build.sh

The jinn project files are generated by jinn itself.  Although the project files are checked into the git repository to facilitate bootstrapping, it is also possible to use the binary distribution to generate them.

### Dependencies

* (Windows)  Visual Studio 2019, Cygwin
* (Linux)    g++, gmake, bash

bash scripts are used (even on Windows) to make it convenient to run the preprocessor and project generator in one step.  If no bash is available it is trivial to run the commands in jgen.sh and build.sh manually.

## Version History

* 1.0
    * Initial Release

## License

jinn is licensed under the zlib License - see the LICENSE file for details

## Acknowledgments

Thanks to:

* Jess Perkins aka Industrious One for giving us premake4 - a better way to manage C++ builds
* Branimir Karadžić et al. giving us GENie - a better premake4

