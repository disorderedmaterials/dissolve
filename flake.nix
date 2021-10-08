{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        exe-name = mpi: if mpi then "dissolve-mpi" else "dissolve";
        version = "0.9.0";
        dissolve = { mpi }:
          pkgs.gcc9Stdenv.mkDerivation {
            inherit version;
            pname = exe-name mpi;
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
              ] ++ pkgs.lib.optional mpi pkgs.openmpi;
            cmakeFlags = [
              "-DMULTI_THREADING=OFF"
              ("-DPARALLEL=" + (if mpi then "ON" else "OFF"))
            ];
            installPhase = ''
              mkdir -p $out/bin
              ls nix
              mv ./$out/bin/${exe-name mpi} $out/bin/
            '';

            meta = with pkgs.lib; {
              description = "";
              homepage = "";
              # license = licenses.unlicense;
              maintainers = [ maintainers.rprospero ];
            };
          };
        mkContainer = { mpi }:
          pkgs.ociTools.buildContainer {
            args = [
              (with pkgs;
                writeScript "run.sh" ''
                  #!${bash}/bin/bash
                  exec ${dissolve { inherit mpi; }}/bin/${exe-name mpi}
                '').outPath
            ];
          };
        mkSingularity = { mpi }:
          pkgs.stdenvNoCC.mkDerivation {
            inherit version;
            name = "${exe-name mpi}.sif";
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            buildPhase = ''
              ${pkgs.gnutar}/bin/tar czf ${
                exe-name mpi
              }.oci.tar.gz --directory=${mkContainer { inherit mpi; }} .
            '';
            installPhase = ''
              mkdir -p $out
              cp ${exe-name mpi}.oci.tar.gz $out/
            '';
          };
      in {
        defaultPackage = dissolve { mpi = false; };

        packages = {
          dissolve = dissolve { mpi = false; };
          dissolve-mpi = dissolve { mpi = true; };

          singularity = mkSingularity { mpi = false; };
          singularity-mpi = mkSingularity { mpi = true; };

          container = mkContainer { mpi = false; };
          container-mpi = mkContainer { mpi = true; };
        };
      });
}
