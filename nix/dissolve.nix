{ pkgs ? import ./linux.nix, unstable ? import ./linux-unstable.nix
, parallel ? false, gui ? false }:

let
  cmakeBool = x: if x then "ON" else "OFF";
  gitignoreSrc = pkgs.fetchFromGitHub {
    owner = "hercules-ci";
    repo = "gitignore";
    # put the latest commit sha of gitignore Nix library here:
    rev = "c4662e662462e7bf3c2a968483478a665d00e717";
    # use what nix suggests in the mismatch message here:
    sha256 = "sha256:1npnx0h6bd0d7ql93ka7azhj40zgjp815fw2r6smg8ch9p7mzdlx";
  };
  inherit (import gitignoreSrc { inherit (pkgs) lib; }) gitignoreSource;

in assert pkgs.lib.asserts.assertMsg (!(gui && parallel))
"The GUI cannot be built in parallel mode";
pkgs.stdenv.mkDerivation {
  name = "dissolve";
  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DGUI=${cmakeBool gui}"
    "-DPARALLEL=${cmakeBool parallel}"
    "-DCMAKE_POLICY_DEFAULT_CMP0025=NEW"
  ];
  patches = [ ./ignore_conan.patch ];
  buildInputs = [
    pkgs.antlr
    pkgs.bison
    pkgs.cmake
    unstable.fmt
    unstable.fmt.dev
    pkgs.ninja
  ] ++ pkgs.lib.optionals parallel [ pkgs.openmpi ] ++ pkgs.lib.optionals gui [
    pkgs.freetype
    pkgs.ftgl
    pkgs.libGL
    pkgs.libglvnd
    pkgs.libglvnd.dev
    pkgs.qt5.full
  ];
  nativeBuildInputs = pkgs.lib.optional gui pkgs.qt5.wrapQtAppsHook;

  src = gitignoreSource ./..;
  installPhase = "cp -r .$prefix $out";
  meta = {
    name = "Dissolve";
    description = "Simulation software for scattering off disordered materials";
    longDescription = "TODO: Write long description";
  };
}
