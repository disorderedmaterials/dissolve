{ pkgs ? import <nixpkgs> { }, parallel ? false, gui ? true }:

let
  dissolve = import ./dissolve.nix {
    pkgs = pkgs;
    parallel = parallel;
    gui = gui;
  };

in pkgs.snapTools.makeSnap {
  meta = {
    name = "dissolve";
    summary = dissolve.meta.description;
    description = dissolve.meta.longDescription;
    architecures = [ "amd64" ];
    confinement = "strict";
    apps.cli = {
      command = "${dissolve}/bin/dissolve";
      plugs = [ "home" "removable-media" ];
    };
    apps.dissolve = {
      command = "${dissolve}/bin/dissolve-gui";
      plugs = [ "desktop" "home" "removable-media" "opengl" "wayland" "x11" ];
    };
  };
}
