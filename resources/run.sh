#!/bin/sh
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib:/usr/local/lib:$(pwd):$(pwd)/../lib
export LD_LIBRARY_PATH
export LC_ALL=C
cd "$(dirname "$0")"
./creepMiner
