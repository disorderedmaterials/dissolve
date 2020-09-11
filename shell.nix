let
  pkgs = import <nixpkgs> {};
  dissolve = import ./nix/dissolve.nix {pkgs=pkgs;};
in
pkgs.mkShell {
  inputsFrom = [dissolve];
  buildInputs = [
    pkgs.ccls
    pkgs.clang-tools
    pkgs.cmake
    pkgs.conan
    pkgs.distcc
    pkgs.valgrind
  ];
}
