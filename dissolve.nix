{pkgs ? import <nixpkgs> {},
  unstable ? import <unstable> {},
  parallel ? false,
  gui ? false}:

let
  cmakeBool = x: if x then "ON" else "OFF";
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
    pkgs.conan
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

  src = pkgs.fetchFromGitHub {
    owner = "projectdissolve";
    repo = "dissolve";
    rev = "d58b5d1118afc817d22c04ad4319e0b99ed7dbb2";
    sha256 = "0dwdblnaim9kgrjvvvyz9a8g27izwm6ybcsfa2rc90qg2ys80163";
  };
  installPhase = "cp -r .$prefix $out";
  meta = {
    name = "Dissolve";
    description = "Simulation software for scattering off disordered materials";
    longDescription = "TODO: Write long description";
  };
}
