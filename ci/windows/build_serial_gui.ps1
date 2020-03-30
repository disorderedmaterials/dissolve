#!/bin/bash

# Script to build Dissolve GUI (and serial) versions on Windows systems.
# Intended to be called as a job within GitHub Actions.

#
# Configuration
#
# -- Enforce exit on first failure
$ErrorActionPreference = 'Stop'
echo $env:PATH
# -- Store current directory on runner
$location = Get-Location

#
# Install Prerequisites
#
# -- General
choco install -y ninja winflexbison
choco install mingw --version=7.3.0 --allow-downgrade
# -- Qt
# ** Installed via jurplel/install-qt-action@v2.2.1

#
# Build Freetype
#
cd $HOME
git clone 'git://git.sv.nongnu.org/freetype/freetype2.git' freetype-latest
Echo 'Compiling FreeType...'
mkdir freetype-build
cd freetype-build
cmake -G "Ninja" ../freetype-latest -DBUILD_SHARED_LIBS:STRING=ON -DCMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE -DCMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE -DCMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE
ninja

#
# Build FTGL
#
cd $HOME
echo 'Compiling FTGL'
git clone 'https://github.com/frankheckenbach/ftgl.git' ftgl-latest
mkdir ftgl-build
cd ftgl-build
$env:INCLUDE += "$HOME\freetype-2.10.1;"
$env:LIB += "$HOME\freetype-build;"
cmake -G "Ninja" ..\ftgl-latest
ninja

#
# Build
#
Set-Location $location
mkdir build
cd build
$env:INCLUDE += "$HOME\freetype-2.10.1;"
$env:LIB += "$HOME\freetype-build;"
$env:INCLUDE += "$HOME\ftgl-latest\src;"
$env:LIB += "$HOME\ftgl-build\src;"
cmake -G "Ninja" -DGUI:bool=true ../
ninja
