#!/bin/bash

# Install dev tools and Qt
sudo yum install -y centos-release-scl
sudo yum install -y devtoolset-9
sudo yum install -y qt-5.15.2-gcc9.3

# Instal prerequisites
sudo pip-3 install conan wheel
sudo yum install -y ftgl-devel ninja-build cmake3 qt5-custom mesa-libGL-devel libuuid-devel

# Create / enter build directory
mkdir build-$$
cd build-$$

# Get ANTLR4 Java binary
wget https://www.antlr.org/download/antlr-4.8-complete.jar

# Configure and build
scl enable devtoolset-9 - << \EOF
conan install ../../ -s compiler.version=9 -s compiler.libcxx=libstdc++11 -o tbb:shared=False
QT_INSTALL_DIR=/opt/qt-5.15.2-gcc9.3
cmake3 ../../ -G Ninja -DGUI:bool=true -DMULTI_THREADING:bool=true -DBUILD_ANTLR_RUNTIME:bool=true -DANTLR_EXECUTABLE=`pwd`/antlr-4.8-complete.jar -DQt5Widgets_DIR:path=${QT_INSTALL_DIR}/lib/cmake/Qt5Widgets
ninja
EOF

