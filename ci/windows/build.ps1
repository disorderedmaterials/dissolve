# Dissolve Build Script
# For use with associated Docker container

# Add binary paths
$env:PATH += "C:\Qt\Tools\mingw730_64\bin;"
$env:PATH += "C:\Qt\5.13.1\mingw73_64\bin;"

# Add include paths
$env:INCLUDE += "C:\freetype-2.10.1\include;C:\ftgl-latest\src;"
# $env:INCLUDE += "C:\Qt\5.13.1\mingw73_64\include;"

# Add missing library paths
$env:LIB += "C:\build\freetype;C:\build\ftgl\src;"

# Obtain latest source
git clone