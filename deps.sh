#!/bin/bash
pushd `dirname $0` > /dev/null
mkdir -p include lib
tar xvf ../pkgs/genie.tar.gz
popd > /dev/null
