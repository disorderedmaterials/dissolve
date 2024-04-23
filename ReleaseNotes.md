# Changelog

## [1.5.1] - 23rd April 2024

### 🐛 Bug Fixes

- Fix display of surfaces in SDF module

### 🔨️ Build Tools

- Include correct QML-related libs in OSX package
- Include correct QML-related libs in Windows package

## [1.5.0] - 17th April 2024

### 🚀 Features

- ([#1791](https://github.com/disorderedmaterials/dissolve/issues/1791)) Mugglify CIF Importer ([#1799](https://github.com/disorderedmaterials/dissolve/issues/1799))
- ([#1620](https://github.com/disorderedmaterials/dissolve/issues/1620)) Compare Module GUI Implementation ([#1662](https://github.com/disorderedmaterials/dissolve/issues/1662))
- Wrap QSortFilterProxy so it is accessible from QML ([#1813](https://github.com/disorderedmaterials/dissolve/issues/1813))
- Modifier Oxygen Sites Module setup ([#1793](https://github.com/disorderedmaterials/dissolve/issues/1793))
- Override Pair Potentials ([#1805](https://github.com/disorderedmaterials/dissolve/issues/1805)) ([#1826](https://github.com/disorderedmaterials/dissolve/issues/1826))
- Running CNs added to SiteRDF (#1819)

### 🐛 Bug Fixes

- Fix onetbb version ([#1759](https://github.com/disorderedmaterials/dissolve/issues/1759))
- Fix Benchmarks 2024 ([#1764](https://github.com/disorderedmaterials/dissolve/issues/1764))
- Code Tidy January 2024 ([#1784](https://github.com/disorderedmaterials/dissolve/issues/1784))
- Fix Benchmarks After Conan Tidy ([#1796](https://github.com/disorderedmaterials/dissolve/issues/1796))
- ([#1775](https://github.com/disorderedmaterials/dissolve/issues/1775)) Serial Compilation ([#1802](https://github.com/disorderedmaterials/dissolve/issues/1802))
- ([#1689](https://github.com/disorderedmaterials/dissolve/issues/1689)) Root Elements in Generated NETA ([#1803](https://github.com/disorderedmaterials/dissolve/issues/1803))
- Include final line of multi-line error messages ([#1822](https://github.com/disorderedmaterials/dissolve/issues/1822)) ([#1823](https://github.com/disorderedmaterials/dissolve/issues/1823))
- Correctly Set Working Directory in GUI ([#1829](https://github.com/disorderedmaterials/dissolve/issues/1829))
- Removed broken assertion ([#1838](https://github.com/disorderedmaterials/dissolve/issues/1838))
- Allow non-pr runs to pass ci ([#1839](https://github.com/disorderedmaterials/dissolve/issues/1839))
- Typo in NeutronSQ NormaliseTo Keyword ([#1844](https://github.com/disorderedmaterials/dissolve/issues/1844))
- 1845 - Specify which sites require a defined axis ([#1850](https://github.com/disorderedmaterials/dissolve/issues/1850))
- 1844 - QML ScaleLayout scale charges dialog to accept floats ([#1848](https://github.com/disorderedmaterials/dissolve/issues/1848))
- Remove CMakeLists from packaged examples ([#1851](https://github.com/disorderedmaterials/dissolve/issues/1851))

### 🚜 Refactor

- ([#1637](https://github.com/disorderedmaterials/dissolve/issues/1637)) remove modules singleton ([#1749](https://github.com/disorderedmaterials/dissolve/issues/1749))
- ([#1637](https://github.com/disorderedmaterials/dissolve/issues/1637)) dynamic keywords ([#1766](https://github.com/disorderedmaterials/dissolve/issues/1766))
- Reinventing SiteRDF ([#1778](https://github.com/disorderedmaterials/dissolve/issues/1778))
- ([#1690](https://github.com/disorderedmaterials/dissolve/issues/1690)) Molecularize CIF ([#1792](https://github.com/disorderedmaterials/dissolve/issues/1792))
- Analysis Refactoring - Part 1 ([#1789](https://github.com/disorderedmaterials/dissolve/issues/1789))
- Simulation data manager as QML TableView ([#1809](https://github.com/disorderedmaterials/dissolve/issues/1809))
- Analysis Refactoring - Part 2 ([#1795](https://github.com/disorderedmaterials/dissolve/issues/1795))
- 1816 Rename Data Normaliser ([#1820](https://github.com/disorderedmaterials/dissolve/issues/1820))

### 📚 Documentation

- Update website release info (v1.4.1) ([#1765](https://github.com/disorderedmaterials/dissolve/issues/1765))
- Update website release info (v1.4.2) ([#1780](https://github.com/disorderedmaterials/dissolve/issues/1780))
- Provide installation information for Dissolve on MacOS ([#1832](https://github.com/disorderedmaterials/dissolve/issues/1832))
- Fix web release version. ([#1833](https://github.com/disorderedmaterials/dissolve/issues/1833))
- Add search bar ([#1842](https://github.com/disorderedmaterials/dissolve/issues/1842))

### ⚡ Performance

- 1806 faster combinations ([#1807](https://github.com/disorderedmaterials/dissolve/issues/1807))

### 🎨 Styling

- Display qml scale charges dialog ([#1763](https://github.com/disorderedmaterials/dissolve/issues/1763))
- Modify charge dialogs qml ([#1787](https://github.com/disorderedmaterials/dissolve/issues/1787))

### 🧪 Testing

- ([#1576](https://github.com/disorderedmaterials/dissolve/issues/1576)) More CIF Unit Tests ([#1804](https://github.com/disorderedmaterials/dissolve/issues/1804))
- Show Testing Throws ([#1827](https://github.com/disorderedmaterials/dissolve/issues/1827))

### ⚙️ Miscellaneous Tasks

- Remove need for QC before build workflow ([#1771](https://github.com/disorderedmaterials/dissolve/issues/1771))
- ([#1779](https://github.com/disorderedmaterials/dissolve/issues/1779)) Upgrade GH Upload/Download ([#1781](https://github.com/disorderedmaterials/dissolve/issues/1781))
- Remove -user flag from pip install ([#1830](https://github.com/disorderedmaterials/dissolve/issues/1830))

### 🔨️ Build Tools

- Disable arm64 build ([#1757](https://github.com/disorderedmaterials/dissolve/issues/1757))
- Link qt-idaaas nixpkgs version to our primary nixpkgs version ([#1760](https://github.com/disorderedmaterials/dissolve/issues/1760))
- Build for Apple Silicon ([#1743](https://github.com/disorderedmaterials/dissolve/issues/1743))
- ([#1733](https://github.com/disorderedmaterials/dissolve/issues/1733)) Auto Qt ([#1772](https://github.com/disorderedmaterials/dissolve/issues/1772))
- Conan Deps ([#1785](https://github.com/disorderedmaterials/dissolve/issues/1785))
- Tidy CMake Presets ([#1808](https://github.com/disorderedmaterials/dissolve/issues/1808))
- Add git-cliff support for changelog generation ([#1825](https://github.com/disorderedmaterials/dissolve/issues/1825))
