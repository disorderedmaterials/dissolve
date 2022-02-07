{ pkgs }:

let
  aqt = pkgs.python3.pkgs.buildPythonPackage rec {
    pname = "aqtinstall";
    version = "1.2.4";
    src = pkgs.python3.pkgs.fetchPypi {
      inherit pname version;
      sha256 = "fmaAYOSHrx5LVUoPlIw0p/0jMRVGSPE+teEVlNurz10=";
    };
    propagatedBuildInputs = [
      pkgs.python3.pkgs.setuptools-scm
      pkgs.python3.pkgs.texttable
      pkgs.python3.pkgs.patch
      pkgs.python3.pkgs.requests
      semantic_version
      pkgs.p7zip
    ];
    pipInstallFlags = [ "--no-deps" ];

    doCheck = false;
  };

  semantic_version = pkgs.python3.pkgs.buildPythonPackage rec {
    pname = "semantic_version";
    version = "2.8.5";
    src = pkgs.python3.pkgs.fetchPypi {
      inherit pname version;
      sha256 = "0sst4FWHYpNGebmhBOguynr0SMn0l00fPuzP9lHfilQ=";
    };
    # propagatedBuildInputs =
    #   [ pkgs.python3.pkgs.setuptools-scm pkgs.python3.pkgs.semantic_verion ];
    # pipInstallFlags = [ "--no-deps" ];

    doCheck = false;
  };

  qtbase = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_611/qt.qt6.611.gcc_64/6.1.1-0-202106031044qtbase-Linux-CentOS_8_3-GCC-Linux-CentOS_8_3-X86_64.7z";
    sha256 = "fcE1ShAuAbKPihOG4OILnijM0mgAsa5l8V8V1bOYykM=";
  };
  qtchart = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_611/qt.qt6.611.addons.qtcharts.gcc_64/6.1.1-0-202106031044qtcharts-Linux-CentOS_8_3-GCC-Linux-CentOS_8_3-X86_64.7z";
    sha256 = "16kms6dfgxm2d54rdwz8r56s35rd8823nsjrmj7ak39wzlkzm173";
  };
  qtdeclarative = pkgs.fetchurl {
    url = "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_611/qt.qt6.611.gcc_64/6.1.1-0-202106031044qtdeclarative-Linux-CentOS_8_3-GCC-Linux-CentOS_8_3-X86_64.7z";
    sha256 = "11r2nv6n9xr5lvvj1g3lhqqwxlnjn7xjz3jdgqqvlj3c7r73q8fk";
  };
  qtsvg = pkgs.fetchurl {
    url =
      "https://mirrors.ukfast.co.uk/sites/qt.io/online/qtsdkrepository/linux_x64/desktop/qt6_611/qt.qt6.611.gcc_64/6.1.1-0-202106031044qtsvg-Linux-CentOS_8_3-GCC-Linux-CentOS_8_3-X86_64.7z";
    sha256 = "zO9CAMNN7k5k51V4JcrCZFbAag3sn2gmd0YoYvh+qng=";
  };

in pkgs.stdenv.mkDerivation {
  name = "qt6";
  unpackPhase = ''
    ${pkgs.p7zip}/bin/7z x ${qtbase} -o$out
    ${pkgs.p7zip}/bin/7z x ${qtsvg} -o$out
  '';
  installPhase = ''
    mkdir $out/lib
    ln -s ${pkgs.libmysqlclient}/lib/mysql/libmysqlclient.so $out/lib/libmysqlclient.so.21
    patchelf --set-rpath $out/lib $out/6.1.1/gcc_64/lib/libQt6Core.so.6.1.1
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
