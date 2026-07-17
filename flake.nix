{
  inputs = {
    self.submodules = true;
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.05";
    outdated.url = "github:NixOS/nixpkgs/nixos-24.05";
    bundlers.url = "github:nix-community/nix-bundle";
    bundlers.inputs.nixpkgs.follows = "outdated";
    nixGL-src.url = "github:guibou/nixGL";
    nixGL-src.flake = false;
  };

  outputs =
    {
      self,
      nixpkgs,
      outdated,
      home-manager,
      flake-utils,
      bundlers,
      nixGL-src,
    }:
    let

      toml = pkgs: ((import ./nix/toml11.nix) { inherit pkgs; });
      onedpl =
        pkgs:
        ((import ./nix/onedpl.nix) {
          inherit (pkgs)
            lib
            stdenv
            fetchFromGitHub
            fetchpatch
            cmake
            ;
          tbb = pkgs.tbb_2021_11;
        });
      exe-name = gui: if gui then "dissolve-gui" else "dissolve";
      cmake-bool = x: if x then "ON" else "OFF";
      version = "1.9.0";
      base_libs =
        pkgs: with pkgs; [
          antlr4
          antlr4.runtime.cpp
          antlr4.runtime.cpp.dev
          gbenchmark
          cmake
          cli11
          gsl
          inetutils # for rsh
          ninja
          jre
          pkg-config
          pugixml
          (toml pkgs)
        ];
      gui_libs = system: pkgs: qt: [
        pkgs.glib
        pkgs.libGL.dev
        pkgs.libglvnd
        pkgs.libglvnd.dev
        qt.qt3d
        qt.qtbase
        qt.qtbase.dev
        qt.qtquick3d
        qt.qtsvg
        qt.qtshadertools
        qt.qttools
        qt.qtdeclarative
        qt.qtdeclarative.dev
        qt.wrapQtAppsHook
      ];
      check_libs = pkgs: with pkgs; [ gtest ];

    in
    flake-utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" ] (
      system:

      let
        pkgs = import nixpkgs { inherit system; };
        old = import outdated { inherit system; };
        nixGL = import nixGL-src { inherit pkgs; };
        qt = old.qt6;
        dissolve =
          {
            gui ? false,
            threading ? true,
            checks ? true,
            benchmarks ? false,
          }:
          pkgs.stdenv.mkDerivation ({
            inherit version;
            pname = exe-name gui;
            src = pkgs.lib.fileset.toSource {
              root = ./.;
              fileset = (
                pkgs.lib.fileset.unions [
                  ./src
                  ./cmake
                  ./examples
                  ./tests
                  ./benchmark
                  ./CMakeLists.txt
                  ./QuickPlot
                ]
              );
            };
            buildInputs =
              base_libs pkgs
              ++ pkgs.lib.optionals gui (gui_libs system pkgs qt)
              ++ pkgs.lib.optionals checks (check_libs pkgs)
              ++ pkgs.lib.optionals threading [
                pkgs.tbb_2021_11
                (onedpl pkgs)
                (onedpl pkgs).dev
              ];
            nativeBuildInputs = pkgs.lib.optionals gui [ pkgs.wrapGAppsHook ];

            CTEST_OUTPUT_ON_FAILURE = "ON";

            cmakeFlags = [
              "-DCONAN=OFF"
              "-G Ninja"
              ("-DMULTI_THREADING=" + (cmake-bool threading))
              ("-DGUI=" + (cmake-bool gui))
              "-DBUILD_TESTS:bool=${cmake-bool checks}"
              "-DBUILD_BENCHMARKS:bool=${cmake-bool benchmarks}"
              "-DCMAKE_BUILD_TYPE=Release"
            ];
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
          })
          // (if checks then { QT_QPA_PLATFORM = "offscreen"; } else { });
        mkSingularity =
          {
            gui ? false,
            threading ? true,
          }:
          outdated.legacyPackages.${system}.singularity-tools.buildImage {
            name = "${exe-name gui}-${version}";
            diskSize = 1024 * 50;
            contents = [ (dissolve { inherit gui threading; }) ];
            runScript =
              if gui then
                "${nixGL.nixGLIntel}/bin/nixGLIntel ${dissolve { inherit gui threading; }}/bin/${exe-name gui} $@"
              else
                "${dissolve { inherit gui threading; }}/bin/${exe-name gui} $@";
          };
      in
      {
        bundlers = {
          default = bundlers.bundlers.${system}.nix-bundle;
        };

        checks.dissolve = dissolve { checks = true; };
        checks.dissolve-gui = dissolve {
          gui = true;
          checks = true;
        };
        checks.dissolve-threadless = dissolve {
          threading = false;
          gui = false;
          checks = true;
        };

        defaultPackage = self.packages.${system}.dissolve;

        devShells.default = pkgs.mkShell {
          name = "dissolve-shell";
          buildInputs =
            base_libs pkgs
            ++ gui_libs system pkgs qt
            ++ check_libs pkgs
            ++ (with pkgs; [
              llvmPackages_20.clang-tools

                (onedpl pkgs)

              ccache
              ccls
              cmakeWithGui
              cmake-format
              cmake-language-server
              conan
              cppcheck
              direnv
              gdb
              gtk3
              nixGL.nixGLIntel
              qt.qttools
              tbb_2021_11
              valgrind
              weggli
            ]);
          shellHook = ''
            export XDG_DATA_DIRS=$GSETTINGS_SCHEMAS_PATH:$XDG_DATA_DIRS
            export LIBGL_DRIVERS_PATH=${pkgs.lib.makeSearchPathOutput "lib" "lib/dri" [ pkgs.mesa ]}
            export LIBVA_DRIVERS_PATH=${pkgs.lib.makeSearchPathOutput "out" "lib/dri" [ pkgs.mesa ]}
            export __EGL_VENDOR_LIBRARY_FILENAMES=${pkgs.mesa}/share/glvnd/egl_vendor.d/50_mesa.json
            export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [ pkgs.mesa ]}:${
              pkgs.lib.makeSearchPathOutput "lib" "lib/vdpau" [ pkgs.libvdpau ]
            }:${pkgs.lib.makeLibraryPath [ pkgs.libglvnd ]}"''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
            # export QT_PLUGIN_PATH="${qt.qt3d}/lib/qt-6/plugins:${qt.qtsvg}/lib/qt-6/plugins:$QT_PLUGIN_PATH"
            export QT_PLUGIN_PATH="${qt.qtquick3d}/lib/qt-6/plugins:${qt.qt3d}/lib/qt-6/plugins:${qt.qtsvg}/lib/qt-6/plugins:$QT_PLUGIN_PATH"
          '';

          CMAKE_CXX_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache";
          CMAKE_C_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache";
          CMAKE_CXX_FLAGS_DEBUG = "-g -O0";
          CXXL = "${pkgs.stdenv.cc.cc.lib}";
          Qt6Quick3D_DIR = "${qt.qtquick3d}/lib/";
          QML_IMPORT_PATH = "${qt.qtquick3d}/lib/qt-6/qml:${qt.qtdeclarative}/lib/qt-6/qml/";
          QML2_IMPORT_PATH = "$\${qt.qtquick3d}/lib/qt-6/qml:{qt.qtdeclarative}/lib/qt-6/qml/";
        };

        apps = {
          benchmarks = {
            type = "app";
            program = toString (
              pkgs.writeScript "benchmark.sh" ''
                #!/bin/sh
                set -e
                export TMP=$(mktemp -d)
                for bm in ${self.packages.${system}.benchmarks}/bin/benchmark_*
                do
                  export BENCHNAME=$(basename ${"$"}{bm})_result.json
                  >&2 echo Running ${"$"}{BENCHNAME}
                  ${"$"}{bm} --benchmark_format=json > $TMP/${"$"}{BENCHNAME}
                done
                ${pkgs.jq}/bin/jq -s '[.[] | to_entries] | flatten | reduce .[] as $dot ({}; .[$dot.key] += $dot.value)' $TMP/benchmark_*.json > $TMP/all_benchmark_results.json
                cat $TMP/all_benchmark_results.json
              ''
            );
          };
          dissolve-app = flake-utils.lib.mkApp { drv = self.packages.${system}.dissolve; };
          dissolve-gui-app = flake-utils.lib.mkApp {
            drv = self.packages.${system}.dissolve-gui;
          };
          uploader = {
            type = "app";
            program = toString (
              pkgs.writeScript "upload.sh" ''
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
              ''
            );
          };
        };

        defaultApp = flake-utils.lib.mkApp { drv = self.defaultPackage.${system}; };

        packages = {
          benchmarks = dissolve {
            benchmarks = true;
            checks = false;
          };
          dissolve = dissolve { };
          dissolve-threadless = dissolve {
            gui = false;
            threading = false;
          };
          dissolve-gui = dissolve { gui = true; };

          singularity = mkSingularity { };
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
        };

        homeConfigurations = {
          "dissolve" = home-manager.lib.homeManagerConfiguration {
            inherit pkgs;
            modules = with self.homeManagerModules; [ user-env ];
          };
        };

        homeManagerModule = {
          user-env = import ./nix/user-env.nix;
        };
      }
    );
}
