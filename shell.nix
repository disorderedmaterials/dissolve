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
    pkgs.distcc
    pkgs.valgrind
  ];
}
