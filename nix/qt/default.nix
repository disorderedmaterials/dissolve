{ newScope
, lib
, stdenv
, fetchurl
, fetchgit
, fetchpatch
, fetchFromGitHub
, makeSetupHook
, makeWrapper
, bison
, cups
, harfbuzz
, libGL
, perl
, cmake
, ninja
, writeText
, gtk3
, dconf
, libglvnd
, darwin
, buildPackages

  # options
, developerBuild ? false
, debug ? false
}:

let
  srcs = import ./srcs.nix {
    inherit fetchurl;
    mirror = "mirror://qt";
  };

  qtModule =
    import ./qtModule.nix
      { inherit stdenv lib perl cmake ninja writeText; }
      { inherit self srcs; };

  addPackages = self: with self;
    let
      callPackage = self.newScope ({ inherit qtModule stdenv srcs; });
    in
    {

      inherit callPackage qtModule srcs;

      qtbase = callPackage ./modules/qtbase.nix {
        withGtk3 = true;
        inherit (srcs.qtbase) src version;
        inherit bison cups harfbuzz libGL dconf gtk3 developerBuild cmake;
        inherit (darwin.apple_sdk_11_0.frameworks) AGL AVFoundation AppKit GSS MetalKit;
        patches = [
          ./patches/qtbase-qmake-pkg-config.patch
          ./patches/qtbase-tzdir.patch
          # Remove symlink check causing build to bail out and fail.
          # https://gitlab.kitware.com/cmake/cmake/-/issues/23251
          (fetchpatch {
            url = "https://github.com/Homebrew/formula-patches/raw/c363f0edf9e90598d54bc3f4f1bacf95abbda282/qt/qt_internal_check_if_path_has_symlinks.patch";
            sha256 = "sha256-Gv2L8ymZSbJxcmUijKlT2NnkIB3bVH9D7YSsDX2noTU=";
          })
        ];
      };
      env = callPackage ./qt-env.nix {};

      qt3d = callPackage ./modules/qt3d.nix { };
      qt5compat = callPackage ./modules/qt5compat.nix { };
      qtdatavis3d = callPackage ./modules/qtdatavis3d.nix { };
      qtdeclarative = callPackage ./modules/qtdeclarative.nix { };
      qtshadertools = callPackage ./modules/qtshadertools.nix { };
      qtquick3d = callPackage ./modules/qtquick3d.nix { };
      qtsvg = callPackage ./modules/qtsvg.nix { };
      qttools = callPackage ./modules/qttools.nix { };
      qtwayland = callPackage ./modules/qtwayland.nix { };

      wrapQtAppsHook = makeSetupHook {
          deps = [ buildPackages.makeWrapper ];
        } ./hooks/wrap-qt-apps-hook.sh;

      qmake = makeSetupHook {
        deps = [ self.qtbase.dev ];
        substitutions = {
          inherit debug;
          fix_qmake_libtool = ./hooks/fix-qmake-libtool.sh;
        };
      } ./hooks/qmake-hook.sh;
    };

  # TODO(@Artturin): convert to makeScopeWithSplicing
  # simple example of how to do that in 5568a4d25ca406809530420996d57e0876ca1a01
  self = lib.makeScope newScope addPackages;
in
self
