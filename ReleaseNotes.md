Version 0.9.2 is a patch release providing significant number of bugfixes, improvements, and new functionality.

The full release notes for 0.9.0 detailing major changes in this version can be found at https://github.com/disorderedmaterials/dissolve/releases/tag/0.9.0.

### Summary of Changes
- New Feature: "Add Configuration Wizard" to streamline setting up new configurations.
- New Feature: `AddPair` node to add pairs of species at the same relative coordinates.
- New Feature: Layers can now be prevented from running if energies are not stable, or configuration size factors are currently being applied.
- New Feature: The `MD` module now has a new timestep mode "Auto" which chooses a variable timestep based on interatomic forces rather than total force, and which reduces to the specified fixed timestep. This should be considered an experimental feature, but tests have shown good stability and better applicability to more systems than the existing options (fixed or variable timestep).
- Fix: Requested system density was not quite achieved by `Add` nodes.
- Fix: Angle adjustment in `IntraShake` module was broken.
- Fix: Crash related to setting keyword data / `std::type_info` (OSX).
- Fix: Exponential spin widgets no longer get squeezed to minimum horizontal space (OSX).
- Fix: Crash when running after editing `Bragg` module parameters.
- Fix: Crash when clearing data in a `CoordinateSets` node.
- Fix: Missing improper conflict info in Add Forcefield Terms wizard.
- Fix: Crash when assigning terms to selected atoms in Add Forcefield Terms wizard.
- Fix: Master term parameters are now correctly overwritten when applying forcefield terms.
