{pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/252bfe0107587d40092057f338e9ffcf7bbd90cb.tar.gz") {overlays = [ (import ./overlays.nix)];},
  unstable ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/e0508c81809b12013bce95562d556b1e672e3541.tar.gz") {overlays = [ (import ./overlays.nix)];},
  parallel ? false,
  gui ? false}:

let
  cmakeBool = x: if x then "ON" else "OFF";
    gitignoreSrc = pkgs.fetchFromGitHub {
    owner = "hercules-ci";
    repo = "gitignore";
    # put the latest commit sha of gitignore Nix library here:
    rev = "c4662e662462e7bf3c2a968483478a665d00e717";
    # use what nix suggests in the mismatch message here:
    sha256 = "sha256:0000000000000000000000000000000000000000000000000000";
    };
    inherit (import gitignoreSrc { inherit (pkgs) lib; }) gitignoreSource;
in

assert pkgs.lib.asserts.assertMsg (!(gui && parallel)) "The GUI cannot be built in parallel mode";
pkgs.stdenv.mkDerivation {
  name = "dissolve";
  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DGUI=${cmakeBool gui}"
    "-DPARALLEL=${cmakeBool parallel}"
  ];
  patches = [./ignore_conan.patch];
  buildInputs = [
    pkgs.antlr
    pkgs.bison
    pkgs.cmake
    unstable.fmt
    unstable.fmt.dev
    pkgs.ninja
  ] ++ pkgs.lib.optionals parallel [
    pkgs.openmpi
  ] ++ pkgs.lib.optionals gui [
    pkgs.freetype
    pkgs.ftgl
    pkgs.libGL
    pkgs.libglvnd
    pkgs.libglvnd.dev
    pkgs.qt5.full
  ];
  nativeBuildInputs = pkgs.lib.optional gui pkgs.qt5.wrapQtAppsHook;

  src = builtins.path { path = ./..; name = "dissolve-source";};
  installPhase = "cp -r .$prefix $out";
  meta = {
    name = "Dissolve";
    description = "Simulation software for scattering off disordered materials";
    longDescription = "TODO: Write long description";
  };
}
