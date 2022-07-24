#!/bin/bash
#
pushd `dirname $0` > /dev/null
if [ "$OS" == "Windows_NT" ]
then
  tools/genie.exe vs2017
  cp src/jinn.sln{,.genie}
  cp src/jinn/jinn.vcxproj{,.genie}
  cp src/jinn/jinn.vcxproj.filters{,.genie}
else
  chmod +x tools/genie
  tools/genie gmake
  cp src/Makefile{,.genie}
  cp src/jinn/Makefile{,.genie}
  cp src/futil/Makefile{,.genie}
fi
popd > /dev/null

