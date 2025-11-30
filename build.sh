#!/bin/bash
cd "$(dirname "$0")"
cd build
cmake --build . -- -j16