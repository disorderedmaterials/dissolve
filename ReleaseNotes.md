Version 1.1.3 is a patch release addressing some usability issues.

## Minor Changes in v1.1.3

- Feature: Export of calculated molecule in `AvgMol` module now possible.
- Fix: Mitigate overflows in Select nodes.

## Major Changes in v1.1.2

- Fix: EPSR module now correctly handles region below Qmin when fitting empirical potentials. This has been shown to resolve the generation of voids / component separation evident in some systems.

## Minor Changes in v1.1.2

- Fix: Singularity containers did not handle CLI arguments.
- Feature: Add ability to clear module data from an entire layer (in tab context menu).

## Minor Changes in v1.1.1

- Fix: Generation of coordinate sets via MD now uses automatic (variable) timestep, greatly reducing changes of explosions and bad configs.
- Fix: Importing a LigParGen molecule would crash the GUI if the "Simplify AtomTypes" control was activated.
- Fix: Organisation of file-based options in import & export modules.
- Fix: Regenerating species connectivity from the menu now has an effect.
- Fix: GUI now updates correctly when errors are encountered after performing File->Open...
- Fix: Functional forms for intramolecular interactions could not be edited in the GUI.
- Fix: Values were sometimes lost when setting functional form to one with a variable number of parameters.

## Major Changes in v1.1

### New Modules

Two new modules have been added, focusing on the calculation of intramolecular distance (`IntraDistance`) and angle (`IntraAngle`) histograms.

## Minor Changes in v1.1

- Fix: Output frequencies for energy and step information in the `MD` module were off by default.
- Fix: Output of restricted species information in `MD`, `MolShake`, and `IntraShake` modules was broken.
