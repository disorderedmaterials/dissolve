# Dissolve Build Script
# # One of a sequence of scripts.
# - Build Dissolve

#$ErrorActionPreference = 'Stop'

# Build Dissolve
mkdir build
cd build
cmake -G "Ninja" -DGUI:bool=true ../
ninja
