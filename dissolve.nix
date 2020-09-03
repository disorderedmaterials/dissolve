{pkgs ? import <nixos> {},
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
  buildInputs = [
    pkgs.antlr
    pkgs.bison
    pkgs.cmake
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
    rev = "e8da2be5c0ca2026464431f7c9cc9d58980bd15f";
    sha256 = "0fxbh4d4wbapv1yhjm7vb9xnnv9h4v22k8x9jpmd3xvc0i687g42";
  };
  installPhase = "cp -r .$prefix $out";
  meta = {
    name = "Dissolve";
    description = "Simulation software for scattering off disordered materials";
    longDescription = "TODO: Write long description";
  };
}
