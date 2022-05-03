{ pkgs }:

pkgs.gcc9Stdenv.mkDerivation rec {
  name = "toml11";
  version = "v3.7.1";
  cmake = true;
  buildInputs = [pkgs.cmake];
  src = pkgs.fetchFromGitHub {
    owner = "ToruNiina";
    repo = "toml11";
    rev = version;
    sha256 = "HnhXBvIjo1JXhp+hUQvjs83t5IBVbNN6o3ZGhB4WESQ=";
    fetchSubmodules = true;
  };
  cmakeFlags = [
    "-DCMAKE_INSTALL_INCLUDEDIR=include/toml11"
  ];
}
