{ fetchurl, mirror,  }:

let
  minor = "6.4";
  patch = "${minor}.2";

in {
  qt3d = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qt3d-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qt3d-everywhere-src-${patch}.tar.xz";
    };
  };
  qt5compat = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qt5compat-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qt5compat-everywhere-src-${patch}.tar.xz";
    };
  };
  qtactiveqt = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtactiveqt-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtactiveqt-everywhere-src-${patch}.tar.xz";
    };
  };
  qtbase = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtbase-everywhere-src-${patch}.tar.xz";
      sha256 = "qIvGztuzSHikmmIrqnnKznjPutT5X9vTZW3bIccFUl0=";
      name = "qtbase-everywhere-src-${patch}.tar.xz";
    };
  };
  qtcharts = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtcharts-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtcharts-everywhere-src-${patch}.tar.xz";
    };
  };
  qtconnectivity = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtconnectivity-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtconnectivity-everywhere-src-${patch}.tar.xz";
    };
  };
  qtdatavis3d = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtdatavis3d-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtdatavis3d-everywhere-src-${patch}.tar.xz";
    };
  };
  qtdeclarative = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtdeclarative-everywhere-src-${patch}.tar.xz";
      sha256 = "0pSwKdwrLU9l2lFv3DuAiNMmQ+t/932xNai5zpBMqjc=";
      name = "qtdeclarative-everywhere-src-${patch}.tar.xz";
    };
  };
  qtdoc = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtdoc-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtdoc-everywhere-src-${patch}.tar.xz";
    };
  };
  qthttpserver = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qthttpserver-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qthttpserver-everywhere-src-${patch}.tar.xz";
    };
  };
  qtimageformats = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtimageformats-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtimageformats-everywhere-src-${patch}.tar.xz";
    };
  };
  qtlanguageserver = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtlanguageserver-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtlanguageserver-everywhere-src-${patch}.tar.xz";
    };
  };
  qtlottie = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtlottie-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtlottie-everywhere-src-${patch}.tar.xz";
    };
  };
  qtmultimedia = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtmultimedia-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtmultimedia-everywhere-src-${patch}.tar.xz";
    };
  };
  qtnetworkauth = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtnetworkauth-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtnetworkauth-everywhere-src-${patch}.tar.xz";
    };
  };
  qtpositioning = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtpositioning-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtpositioning-everywhere-src-${patch}.tar.xz";
    };
  };
  qtquick3d = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtquick3d-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtquick3d-everywhere-src-${patch}.tar.xz";
    };
  };
  qtquick3dphysics = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtquick3dphysics-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtquick3dphysics-everywhere-src-${patch}.tar.xz";
    };
  };
  qtquicktimeline = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtquicktimeline-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtquicktimeline-everywhere-src-${patch}.tar.xz";
    };
  };
  qtremoteobjects = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtremoteobjects-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtremoteobjects-everywhere-src-${patch}.tar.xz";
    };
  };
  qtscxml = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtscxml-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtscxml-everywhere-src-${patch}.tar.xz";
    };
  };
  qtsensors = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtsensors-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtsensors-everywhere-src-${patch}.tar.xz";
    };
  };
  qtserialbus = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtserialbus-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtserialbus-everywhere-src-${patch}.tar.xz";
    };
  };
  qtserialport = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtserialport-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtserialport-everywhere-src-${patch}.tar.xz";
    };
  };
  qtshadertools = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtshadertools-everywhere-src-${patch}.tar.xz";
      sha256 = "VJiVm503wlS9EmqxMgz4auuKMbPqflHbZmqPlpivvWw=";
      name = "qtshadertools-everywhere-src-${patch}.tar.xz";
    };
  };
  qtspeech = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtspeech-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtspeech-everywhere-src-${patch}.tar.xz";
    };
  };
  qtsvg = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtsvg-everywhere-src-${patch}.tar.xz";
      sha256 = "t0avPLF5NiHY7X6uONmtWhVUHcJ0IDEGnyrj/odZAxQ=";
      name = "qtsvg-everywhere-src-${patch}.tar.xz";
    };
  };
  qttools = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qttools-everywhere-src-${patch}.tar.xz";
      sha256 = "/OlGiOqSV4Kih5NHWEmR+FRjDarbpsUq7W2T4zzQsZw=";
      name = "qttools-everywhere-src-${patch}.tar.xz";
    };
  };
  qttranslations = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qttranslations-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qttranslations-everywhere-src-${patch}.tar.xz";
    };
  };
  qtvirtualkeyboard = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtvirtualkeyboard-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtvirtualkeyboard-everywhere-src-${patch}.tar.xz";
    };
  };
  qtwayland = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtwayland-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtwayland-everywhere-src-${patch}.tar.xz";
    };
  };
  qtwebchannel = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtwebchannel-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtwebchannel-everywhere-src-${patch}.tar.xz";
    };
  };
  qtwebengine = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtwebengine-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtwebengine-everywhere-src-${patch}.tar.xz";
    };
  };
  qtwebsockets = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtwebsockets-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtwebsockets-everywhere-src-${patch}.tar.xz";
    };
  };
  qtwebview = {
    version = "${patch}";
    src = fetchurl {
      url =
        "${mirror}/official_releases/qt/${minor}/${patch}/submodules/qtwebview-everywhere-src-${patch}.tar.xz";
      sha256 = "";
      name = "qtwebview-everywhere-src-${patch}.tar.xz";
    };
  };
}
