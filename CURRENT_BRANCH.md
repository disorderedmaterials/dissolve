# Current Branch Worklist

## Forcefield Specification for Species
- Allow source forcefield to be specified in input file, indicating that intramolecular and atomtype terms should be automatically selected / generated
  - Still allow specification of bonds to provide connectivity, but remove need to supply forcefield terms in those lines
  - Add CalculateBonding option to determine bonding between current atoms (also takes tolerance argument as with Aten)
  - Term generation will be performed on load, and if any significant changes are made to the model

## Automatic Pair Potential
- Regenerate pairpotentials matrix whenever necessary
- Since there may be multiple different types of potential, need to store potential type in the forcefield now.
- Revamp ForcefieldTab with graphing, options for autogeneration.
- Table of indicators showing if PP are OK, succesfully cross-generated, or are missing
  - Generate missing potentials automatically if functional form is the same and the PP allows it
  - Allow option to add missing potentials that can't be generated from a basic source (e.g. UFF?) 
- Checks for NULL pairpotentials are still made before simulation begins?
- Store AtomType pointer in SpeciesAtom rather than global index?
