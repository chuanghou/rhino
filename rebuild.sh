rm -rf build
cmake -S . -B build -DUSE_FETCH_CACHE=TRUE
cmake --build build -- -j4
cmake --install build