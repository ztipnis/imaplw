#!/bin/bash
LIBRESSL_ROOT_DIR=/usr/local/opt/libressl ./setup_build.sh -DBoost_NO_BOOST_CMAKE=ON ${@}
pushd ./build
cmake --build .
popd
