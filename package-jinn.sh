#!/bin/bash
pushd `dirname $0` > /dev/null
b=../pkgs
v=
mkdir -p $b
out=$b/jinn.tar
rm -f $out*
if [ "$OS" == "Windows_NT" ]
then
  tar cf$v $out --transform=s,bin/,tools/,g bin/jinn.{exe*,pdb} bin/lua53.{exe*,dll} 
else
  tar cf$v $out --transform=s,bin/,tools/,g bin/jinn bin/lua53
fi
tar rf$v $out jinnfile.lua lua/*.lua jgen.sh 
tar rf$v $out --transform=s,README.md,docs/jinn/README.md,g README.md
tar rf$v $out --transform=s,LICENSE,docs/jinn/LICENSE,g LICENSE
echo Compressing...
gzip $out
tar tf $out* | sort 
popd > /dev/null

