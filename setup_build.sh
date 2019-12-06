#!/bin/bash
git submodule update --remote --recursive --init
mkdir -p ./build
cd build
cmake .. ${@}