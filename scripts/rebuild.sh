rm -rf build
cmake -S . --fresh -B build
cmake --build build -- -j4
cmake --install build
