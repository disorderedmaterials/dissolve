{pkgs ? import <nixos> {},
  mounts ? [],
  parallel ? false,
  gui ? false}:

let
  dissolve = import ./dissolve.nix {pkgs=pkgs; parallel=parallel; gui=gui;};

  ociBase = pkgs.ociTools.buildContainer {
    args = [ "${dissolve}/bin/dissolve" ];
    inherit mounts;
  };
in

# This creates an OCI image of dissolve
# This image can be turned into a SIF with
# > singularity build dissolve.sif result
pkgs.stdenv.mkDerivation {
  name = "dissolve-singularity";
  buildInputs = [
    pkgs.singularity
  ];
  src = ociBase;
  buildPhase = "tar -caf tmp.tar.bz2 $src";
  installPhase = "mv tmp.tar.bz2 $out";
}
