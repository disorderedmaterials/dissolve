Version 0.9.1 is a patch release providing bugfixes, improvements, and some new functionality.

### Summary of Changes
- Added option to create pure MD evolution layer.
- Improved CIF wizard.
- Imported species are now immediately shown.
- Add Morse bond potentials.
- Add Zhao 2010 Cu-BTC forcefield.
- Add context menu to main tabs.
- Allow charge scaling over whole species.
- Allow duplication and H-manipulation of isotopologues.
- Reject velocities array with bad size when initialising MD.
- Be more selective about when to generate configurations.
- Recognise all 0.8.X versions when parsing restart file.
- Better UX on Forcefield tab.
- Reorganised main menus.
- Reorganise module control functions on layer tab.
- Better reporting / handling of non-zero species and configurations.
- Show periodic box information for species (if one exists).
- Fixes to data notification within GUI.
- Fix calculation of average species.
- Fix crash en EPSR/Potentials tab when no pairpotentials are defined.
- Fix cosine definitions in Zhang 2013 forcefield.
- Fix bond colours in periodic species rendering.
- Fix editing of atom type names.
- Fix crash when deleting layer.
- Fix crash when parsing empty ';' CIF blocks.
- Fix OSX and Windows packaging.
