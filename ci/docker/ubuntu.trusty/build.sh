#!/bin/bash

# Enable verbosity and erroring
set -ev

# Point to up-to-date Qt libs (in /opt)
# /opt/qt510/bin/qt510-env.sh
QT_BASE_DIR=/opt/qt510
export QTDIR=$QT_BASE_DIR
export PATH=$QT_BASE_DIR/bin:$PATH

if [[ $(uname -m) == "x86_64" ]]; then
  export LD_LIBRARY_PATH=$QT_BASE_DIR/lib/x86_64-linux-gnu:$QT_BASE_DIR/lib:$LD_LIBRARY_PATH
else
  export LD_LIBRARY_PATH=$QT_BASE_DIR/lib/i386-linux-gnu:$QT_BASE_DIR/lib:$LD_LIBRARY_PATH
fi

export PKG_CONFIG_PATH=$QT_BASE_DIR/lib/pkgconfig:$PKG_CONFIG_PATH

# Configure
cd dissolve && ./autogen.sh && ./configure --with-gui --with-qtmoc=moc --with-qtrcc=rcc --with-qtuic=uic

# Build
make
