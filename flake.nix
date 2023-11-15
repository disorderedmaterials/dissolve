{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.11";
    outdated.url = "github:NixOS/nixpkgs/nixos-21.05";
    nixGL-src.url = "github:guibou/nixGL";
    nixGL-src.flake = false;
    weggli.url = "github:googleprojectzero/weggli";
    weggli.flake = false;
    qt-idaaas.url = "github:disorderedmaterials/qt-idaaas";
  };
  outputs = { self, nixpkgs, outdated, home-manager, flake-utils, bundlers, nixGL-src
    , qt-idaaas, weggli }:
    let

      toml = pkgs: ((import ./nix/toml11.nix) { inherit pkgs; });
      exe-name = mpi: gui:
        if mpi then
          "dissolve-mpi"
        else
          (if gui then "dissolve-gui" else "dissolve");
      cmake-bool = x: if x then "ON" else "OFF";
      version = "1.4.0";
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
          ninja
          jre
          pkgconfig
          pugixml
          (toml pkgs)
        ];
      gui_libs = system: pkgs:
        let q = qt-idaaas.packages.${system};
        in with pkgs; [
          glib
          freetype
          ftgl
          libGL.dev
          libglvnd
          libglvnd.dev
          q.qtbase
          q.qtbase.dev
          q.qtsvg
          q.qtshadertools
          q.qttools
          q.qtdeclarative
          q.qtdeclarative.dev
          q.wrapQtAppsHook
        ];
      check_libs = pkgs: with pkgs; [ gtest ];

    in flake-utils.lib.eachSystem [ "x86_64-linux" "aarch64-linux" ] (system:

      let
        pkgs = import nixpkgs { inherit system; };
        nixGL = import nixGL-src { inherit pkgs; };
        dissolve =
          { mpi ? false, gui ? false, threading ? true, checks ? true }:
          assert (!(gui && mpi));
          pkgs.stdenv.mkDerivation ({
            inherit version;
            pname = exe-name mpi gui;
            src = builtins.path {
              path = ./.;
              name = "dissolve-src";
              filter = path: type:
                type != "directory" || builtins.baseNameOf path
                != ".azure-pipelines" || builtins.baseNameOf path != "web";
            };
            buildInputs = base_libs pkgs ++ pkgs.lib.optional mpi pkgs.openmpi
              ++ pkgs.lib.optionals gui (gui_libs system pkgs)
              ++ pkgs.lib.optionals checks (check_libs pkgs)
              ++ pkgs.lib.optional threading pkgs.tbb;
            nativeBuildInputs = pkgs.lib.optionals gui [ pkgs.wrapGAppsHook ];

            TBB_DIR = "${pkgs.tbb}";
            CTEST_OUTPUT_ON_FAILURE = "ON";

            cmakeFlags = [
              "-DAntlrRuntime_INCLUDE_DIRS=${pkgs.antlr4.runtime.cpp.dev}/include/antlr4-runtime"
              "-DAntlrRuntime_LINK_DIRS=${pkgs.antlr4.runtime.cpp}/lib"
              "-DCONAN=OFF"
              "-G Ninja"
              ("-DMULTI_THREADING=" + (cmake-bool threading))
              ("-DPARALLEL=" + (cmake-bool mpi))
              ("-DGUI=" + (cmake-bool gui))
              "-DBUILD_TESTS:bool=${cmake-bool checks}"
              "-DCMAKE_BUILD_TYPE=Release"
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
          }) // (if checks then { QT_QPA_PLATFORM = "offscreen"; } else { });
        mkSingularity = { mpi ? false, gui ? false, threading ? true }:
          outdated.legacyPackages.${system}.singularity-tools.buildImage {
            name = "${exe-name mpi gui}-${version}";
            diskSize = 1024 * 50;
            contents = [ (dissolve { inherit mpi gui threading; }) ];
            runScript = if gui then
              "${nixGL.nixGLIntel}/bin/nixGLIntel ${
                dissolve { inherit mpi gui threading; }
              }/bin/${exe-name mpi gui} $@"
            else
              "${dissolve { inherit mpi gui threading; }}/bin/${
                exe-name mpi gui
              } $@";
          };
      in {
        checks.dissolve = dissolve { checks = true; };
        checks.dissolve-gui = dissolve {
          gui = true;
          checks = true;
        };
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

        defaultPackage = self.packages.${system}.dissolve;

        devShells.default = pkgs.mkShell {
          name = "dissolve-shell";
          buildInputs = base_libs pkgs ++ gui_libs system pkgs ++ check_libs pkgs
            ++ (with pkgs; [
              (pkgs.clang-tools.override {
                llvmPackages = pkgs.llvmPackages_13;
              })
              busybox
              ccache
              ccls
              cmakeWithGui
              cmake-format
              cmake-language-server
              conan
              distcc
              direnv
              gdb
              gtk3
              nixGL.nixGLIntel
              openmpi
              qt-idaaas.packages.${system}.qttools
              tbb
              valgrind
              ((import ./nix/weggli.nix) {
                inherit pkgs;
                src = weggli;
              })
            ]);
          shellHook = ''
            export XDG_DATA_DIRS=$GSETTINGS_SCHEMAS_PATH:$XDG_DATA_DIRS
            export LIBGL_DRIVERS_PATH=${pkgs.lib.makeSearchPathOutput "lib" "lib/dri" [pkgs.mesa.drivers]}
            export LIBVA_DRIVERS_PATH=${pkgs.lib.makeSearchPathOutput "out" "lib/dri" [pkgs.mesa.drivers]}
            export __EGL_VENDOR_LIBRARY_FILENAMES=${pkgs.mesa.drivers}/share/glvnd/egl_vendor.d/50_mesa.json
            export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [pkgs.mesa.drivers]}:${pkgs.lib.makeSearchPathOutput "lib" "lib/vdpau" [pkgs.libvdpau]}:${pkgs.lib.makeLibraryPath [pkgs.libglvnd]}"''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
            export QT_PLUGIN_PATH="${qt-idaaas.packages.${system}.qtsvg}/lib/qt-6/plugins:$QT_PLUGIN_PATH"
          '';


          AntlrRuntime_INCLUDE_DIRS =
            "${pkgs.antlr4.runtime.cpp.dev}/include/antlr4-runtime";
          AntlrRuntime_LINK_DIRS = "${pkgs.antlr4.runtime.cpp}/lib";
          CMAKE_CXX_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache;${pkgs.distcc}/bin/distcc";
          CMAKE_C_COMPILER_LAUNCHER = "${pkgs.ccache}/bin/ccache;${pkgs.distcc}/bin/distcc";
          CMAKE_CXX_FLAGS_DEBUG = "-g -O0";
          CXXL = "${pkgs.stdenv.cc.cc.lib}";
          QML_IMPORT_PATH = "${qt-idaaas.packages.${system}.qtdeclarative}/lib/qt-6/qml/";
          QML2_IMPORT_PATH = "${qt-idaaas.packages.${system}.qtdeclarative}/lib/qt-6/qml/";
          THREADING_LINK_LIBS = "${pkgs.tbb}/lib/libtbb.so";
        };

        apps = {
          dissolve-app =
            flake-utils.lib.mkApp { drv = self.packages.${system}.dissolve; };
          dissolve-mpi-app = flake-utils.lib.mkApp {
            drv = self.packages.${system}.dissolve-mpi;
          };
          dissolve-gui-app = flake-utils.lib.mkApp {
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
          dissolve = dissolve { };
          dissolve-threadless = dissolve {
            gui = false;
            threading = false;
          };
          dissolve-mpi = dissolve {
            mpi = true;
            gui = false;
          };
          dissolve-gui = dissolve { gui = true; };

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

        homeConfigurations = {
          "dissolve" = home-manager.lib.homeManagerConfiguration {
            inherit pkgs;
            modules = with self.homeManagerModules; [
              user-env
            ];
          };
        };

        homeManagerModule = {
          user-env = import ./nix/user-env.nix;
        };
      });
}
