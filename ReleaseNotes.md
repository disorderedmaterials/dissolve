Version 0.9.4 is a patch release providing several bugfixes.

The full release notes for 0.9.0 detailing major changes in this version can be found at https://github.com/disorderedmaterials/dissolve/releases/tag/0.9.0.

### Summary of Changes
- Fix: CIF space groups now detected much more accurately from files.
- Fix: CIFs containing no site labels now have atoms created correctly.
- Fix: Legacy data removal broke current data in some cases.
- Fix: Incorrect text on angles group in Add Configuration Wizard.
- Fix: Total, bound, and unbound functions are now written correctly by `PartialSet`.
- Fix: Correctly set `BoxAction` parameters when using a fixed size cell in Add Configuration Wizard.
