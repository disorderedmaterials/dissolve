Version 1.4.2 is a patch release addressing some crashes when working the modules and layers.

## Major Changes in v1.4.2

Fixes crashes when adding a new module to any layer, and when adding an EPSR module when there are no configurations present.

## Major Changes in v1.4.1

Version 1.4.1 fixes some calculation issues with the `QSpecies` module where the proportion of Q(0) species was not correctly determined, and which could result in misleading results.

## Major Changes in v1.4

### Input File Compatibility with v1.3

There are no expected incompatibilities with input files from version 1.3, but users may notice warning messages because of the transition away from some configuration keywords towards dedicated procedure nodes.

### UI

Many improvements in layout, icons, and usability have been made in the main UI. Version 1.4 also begins the long journey towards reimplementing the majority of the UI in QML. At present only the Add Forcefield Terms dialog has been transitioned as a test case, so users may notice some subtle differences in the look & feel compared to the rest of the UI.

### TOML-format Input Files

Version 1.4 now has the capability to read and write TOML-formatted input files, although this is not directly visible to the user at present. While the existing "txt"-style files are still the standard, it is intended to move to TOML as the standard, and drop support for txt input files, in version 2.0.

### Crystallographic Information File Import

Import of Crystallographic Information File (CIF) files now has preliminary support for generating individual species descriptions for components detected in the packed cell. This should be considered as a fragile feature at present, and may well not work correctly in all cases.

### Configuration-related Procedure Nodes

As mentioned above, version 1.4 deprecates (but does not yet remove) several configuration-related keywords in favour of leveraging specialised procedure nodes in the configuration's generator. As such, three new nodes - "Temperature", "SizeFactor", and "InputCoordinates" - have been introduced.

### Region Potentials

A new type of global potential type - RegionPotential - has been added, and allows application of a pseudo-potential (based on the magnitude of a user-provided function at that voxel) to be applied and utilised in evolution algorithms and configuration construction.

### Modules

#### Energy
Output on the Energy graph has been fixed to properly show torsion and improper contributions, and to clearly show and define intramolecular (within molecules), cohesive (intermolecular, between molecules), and the total contribution from pair potential terms.

#### EPSR
The EPSR module has had several improvements including the ability to apply weightings to individual datasets, calculation of R-factors over specific Q-ranges (in addition to the "normal" full Q-range metric), and detection of atom types that should not be considered in the fitting process (e.g. because they are not present in the configuration targeted by the structural modules which EPSR itself is referencing).

#### Q-Species (New)
A new module to calculate Q-species information for glassy network systems is now available.

## Minor Changes in v1.4

- Optimisation: Region generation is now multithreaded.
- Code: Dependent versions of ANTLR and TBB have been updated.

