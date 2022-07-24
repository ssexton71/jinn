#!/bin/bash
pushd `dirname $0` > /dev/null
./package-jinn.sh
./package-futil.sh
popd > /dev/null

