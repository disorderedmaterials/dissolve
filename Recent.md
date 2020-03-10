- New forcefields added (OPLS-AA alkanes, alkenes, alcohols, diols, and triols)
- Improved Add Forcefield Terms wizard, including assignment of terms to atom selections

### Updating to 0.6.0

#### Input File Compatibility

Some small changes in the input file have been made and will cause errors on load. To fix these:

1) In the PairPotentials block, the `Parameters` keyword now takes the element associated to the atom type as the second argument, followed by the atomic charge and short range form/parameters. To maintain compatibility, these elements must be added in by hand.
