{pkgs ? import <nixos> {},
  parallel ? false,
  gui ? false}:

let
  dissolve = import ./dissolve.nix {pkgs=pkgs; parallel=parallel; gui=gui;};
in

pkgs.dockerTools.buildImage {
  name = "dissolve";
  tag = "WIP";
  contents = dissolve;
  created = "now";
  config = {
    Cmd = ["${dissolve}/bin/dissolve"];
    WorkingDir = "/data";
    Volumes = {
      "/data" = {};
    };
  };
}
