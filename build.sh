#!/bin/bash
pushd `dirname $0` > /dev/null
pwd
if [ "$OS" == "Windows_NT" ]
then
  cmd /c build-win.bat
else
  make -C src clean all
fi
./package.sh
popd > /dev/null
