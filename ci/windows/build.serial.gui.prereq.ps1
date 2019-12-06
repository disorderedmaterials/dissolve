# Dissolve Windows Build
# One of a sequence of scripts.
# - Install build prerequisites

#$ErrorActionPreference = 'Stop'

# Install prerequisites
choco install -y windows-sdk-10-version-1803-all
choco install -y unzip --version 6.0
choco install -y zip --version 3.0
choco install -y cmake
choco install -y ninja
choco install -y winflexbison
choco install -y innosetup
choco install -y 7zip
choco install -y innounp

# Set environment
$env:PATH += "C:\\Program Files\\CMake\\bin;"

