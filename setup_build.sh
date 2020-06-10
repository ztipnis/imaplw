#!/bin/bash
git submodule update --remote --recursive --init
mkdir -p ./build
mkdir -p ./build/bin
source ./snakeoil.sh
cp ./exampleConfig.conf ./build/bin/config.conf
pushd build
cmake ${@} ..
popd
