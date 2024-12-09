# Changelog

All notable changes to this project will be documented in this file.

## [1.6.0] - 2024-12-09

### 🚀 Features

- TemperatureSchedule module ([#1892](https://github.com/disorderedmaterials/dissolve/issues/1892))
- ([#1890](https://github.com/disorderedmaterials/dissolve/issues/1890)) Support extended trajectory export ([#1899](https://github.com/disorderedmaterials/dissolve/issues/1899))
- Add Buckingham potential and Guassian FD ([#1885](https://github.com/disorderedmaterials/dissolve/issues/1885))
- Permit global potential origins in fractional coordinates ([#1914](https://github.com/disorderedmaterials/dissolve/issues/1914))
- Create empty configuration ([#1921](https://github.com/disorderedmaterials/dissolve/issues/1921))
- Add Derivative namespace. ([#1907](https://github.com/disorderedmaterials/dissolve/issues/1907))
- Voxel density module NEW ([#1964](https://github.com/disorderedmaterials/dissolve/issues/1964))
- Coulombic as additional potential ([#1981](https://github.com/disorderedmaterials/dissolve/issues/1981))
- Phantom atoms ([#1953](https://github.com/disorderedmaterials/dissolve/issues/1953))
- TR module added ([#1956](https://github.com/disorderedmaterials/dissolve/issues/1956))
- Peak finding ([#1978](https://github.com/disorderedmaterials/dissolve/issues/1978))
- Add on sphere ([#2011](https://github.com/disorderedmaterials/dissolve/issues/2011))
- Use PCG for random number generation ([#2014](https://github.com/disorderedmaterials/dissolve/issues/2014))

### 🐛 Bug Fixes

- Fixed GUI output for SDF module ([#1880](https://github.com/disorderedmaterials/dissolve/issues/1880))
- Update runners to GitHub runner change ([#1888](https://github.com/disorderedmaterials/dissolve/issues/1888))
- Remove atom type and master term from forcefield tab ([#1889](https://github.com/disorderedmaterials/dissolve/issues/1889))
- Prevent extra newlines in error messages ([#1898](https://github.com/disorderedmaterials/dissolve/issues/1898))
- Update SpeciesSite keywords after data mutation ([#1905](https://github.com/disorderedmaterials/dissolve/issues/1905))
- Output newlines again ([#1904](https://github.com/disorderedmaterials/dissolve/issues/1904))
- Remove redundant assertions ([#1918](https://github.com/disorderedmaterials/dissolve/issues/1918))
- Copy global potentials from source configuration ([#1923](https://github.com/disorderedmaterials/dissolve/issues/1923))
- Copy atom type charge ([#1924](https://github.com/disorderedmaterials/dissolve/issues/1924))
- Inversion of custom regions ([#1926](https://github.com/disorderedmaterials/dissolve/issues/1926))
- Don't run analysis modules if configuration contents haven't changed ([#1935](https://github.com/disorderedmaterials/dissolve/issues/1935))
- Correct tabulated energies and forces when applying internal scaling factors ([#1908](https://github.com/disorderedmaterials/dissolve/issues/1908))
- AtomType Indexing ([#1938](https://github.com/disorderedmaterials/dissolve/issues/1938))
- Debug mode assertions error due to const atom not pointer ([#1943](https://github.com/disorderedmaterials/dissolve/issues/1943))
- Graph rename bug fix ([#1942](https://github.com/disorderedmaterials/dissolve/issues/1942))
- SizeFactor icon ([#1987](https://github.com/disorderedmaterials/dissolve/issues/1987))
- Restart Load Crash ([#1986](https://github.com/disorderedmaterials/dissolve/issues/1986))
- Spikes in difference functions ([#1988](https://github.com/disorderedmaterials/dissolve/issues/1988))
- Correct short range truncation label ([#2004](https://github.com/disorderedmaterials/dissolve/issues/2004))
- Species site display ([#2005](https://github.com/disorderedmaterials/dissolve/issues/2005))
- Size factor persisted after node was removed ([#2013](https://github.com/disorderedmaterials/dissolve/issues/2013))

### 🚜 Refactor

- Separate Forms ([#1837](https://github.com/disorderedmaterials/dissolve/issues/1837))
- Clarify forces and derivatives ([#1852](https://github.com/disorderedmaterials/dissolve/issues/1852))
- Rework Function1D, add derivatives ([#1876](https://github.com/disorderedmaterials/dissolve/issues/1876))
- Fully QML GUI as separate target executable ([#1894](https://github.com/disorderedmaterials/dissolve/issues/1894))
- Master term models ([#1893](https://github.com/disorderedmaterials/dissolve/issues/1893))
- Tidy site generation ([#1910](https://github.com/disorderedmaterials/dissolve/issues/1910))
- Refactor PairPotentials - Part 1 ([#1901](https://github.com/disorderedmaterials/dissolve/issues/1901))
- Refactor PairPotentials - Part 2 ([#1903](https://github.com/disorderedmaterials/dissolve/issues/1903))
- External potentials now use Function1D ([#1928](https://github.com/disorderedmaterials/dissolve/issues/1928))
- Refactor PairPotentials - Part 3 ([#1906](https://github.com/disorderedmaterials/dissolve/issues/1906))
- Function1D definitions ([#1930](https://github.com/disorderedmaterials/dissolve/issues/1930))
- Remove custom analysis nodes ([#1931](https://github.com/disorderedmaterials/dissolve/issues/1931))
- Tidy up benchmarks ([#1927](https://github.com/disorderedmaterials/dissolve/issues/1927)) ([#1944](https://github.com/disorderedmaterials/dissolve/issues/1944))
- Remove contexts from procedure ([#1933](https://github.com/disorderedmaterials/dissolve/issues/1933))
- Rename procedure node icons ([#1934](https://github.com/disorderedmaterials/dissolve/issues/1934))
- Procedures are now just Generators ([#1946](https://github.com/disorderedmaterials/dissolve/issues/1946))
- Update docs to reflect Generators rather than Procedures ([#1947](https://github.com/disorderedmaterials/dissolve/issues/1947))
- Add full pair iterator and for_each_full_pair ([#1973](https://github.com/disorderedmaterials/dissolve/issues/1973))
- Unify for_each_pair and for_each_full_pair ([#1977](https://github.com/disorderedmaterials/dissolve/issues/1977))
- Tidy interpolator and benchmark ([#1989](https://github.com/disorderedmaterials/dissolve/issues/1989))
- Add generator node base ([#2006](https://github.com/disorderedmaterials/dissolve/issues/2006))
- Single site class ([#2009](https://github.com/disorderedmaterials/dissolve/issues/2009))

### 📚 Documentation

- Fix DMG download links ([#1863](https://github.com/disorderedmaterials/dissolve/issues/1863))
- Update package-related links ([#1865](https://github.com/disorderedmaterials/dissolve/issues/1865))
- Corrections to tr documentation ([#2007](https://github.com/disorderedmaterials/dissolve/issues/2007))
- Remove duplicate IDAAAS documentation ([#2017](https://github.com/disorderedmaterials/dissolve/issues/2017))
- Updated for 1.6 release ([#2000](https://github.com/disorderedmaterials/dissolve/issues/2000))

### ⚡ Performance

- Parallelize siteRDF with ProductIterator ([#1882](https://github.com/disorderedmaterials/dissolve/issues/1882))

### ⚙️ Miscellaneous Tasks

- Get OSX Antlr4 Java from official website ([#1940](https://github.com/disorderedmaterials/dissolve/issues/1940))
- Arch specific gsl ([#2008](https://github.com/disorderedmaterials/dissolve/issues/2008))

### 🔨️ Build Tools

- Probe qml ([#1869](https://github.com/disorderedmaterials/dissolve/issues/1869))
- Fix Qt library install for Windows ([#1870](https://github.com/disorderedmaterials/dissolve/issues/1870))
- Bump dev version to 1.6.0. ([#1886](https://github.com/disorderedmaterials/dissolve/issues/1886))
- Update SCARF compile scripts ([#1911](https://github.com/disorderedmaterials/dissolve/issues/1911))
- Fix mac antlr installation ([#1939](https://github.com/disorderedmaterials/dissolve/issues/1939))
- Fix benchmark data upload and nix integration ([#1951](https://github.com/disorderedmaterials/dissolve/issues/1951))
- Naively remove custom Qt ([#1958](https://github.com/disorderedmaterials/dissolve/issues/1958))
- Remove last vestiage of our custom Qt for IDAaaS ([#1960](https://github.com/disorderedmaterials/dissolve/issues/1960))
- Fix single-page example generation ([#1961](https://github.com/disorderedmaterials/dissolve/issues/1961))
- Correctly pass CMake build type to conan ([#1965](https://github.com/disorderedmaterials/dissolve/issues/1965))
- Add QuickPlot into QML GUI ([#1963](https://github.com/disorderedmaterials/dissolve/issues/1963))
- Enable ccache by default under nix ([#1976](https://github.com/disorderedmaterials/dissolve/issues/1976))
- CMake tweaks ([#2015](https://github.com/disorderedmaterials/dissolve/issues/2015))
- Update release publishing workflow ([#2018](https://github.com/disorderedmaterials/dissolve/issues/2018))
- Update website and main / legacy workflows ([#2020](https://github.com/disorderedmaterials/dissolve/issues/2020))
- Correctly pass secrets through to publishing workflow. ([#2023](https://github.com/disorderedmaterials/dissolve/issues/2023))
- Don't try to update release info on the website unless actually releasing ([#2025](https://github.com/disorderedmaterials/dissolve/issues/2025))

### Doc

- Updated IDAaaS developer documentation ([#1972](https://github.com/disorderedmaterials/dissolve/issues/1972))
- Highlight links to other doc versions ([#1983](https://github.com/disorderedmaterials/dissolve/issues/1983))

## [1.5.0] - 2024-04-17

### #619

- Replaced std::lists with std::vectors in the DataStore classes.
- Replaced std::list with std::vector in the gui helper methods.
- Attempt to fix compilation issues shown on the CI/CD
- Updated the vectors to hold reference wrappers to pairs instead of pairs.
- Replaced the `reference_wrappers` with `shared_ptrs`.
- Fixed formatting issues.
- Fixed formatting issues again.
- Fixed the segfaults (hopefully)
- Code tidy-ups according to Tristan's comments on the PR
- Fixed formatting problems

### #964

- Changed getArrayIndex back to arrayIndex
- Cleaned up the code a little bit.
- Fixed clang-formatting issues.

### #999

- Cleaned up all of the input files that were created a natural isotopologue ([#1000](https://github.com/disorderedmaterials/dissolve/issues/1000))

### 🚀 Features

- Wrap QSortFilterProxy so it is accessible from QML ([#1813](https://github.com/disorderedmaterials/dissolve/issues/1813))
- Override Pair Potentials ([#1805](https://github.com/disorderedmaterials/dissolve/issues/1805)) ([#1826](https://github.com/disorderedmaterials/dissolve/issues/1826))
- OSites Bond Lengths ([#1817](https://github.com/disorderedmaterials/dissolve/issues/1817))
- EPSR Manager Module ([#1756](https://github.com/disorderedmaterials/dissolve/issues/1756)) ([#1755](https://github.com/disorderedmaterials/dissolve/issues/1755))
- Allow setting of temperature through button & update docs ([#1855](https://github.com/disorderedmaterials/dissolve/issues/1855))
- Running CNs added to SiteRDF ([#1819](https://github.com/disorderedmaterials/dissolve/issues/1819))

### 🐛 Bug Fixes

- Update displayed options on click or when node selection is changed by keys.
- Include final line of multi-line error messages ([#1822](https://github.com/disorderedmaterials/dissolve/issues/1822)) ([#1823](https://github.com/disorderedmaterials/dissolve/issues/1823))
- Removed broken assertion ([#1838](https://github.com/disorderedmaterials/dissolve/issues/1838))
- Allow non-pr runs to pass ci ([#1839](https://github.com/disorderedmaterials/dissolve/issues/1839))
- Typo in NeutronSQ NormaliseTo Keyword ([#1844](https://github.com/disorderedmaterials/dissolve/issues/1844))
- 1845 - Specify which sites require a defined axis ([#1850](https://github.com/disorderedmaterials/dissolve/issues/1850))
- 1844 - QML ScaleLayout scale charges dialog to accept floats ([#1848](https://github.com/disorderedmaterials/dissolve/issues/1848))
- Remove CMakeLists from packaged examples ([#1851](https://github.com/disorderedmaterials/dissolve/issues/1851))
- Creating configuration for CuBTC example ([#1854](https://github.com/disorderedmaterials/dissolve/issues/1854)) ([#1856](https://github.com/disorderedmaterials/dissolve/issues/1856))

### 🚜 Refactor

- No need for unused params.
- Formatting.
- Remove automatically added includes.
- Simulation data manager as QML TableView ([#1809](https://github.com/disorderedmaterials/dissolve/issues/1809))
- 1816 Rename Data Normaliser ([#1820](https://github.com/disorderedmaterials/dissolve/issues/1820))

### 📚 Documentation

- Fix web release version. ([#1833](https://github.com/disorderedmaterials/dissolve/issues/1833))
- Update website main page ([#1861](https://github.com/disorderedmaterials/dissolve/issues/1861))

### 🧪 Testing

- Show Testing Throws ([#1827](https://github.com/disorderedmaterials/dissolve/issues/1827))

### ⚙️ Miscellaneous Tasks

- Remove -user flag from pip install ([#1830](https://github.com/disorderedmaterials/dissolve/issues/1830))
- Quote PR title in test ([#1857](https://github.com/disorderedmaterials/dissolve/issues/1857))
- Rename icons, add EPSRManager icon ([#1858](https://github.com/disorderedmaterials/dissolve/issues/1858))

### 🔨️ Build Tools

- Add git-cliff support for changelog generation ([#1825](https://github.com/disorderedmaterials/dissolve/issues/1825))

### Docs

- Provide installation information for Dissolve on MacOS ([#1832](https://github.com/disorderedmaterials/dissolve/issues/1832))
- Add search bar ([#1842](https://github.com/disorderedmaterials/dissolve/issues/1842))

### Fix

- Correctly Set Working Directory in GUI ([#1829](https://github.com/disorderedmaterials/dissolve/issues/1829))

### Doc

- Update docs ([#1841](https://github.com/disorderedmaterials/dissolve/issues/1841))

### Script

- Use static antlr4 linkage.

## [0.6.1] - 2020-03-10

### Std

- :advance iterators in energykernel
- :advance iterators in forcekernel

## [0.4.2-patch1] - 2019-06-05

### Debug

- Add df -h.

## [0.35.1] - 2018-09-10

### ⚙️ Miscellaneous Tasks

- :findIsotopologue() now uses DissolveSys::sameString() rather than strcmp().

### AnalysisNode

- :finalise() now takes ProcessPool& as its first argument, bringing it in-line with execute().

### Array

- :first() and last() members now return reference and are not const. Added const Array<T>::firstValue() and lastValue() functions.

### DissolveSys

- :currentDateAndTime() now uses localtime() instead of gmtime().

### ModuleChart

- :layOutWidgets() would cause Array bound errors if zero blocks were displayed.
- :layOutWidgets() caused array errors if there were zero columns.

### PartialSet

- :formTotal() calculated spline interpolation.

## [0.35.0] - 2018-08-08

### ⚙️ Miscellaneous Tasks

- :load(const char*,int,int) did not read in specified columns.
- :parseKeyword() is now no longer a pure virtual - instead, a protected virtual parseComplexKeyword() function now exists for reimplementation in Modules. Definition of keywords for Modules now utilises new, more flexible approach, allowing definition of POD, class, and complex keywords in the same list, as well as argument definitions. Removed PartialsModule::BraggBroadening enum and associated functions. PartialsModule now stores Function definitions for general and Q-dependent broadening functions. XYData now contains single function suitable for Fourier transforming data, XYData::sineFT(), accepting two const Functions for the application of line broadening.
- :isotopologues() and Isotopologue::isotopes() now return references to List/RefList respectively.
- :autoAddGrains() now uses an Array of SpeciesBond RefLists rather than an auto-array.
- :speciesPopulation() is now const.

### Cell

- :removeAtom() did not fully remove the Atom, leading to overflows in Cells.

### CellArray

- :useMim() didn't always get things right. CellArray::withinRange() rewritten.
- :scale() did not scale all necessary quantities, and wrongly scaled fractional cell. Configuration::prepare() did not fold original centre-of-mass reference point.

### CharStringKeywordWidget

- :updateValue() now updates index in the widget.

### Collection

- :displayStyle() did not compare strings correctly.
- :uChromaBase_ is now no longer static, the pointer being set manually on item creation.

### Configuration

- :addMolecule() did not return pointer to the new molecule.
- :finaliseAfterLoad() now performs setup of Box and Cells.

### DUQ

- :iteration() would crash if the writeFrequency_ was zero.
- :configurations() now returns const reference to List.
- :pairPotentials() now returns List& rather than pointer to first item.

### Data

- :setName() now sets object name of stored XYData.

### DoubleExp

- :set(double) now handles absolute zeroes properly.

### Element

- :isotopes() now returns const List.

### EnergyKernel

- :cell() did not account for intramolecular scaling correctly when considering interaction with neighbouring cells.
- :energy(Grain*) always returned zero.
- :bond() now has CHECK for long bond distances.

### Function

- :functionType() did not correctly notify of an unrecognised Function type.

### GenericList

- :equality() was incomplete and entered into an infinite loop.
- :rename() did not rename correctly if no prefix was given.

### IsotopologueMix

- :isotopologues() now returns RefList& of isotopologues, rather than first item.

### KeywordData

- :isValid() now no longer pure virtual, returning false in default implementation.

### KeywordWidgetBase

- :updateValue() is now public.

### Messenger

- :error() now returns false so it can be used in a single-line return from function with message.
- :lineParser_ is now a full class mebmer rather than a pointer.

### Module

- :updateDependentTargets() now handles auto-addition of dependent Modules as well as their setup, and removed autoAddDependents argument from ModuleList::addModule(). Configuration::modules() now returns the ModuleList reference. ModuleList::addModule() now accepts a plain Module pointer as the 'addBeforeThis' argument.

### ModuleList

- :addModule() used wrong module pointer when searching for existing Module in list in order to add before it.

### Molecule

- :centerOfGeometry() and applyTransform() did not operate correctly.
- :updateAttachedAtomLists() now adds lists for Torsion interactions.

### MonitorWindow

- :removeWindow() is now a slot, connected to newly-created subwindow's windowClosed() signal.

### PairPotential

- :setXYDataNames() now also sets the resource names of the objects.

### PartialSet

- :generateTotal() is now const.

### PartialsModule

- :process() did not exit properly if calculateUnweightedSQ() failed.

### PlainValueList

- :valueAs* functions did not report errors correctly.
- :add() now returns PlainValue pointer.

### ProcessPool

- :broadcast(Array2D<>) used wrong array size for half-matrices main data broadcast.

### ScatteringMatrix

- :addReferenceData() now accepts optional factor to apply to scattering weights.
- :initialise() now sets group name in data objects.

### Surface

- :constructLineZY() and constructLineXY() now use GL_LINE_STRIP.

### TextPrimitive

- :set() now takes Matrix4 object as argument, rather than reference.

### UChromaBase

- :load() now sets XYData source correctly.
- :imageExportFormat_ was not initialised, leading to crashes on save.
- :startNewSession() now does not add an empty collection to the pane.

### Viewer

- :renderFullScene() now checks validity before rendering.
- :mouseMove() would potentially crash as no check for a valid ViewPane was performed.

### XYData

- :interpolationC_ was initialised with wrong size, leading to memory leak / occasional crash.
- :transformBroadenedRDF now accepts minimum Q value as argument.
- :broadenedSineFT() wrongly used 'value()' of the supplied broadening Functions, rather than the 'ft()', necessary for the convolution.
- :read() now uses overloaded LineParser::readNextLine().
- :z_ was not initialised on construction.
- :convolveNormalised() now checks for a zero integral.

<!-- generated by git-cliff -->
