let
  pkgs = import <nixpkgs> {};
  dissolve = import ./dissolve.nix {pkgs=pkgs;};
in
pkgs.mkShell {
  inputsFrom = [dissolve];
  buildInputs = [
    pkgs.ccls
    pkgs.clang-tools
    pkgs.cmake
    pkgs.distcc
    pkgs.valgrind
  ];
}
