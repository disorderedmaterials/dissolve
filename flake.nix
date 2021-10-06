{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        dissolve = pkgs.stdenvNoCC.mkDerivation {
          pname = "dissolve";
          version = "0.9.0";
          src = ./.;
          patches = [ ./nix/patches/no-conan.patch ];
          buildInputs = with pkgs; [
            antlr4
            antlr4.runtime.cpp
            bison
            cmake
            cli11
            fmt
            freetype
            ftgl
            jre
            libGL
            libglvnd
            libglvnd.dev
            ninja
            openmpi
            pugixml
          ];
          cmakeFlags = [ "-G Ninja" ];
          buildPhase = ''
            ${pkgs.ninja}/bin/ninja -j4
          '';
          CC = "${pkgs.gcc9}/bin/gcc";
          CXX = "${pkgs.gcc9}/bin/g++";
          TBB_LIBRARIES = "${pkgs.tbb}/lib";
          meta = with pkgs.lib; {
            description = "";
            homepage = "";
            # license = licenses.unlicense;
            maintainers = [ maintainers.rprospero ];
          };
        };
      in { defaultPackage = dissolve; });
}
