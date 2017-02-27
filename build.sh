#!/bin/bash

set -x
sh ./bootstrap || exit 1
autoreconf -fi || exit 1
./configure --disable-static || exit 1
make clean || exit 1
make || exit 1
make -C data convert || exit 1
make check || exit 1
set +x
