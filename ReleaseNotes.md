Version 0.9.9 is a patch release which fixes a trio of issues.

The full release notes for 0.9.0 detailing major changes in this version can be found at https://github.com/disorderedmaterials/dissolve/releases/tag/0.9.0.

### Summary of Changes
- Fix: Adjusting torsions in the `IntraShake` module would destroy parts of molecules attached to rings.
- Fix: Specifying multiple targets in the `Accumulate` module now accumulates separate data for each module (not just the first).
- Fix: Fourier transform of reference structure factor data into G(r) was broken in some situations due to the presence of empty configurations.
