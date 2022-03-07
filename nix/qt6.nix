{ pkgs }:

let
  version = "622";
  dot_version = "6.2.2";
  qtbase = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_${version}/qt.qt6.${version}.gcc_64/${dot_version}-0-202111281958qtbase-Linux-RHEL_8_2-GCC-Linux-RHEL_8_2-X86_64.7z";
    sha256 = "1ix827lr2k7dn96xjn12lfnkhjly1hjllkqbm8q9wn4skjrbi6i0";
  };
  qtchart = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_${version}/qt.qt6.${version}.addons.qtcharts.gcc_64/${dot_version}-0-202106031044qtcharts-Linux-CentOS_8_3-GCC-Linux-CentOS_8_3-X86_64.7z";
    sha256 = "17kms6dfgxm2d54rdwz8r56s35rd8823nsjrmj7ak39wzlkzm173";
  };
  qtdeclarative = pkgs.fetchurl {
    url = "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_${version}/qt.qt6.${version}.gcc_64/${dot_version}-0-202111281958qtdeclarative-Linux-RHEL_8_2-GCC-Linux-RHEL_8_2-X86_64.7z";
    sha256 = "11a881g37f507birn72pl32rgixcb6pjr3aa58rhqdm14fr4kvaf";
  };
  qtsvg = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_${version}/qt.qt6.${version}.gcc_64/${dot_version}-0-202111281958qtsvg-Linux-RHEL_8_2-GCC-Linux-RHEL_8_2-X86_64.7z";
    sha256 = "06bfr0cmxz3n5m4k9hv2r7k1rhw0b70x7w4f7vj3jrskpay49j67";
  };
  qttools = pkgs.fetchurl {
    url = "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_${version}/qt.qt6.${version}.gcc_64/${dot_version}-0-202111281958qttools-Linux-RHEL_8_2-GCC-Linux-RHEL_8_2-X86_64.7z";
    sha256 = "1s6b17npy0d7bka5ckjkclwm5gy1kh596anb046rv0n80x7fvjnk";
  };
  qtwayland = pkgs.fetchurl {
    url = "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_622/qt.qt6.622.gcc_64/6.2.2-0-202111281958qtwayland-Linux-RHEL_8_2-GCC-Linux-RHEL_8_2-X86_64.7z";
    sha256 = "11ivdg25gg3z6iswi0lp4pbjbldjc9fs17gshayrvsiwnb58rs68";
  };

in pkgs.stdenv.mkDerivation {
  name = "qt6";
  unpackPhase = ''
    ${pkgs.p7zip}/bin/7z x ${qtbase} -o$out
    ${pkgs.p7zip}/bin/7z x ${qtdeclarative} -o$out
    ${pkgs.p7zip}/bin/7z x ${qtsvg} -o$out
    ${pkgs.p7zip}/bin/7z x ${qttools} -o$out
    ${pkgs.p7zip}/bin/7z x ${qtwayland} -o$out
  '';
  installPhase = ''
    mkdir $out/lib
    ln -s ${pkgs.libmysqlclient}/lib/mysql/libmysqlclient.so $out/lib/libmysqlclient.so.21
    patchelf --set-rpath $out/lib $out/${dot_version}/gcc_64/lib/libQt6Core.so.${dot_version}
    echo No Install
  '';
  nativeBuildInputs = [ pkgs.autoPatchelfHook ];
  autoPatchelfIgnoreMissingDeps = false;
  buildInputs = [
    pkgs.dbus
    pkgs.libxkbcommon
    pkgs.stdenv.cc.cc.lib

    pkgs.gdk-pixbuf
    pkgs.libdrm
    (import ./icu.nix {
      inherit pkgs;
      version = "56.1";
      sha256 = "OmTpEFxzTc9jHAs+1gQEUxvObA9aZL/hpkAqTMIxSBY=";
    })
    pkgs.libgssglue
    pkgs.mysql
    pkgs.libmysqlclient.dev
    pkgs.gtkd

    pkgs.cups
    pkgs.pango
    pkgs.postgresql
    pkgs.libGL
    pkgs.libglvnd
    pkgs.libglvnd.dev
    pkgs.xorg.xcbutil
    pkgs.xorg.xcbutilwm
    pkgs.xorg.xcbutilkeysyms
    pkgs.xorg.xcbutilimage
    pkgs.xorg.xcbutilrenderutil
    pkgs.unixODBC
  ];
}
