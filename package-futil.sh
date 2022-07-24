#!/bin/bash
pushd `dirname $0` > /dev/null
b=../pkgs
v=
mkdir -p $b
out=$b/futil.tar
rm -f $out*
for a in futil
do
  echo Packaging $a...
  tar rf$v $out --transform=s,src/,include/,g src/$a/*/*.h
if [ "$OS" == "Windows_NT" ]
then
  for c in lib/$a.lib bin/$a.{dll,pdb}
  do
    if [ -f $c ]
    then
	  tar rf$v $out $c
	fi
  done
  for c in bin/$a-s.lib bin/$a-s.pdb
  do
    if [ -f $c ]
    then
	  tar rf$v $out --transform=s,bin/,lib/,g $c
	fi
  done
else
  for c in bin/lib$a.so
  do
    if [ -f $c ]
    then
	  tar rf$v $out $c
	fi
  done
  for c in bin/lib$a-s.a
  do
    if [ -f $c ]
    then
	  tar rf$v $out --transform=s,bin/,lib/,g $c
	fi
  done
fi
done
echo Compressing...
gzip $out
tar tf $out* | sort 
popd > /dev/null

