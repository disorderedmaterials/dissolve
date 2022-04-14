mkdir build
cd build
conan install ..
conan install .. -s build_type=Debug
cmake -DMULTI_THREADING=OFF ..
cmake --build .