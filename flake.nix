{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-21.11";
    outdated.url = "github:NixOS/nixpkgs/nixos-21.05";
    flake-utils.url = "github:numtide/flake-utils";
    flake-utils.inputs.nixpkgs.follows = "nixpkgs";
    bundler.url = "github:matthewbauer/nix-bundle";
    bundler.inputs.nixpkgs.follows = "nixpkgs";
    nixGL-src.url = "github:guibou/nixGL";
    nixGL-src.flake = false;
    weggli.url = "github:googleprojectzero/weggli";
    weggli.flake = false;
  };
  outputs =
    { self, nixpkgs, outdated, flake-utils, bundler, nixGL-src, weggli }:
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
          antlr4.runtime.cpp.dev
          cmake
          cli11
          fmt_8
          fmt_8.dev
          freetype
          inetutils # for rsh
          jre
          pkgconfig
          pugixml
        ];
      gui_libs = pkgs:
        with pkgs; [
          glib
          freetype
          ftgl
          libGL.dev
          libglvnd
          libglvnd.dev
        ];
      check_libs = pkgs: with pkgs; [ gtest ];

    in flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};
        nixGL = import nixGL-src { inherit pkgs; };
        QTDIR = "${import ./nix/qt6.nix { inherit pkgs; }}/6.2.2/gcc_64";
        dissolve =
          { mpi ? false, gui ? true, threading ? true, checks ? false }:
          assert (!(gui && mpi));
          pkgs.gcc9Stdenv.mkDerivation ({
            inherit version;
            pname = exe-name mpi gui;
            src =
              builtins.filterSource (path: type: baseNameOf path != "flake.nix")
              ./.;
            patches = [ ./nix/patches/ctest.patch ];
            buildInputs = base_libs pkgs ++ pkgs.lib.optional mpi pkgs.openmpi
              ++ pkgs.lib.optionals gui (gui_libs pkgs)
              ++ pkgs.lib.optionals checks (check_libs pkgs)
              ++ pkgs.lib.optional threading pkgs.tbb;
            nativeBuildInputs = [ pkgs.wrapGAppsHook ];

            TBB_DIR = "${pkgs.tbb}";
            CTEST_OUTPUT_ON_FAILURE = "ON";

            cmakeFlags = [
              "-DAntlrRuntime_INCLUDE_DIRS=${pkgs.antlr4.runtime.cpp.dev}/include/antlr4-runtime"
              "-DAntlrRuntime_LINK_DIRS=${pkgs.antlr4.runtime.cpp}/lib"
              "-DCONAN=OFF"
              ("-DMULTI_THREADING=" + (cmake-bool threading))
              ("-DPARALLEL=" + (cmake-bool mpi))
              ("-DGUI=" + (cmake-bool gui))
              "-DBUILD_SYSTEM_TESTS:bool=${cmake-bool checks}"
              "-DBUILD_UNIT_TESTS:bool=${cmake-bool (checks && !mpi)}"
              ("-DCMAKE_BUILD_TYPE=" + (if checks then "Debug" else "Release"))
            ] ++ pkgs.lib.optional threading
              ("-DTHREADING_LINK_LIBS=${pkgs.tbb}/lib/libtbb.so");
            doCheck = checks;
            installPhase = ''
              mkdir -p $out/bin
              ls nix
              mv ./$out/bin/* $out/bin/
            '';

            meta = with pkgs.lib; {
              description = "";
              homepage = "";
              # license = licenses.unlicense;
              maintainers = [ maintainers.rprospero ];
            };
          } // (if gui then {
            inherit QTDIR;
            Qt6_DIR = "${QTDIR}/lib/cmake/Qt6";
            Qt6CoreTools_DIR = "${QTDIR}/lib/cmake/Qt6CoreTools";
            Qt6GuiTools_DIR = "${QTDIR}/lib/cmake/Qt6GuiTools";
            Qt6WidgetsTools_DIR = "${QTDIR}/lib/cmake/Qt6WidgetsTools";

          } else
            { }))
          // (if checks then { QT_QPA_PLATFORM = "offscreen"; } else { });
        mkSingularity = { mpi ? false, gui ? false, threading ? true }:
          outdated.legacyPackages.${system}.singularity-tools.buildImage {
            name = "${exe-name mpi gui}-${version}";
            diskSize = 1024 * 25;
            contents = [ (dissolve { inherit mpi gui threading; }) ];
            runScript = if gui then
              "${nixGL.nixGLIntel}/bin/nixGLIntel ${
                dissolve { inherit mpi gui threading; }
              }/bin/${exe-name mpi gui}"
            else
              "${dissolve { inherit mpi gui threading; }}/bin/${
                exe-name mpi gui
              }";
          };
      in {
        checks.dissolve = dissolve { checks = true; };
        checks.dissolve-mpi = dissolve {
          mpi = true;
          gui = false;
          checks = true;
        };
        checks.dissolve-threadless = dissolve {
          threading = false;
          gui = false;
          checks = true;
        };

        defaultPackage = self.packages.${system}.dissolve-gui;

        devShell = pkgs.gcc9Stdenv.mkDerivation {
          name = "dissolve-shell";
          buildInputs = base_libs pkgs ++ gui_libs pkgs ++ check_libs pkgs
            ++ (with pkgs; [
              (pkgs.clang-tools.override { llvmPackages = pkgs.llvmPackages; })
              ccache
              ccls
              cmake-format
              cmake-language-server
              conan
              distcc
              gdb
              ninja
              openmpi
              tbb
              valgrind
              ((import ./nix/weggli.nix) {
                inherit pkgs;
                src = weggli;
              })
            ]);
          CMAKE_CXX_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache";
          CMAKE_CXX_FLAGS_DEBUG = "-g -O0";
          CXXL = "${pkgs.stdenv.cc.cc.lib}";
          inherit QTDIR;
          Qt6_DIR = "${QTDIR}/lib/cmake/Qt6";
          Qt6CoreTools_DIR = "${QTDIR}/lib/cmake/Qt6CoreTools";
          Qt6GuiTools_DIR = "${QTDIR}/lib/cmake/Qt6GuiTools";
          Qt6WidgetsTools_DIR = "${QTDIR}/lib/cmake/Qt6WidgetsTools";
          Qt6Quick_DIR = "${QTDIR}/lib/cmake/Qt6Quick";
          Qt6Qml_DIR = "${QTDIR}/lib/cmake/Qt6Qml";
          Qt6QmlTools_DIR = "${QTDIR}/lib/cmake/Qt6QmlTools";
          PATH = "${QTDIR}/bin";
          THREADING_LINK_LIBS = "${pkgs.tbb}/lib/libtbb.so";
          AntlrRuntime_INCLUDE_DIRS =
            "${pkgs.antlr4.runtime.cpp.dev}/include/antlr4-runtime";
          AntlrRuntime_LINK_DIRS = "${pkgs.antlr4.runtime.cpp}/lib";
        };

        apps = {
          dissolve =
            flake-utils.lib.mkApp { drv = self.packages.${system}.dissolve; };
          dissolve-mpi = flake-utils.lib.mkApp {
            drv = self.packages.${system}.dissolve-mpi;
          };
          dissolve-gui = flake-utils.lib.mkApp {
            drv = self.packages.${system}.dissolve-gui;
          };
          uploader = {
            type = "app";
            program = toString (pkgs.writeScript "upload.sh" ''
              #!/bin/sh
              set -e
              if [ "$#" -ne 4 ] ; then
                echo "Usage: nix run .#uploader HARBOR_USER HARBOR_SECRET IMAGE TAG" >&2
                exit 1
              fi
              ${
                outdated.legacyPackages.${system}.singularity
              }/bin/singularity remote login --username $1 --password $2 docker://harbor.stfc.ac.uk
              ${
                outdated.legacyPackages.${system}.singularity
              }/bin/singularity push $3 oras://harbor.stfc.ac.uk/isis_disordered_materials/dissolve:$4
            '');
          };
        };

        defaultApp =
          flake-utils.lib.mkApp { drv = self.defaultPackage.${system}; };

        packages = {
          dissolve = dissolve { gui = false; };
          dissolve-threadless = dissolve {
            gui = false;
            threading = false;
          };
          dissolve-mpi = dissolve {
            mpi = true;
            gui = false;
          };
          dissolve-gui = dissolve { };

          singularity = mkSingularity { };
          singularity-mpi = mkSingularity { mpi = true; };
          singularity-gui = mkSingularity { gui = true; };
          singularity-threadless = mkSingularity {
            gui = false;
            threading = false;
          };

          docker = pkgs.dockerTools.buildImage {
            name = "dissolve";
            tag = "latest";
            config.Cmd = [ "${self.packages.${system}.dissolve}/bin/dissolve" ];
          };

          docker-gui = pkgs.dockerTools.buildImage {
            name = "dissolve-gui";
            tag = "latest";
            config.ENTRYPOINT = [
              "${nixGL.nixGLIntel}/bin/nixGLIntel"
              "${self.packages.${system}.dissolve-gui}/bin/dissolve-gui"
            ];
          };

          docker-mpi = pkgs.dockerTools.buildImage {
            name = "dissolve-mpi";
            tag = "latest";
            config.ENTRYPOINT =
              [ "${self.packages.${system}.dissolve-mpi}/bin/dissolve-mpi" ];
          };

        };
      });
}
