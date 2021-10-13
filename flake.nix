{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
    flake-utils.inputs.nixpkgs.follows = "nixpkgs";
    bundler.url = "github:matthewbauer/nix-bundle";
    bundler.inputs.nixpkgs.follows = "nixpkgs";
    weggli.url = "github:googleprojectzero/weggli";
    weggli.flake = false;
  };
  outputs = { self, nixpkgs, flake-utils, bundler, weggli }:
    let
      exe-name = mpi: gui:
        if mpi then
          "dissolve-mpi"
        else
          (if gui then "dissolve-gui" else "dissolve");
      cmake-bool = x: if x then "ON" else "OFF";
      version = "0.9.0";
      base_libs = pkgs:
        with pkgs; [
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
        ];
      gui_libs = pkgs:
        with pkgs; [
          pkgs.freetype
          pkgs.ftgl
          pkgs.libGL
          pkgs.libglvnd
          pkgs.libglvnd.dev
        ];
    in flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        qt6 = import ./nix/qt6.nix { inherit pkgs; };
        dissolve = { mpi ? false, gui ? true }:
          assert (!(gui && mpi));
          pkgs.gcc9Stdenv.mkDerivation rec {
            inherit version;
            pname = exe-name mpi gui;
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            patches = [ ./nix/patches/no-conan.patch ];
            buildInputs = base_libs pkgs ++ pkgs.lib.optional mpi pkgs.openmpi
              ++ pkgs.lib.optionals gui (gui_libs pkgs);

            QTDIR = if gui then "${qt6}/6.1.1/gcc_64" else "";
            Qt6_DIR = "${QTDIR}/lib/cmake/Qt6";
            Qt6CoreTools_DIR = "${QTDIR}/lib/cmake/Qt6CoreTools";
            Qt6GuiTools_DIR = "${QTDIR}/lib/cmake/Qt6GuiTools";
            Qt6WidgetsTools_DIR = "${QTDIR}/lib/cmake/Qt6WidgetsTools";
            PATH = "${QTDIR}/bin";

            cmakeFlags = [
              ("-DMULTI_THREADING=" + (cmake-bool false))
              ("-DPARALLEL=" + (cmake-bool mpi))
              ("-DGUI=" + (cmake-bool gui))
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
                  exec ${dissolve { inherit mpi gui; }}/bin/${exe-name mpi gui}
                '').outPath
            ];
          };
        mkSingularity = { mpi ? false, gui ? false }:
          pkgs.stdenvNoCC.mkDerivation {
            inherit version;
            name = "${exe-name mpi gui}.sif";
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            buildPhase = ''
              ${pkgs.gnutar}/bin/tar czf ${
                exe-name mpi gui
              }.oci.tar.gz --directory=${mkContainer { inherit mpi gui; }} .
            '';
            installPhase = ''
              mkdir -p $out
              cp ${exe-name mpi gui}.oci.tar.gz $out/
            '';
          };
      in {
        checks.dissolve = dissolve { gui = false; };

        defaultPackage = self.packages.${system}.dissolve-gui;

        devShell = pkgs.mkShell rec {
          name = "dissolve-shell";
          buildInputs = base_libs pkgs ++ gui_libs pkgs ++ (with pkgs; [
            ccache
            openmpi
            ccls
            (pkgs.clang-tools.override { llvmPackages = pkgs.llvmPackages; })
            cmake-format
            cmake-language-server
            conan
            distcc
            gdb
            ninja
            valgrind
            ((import ./nix/weggli.nix) {
              inherit pkgs;
              src = weggli;
            })
          ]);
          CMAKE_CXX_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache";
          CMAKE_CXX_FLAGS_DEBUG = "-g -O0";
          CXXL = "${pkgs.stdenv.cc.cc.lib}";
          QTDIR = "${qt6}/6.1.1/gcc_64";
          Qt6_DIR = "${QTDIR}/lib/cmake/Qt6";
          Qt6CoreTools_DIR = "${QTDIR}/lib/cmake/Qt6CoreTools";
          Qt6GuiTools_DIR = "${QTDIR}/lib/cmake/Qt6GuiTools";
          Qt6WidgetsTools_DIR = "${QTDIR}/lib/cmake/Qt6WidgetsTools";
          PATH = "${QTDIR}/bin";
        };

        packages = {
          dissolve = dissolve { gui = false; };
          dissolve-mpi = dissolve { mpi = true; };
          dissolve-gui = dissolve { };

          singularity = mkSingularity { };
          singularity-mpi = mkSingularity { mpi = true; };
          singularity-gui = mkSingularity { gui = true; };
        };
      });
}
