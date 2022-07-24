#!/bin/bash
#
pushd `dirname $0` > /dev/null
if [ "$OS" == "Windows_NT" ]
then
  tools/lua53.exe jinnfile.lua Windows
  tools/jinn xml vs2019
else
  tools/lua53 jinnfile.lua Linux
  tools/jinn xml gmake
fi
popd > /dev/null

