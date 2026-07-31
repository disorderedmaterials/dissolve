{ pkgs }:

pkgs.stdenv.mkDerivation rec {
  name = "toml11";
  version = "v4.4.0";
  cmake = true;
  buildInputs = [ pkgs.cmake ];
  src = pkgs.fetchFromGitHub {
    owner = "ToruNiina";
    repo = "toml11";
    rev = version;
    sha256 = "sha256-sgWKYxNT22nw376ttGsTdg0AMzOwp8QH3E8mx0BZJTQ=";
    fetchSubmodules = true;
  };
  cmakeFlags = [
    "-DCMAKE_INSTALL_INCLUDEDIR=include/toml11"
  ];
}
