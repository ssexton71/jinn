#!/bin/bash
pushd `dirname $0` > /dev/null
b=../pkgs
v=
mkdir -p $b
out=$b/jinn.tar
rm -f $out*
if [ "$OS" == "Windows_NT" ]
then
  tar cf$v $out --transform=s,bin/,tools/,g bin/jinn.{exe*,pdb} bin/lua53.{exe*,dll} jinnfile.lua lua/*.lua jgen.sh README.md LICENSE
else
  tar cf$v $out --transform=s,bin/,tools/,g bin/jinn bin/lua53 jinnfile.lua lua/*.lua jgen.sh README.md LICENSE
fi
echo Compressing...
gzip $out
tar tf $out* | sort 
popd > /dev/null

