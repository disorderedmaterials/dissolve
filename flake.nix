{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
    flake-utils.inputs.nixpkgs.follows = "nixpkgs";
    bundler.url = "github:matthewbauer/nix-bundle";
    bundler.inputs.nixpkgs.follows = "nixpkgs";
  };
  outputs = { self, nixpkgs, flake-utils, bundler }:
    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        qt6 = import ./nix/qt6.nix {inherit pkgs;};
        exe-name = mpi: gui:
          if mpi then
            "dissolve-mpi"
          else
            (if gui then "dissolve-gui" else "dissolve");
        version = "0.9.0";
        dissolve = { mpi, gui }:
          assert (!(gui && mpi));
          pkgs.gcc9Stdenv.mkDerivation rec {
            inherit version;
            pname = exe-name mpi gui;
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            patches = [ ./nix/patches/no-conan.patch ];
            buildInputs = with pkgs;
              [
                antlr4
                antlr4.runtime.cpp
                bison
                cmake
                cli11
                fmt
                fmt.dev
                freetype
                jre
                openmpi
                pkgconfig
                pugixml
              ] ++ pkgs.lib.optional mpi pkgs.openmpi
              ++ pkgs.lib.optionals gui [
                pkgs.freetype
                pkgs.ftgl
                pkgs.libGL
                pkgs.libglvnd
                pkgs.libglvnd.dev
              ];

            QTDIR= if gui then "${qt6}/6.1.1/gcc_64" else "";
            Qt6_DIR="${QTDIR}/lib/cmake/Qt6";
            Qt6CoreTools_DIR="${QTDIR}/lib/cmake/Qt6CoreTools";
            Qt6GuiTools_DIR="${QTDIR}/lib/cmake/Qt6GuiTools";
            Qt6WidgetsTools_DIR="${QTDIR}/lib/cmake/Qt6WidgetsTools";
            PATH="${QTDIR}/bin";

            cmakeFlags = [
              "-DMULTI_THREADING=OFF"
              ("-DPARALLEL=" + (if mpi then "ON" else "OFF"))
              ("-DGUI=" + (if gui then "ON" else "OFF"))
            ];
            installPhase = ''
              mkdir -p $out/bin
              ls nix
              mv ./$out/bin/${exe-name mpi gui} $out/bin/
            '';

            meta = with pkgs.lib; {
              description = "";
              homepage = "";
              # license = licenses.unlicense;
              maintainers = [ maintainers.rprospero ];
            };
          };
        mkContainer = { mpi, gui }:
          pkgs.ociTools.buildContainer {
            args = [
              (with pkgs;
                writeScript "run.sh" ''
                  #!${bash}/bin/bash
                  exec ${dissolve { inherit mpi; }}/bin/${exe-name mpi gui}
                '').outPath
            ];
          };
        mkSingularity = { mpi, gui }:
          pkgs.stdenvNoCC.mkDerivation {
            inherit version;
            name = "${exe-name mpi gui}.sif";
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            buildPhase = ''
              ${pkgs.gnutar}/bin/tar czf ${
                exe-name mpi gui
              }.oci.tar.gz --directory=${mkContainer { inherit mpi; }} .
            '';
            installPhase = ''
              mkdir -p $out
              cp ${exe-name mpi gui}.oci.tar.gz $out/
            '';
          };
      in {
        defaultPackage = dissolve {
          mpi = false;
          gui = false;
        };

        packages = {
          dissolve = dissolve {
            mpi = false;
            gui = false;
          };
          dissolve-mpi = dissolve {
            mpi = true;
            gui = false;
          };
          dissolve-gui = dissolve {
            mpi = false;
            gui = true;
          };

          singularity = mkSingularity {
            mpi = false;
            gui = false;
          };
          singularity-mpi = mkSingularity {
            mpi = true;
            gui = false;
          };
          singularity-gui = mkSingularity {
            mpi = false;
            gui = true;
          };

          container = mkContainer {
            mpi = false;
            gui = false;
          };
          container-mpi = mkContainer {
            mpi = true;
            gui = false;
          };
          container-gui = mkContainer {
            mpi = false;
            gui = true;
          };
        };
      });
}
