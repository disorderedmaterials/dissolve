# Current Branch Worklist

## Automatic Pair Potential
- Regenerate pairpotentials matrix whenever necessary
- Since there may be multiple different types of potential, need to store potential type in the forcefield now.
- Revamp ForcefieldTab with graphing, options for autogeneration.
- Table of indicators showing if PP are OK, succesfully cross-generated, or are missing
  - Generate missing potentials automatically if functional form is the same and the PP allows it
  - Allow option to add missing potentials that can't be generated from a basic source (e.g. UFF?) 
- Checks for NULL pairpotentials are still made before simulation begins?
- Store AtomType pointer in SpeciesAtom rather than global index?
