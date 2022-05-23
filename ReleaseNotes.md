Version 0.9 is a major release adding  a significant amount of new functionality and bugfixes.

## Major Changes

### Streamlined GUI

The GUI has been heavily modified in order to reorganise and simplify the layout, as well as to remove many custom display widgets (e.g. those displaying module layers) in favour of using Qt's own model/view architecture. The start screen has been removed, and messages are now contained on their own tab instead of living at the bottom of the main window. Run controls on the left-hand side have been removed.

Many other small improvements to the GUI have also been made, covering aesthetics such as new icons, additional menu options for common tasks (e.g. saving restart files), and usability features such as better awareness of data invalidation (e.g. when changing critical module options) and the ability to filter datasets on graphs (e.g. partial g(r) and S(Q)).

### Input File Compatibility

Input files from 0.8.X are 99% compatible with 0.9, with two small edits required:

1) The `AddSpecies` node has been renamed to just `Add`. So, any configuration generators should be modified from:

```
Generator
  AddSpecies
    Species  'Water'
    ...
  EndAddSpecies
  ...
```
to read:
```
Generator
  Add
    Species  'Water'
    ...
  EndAdd
  ...
```

2) The `RDF` module has a `Configurations` keyword rather than a `Configuration` keyword. This is to distinguish between modules that acept only a single configuration target and those that accept many (of which the only example is currently `RDF`). `RDF` modules in the input file should be changed from:

```
Module  RDF  'RDF01'
  Frequency  1
  Configuration  'Bulk'
  ...
```
to read:
```
```
Module  RDF  'RDF01'
  Frequency  1
  Configurations  'Bulk'
  ...
```

### Restart File Compatibility

Restart files from the 0.8.X version are not immediately compatible with 0.9, but can be made compatible with some minor editing by hand. Essentially, the breaking change is that `AtomTypeList` data is no longer saved to the restart file (rather it is recalculated on-the-fly when required). So, any `AtomTypeList` entries in the restart file simply need to be deleted. An example of such an entry is as follows:

```
Processing  RDF01//SummedAtomTypes  AtomTypeList  17  1
3  # nItems
CT2 10000 0.2631578947368421 0.0 0
CT3 2000 0.05263157894736842 0.0 0
HC 26000 0.6842105263157895 0.0 0
```

The entry comprises a header line, followed by the number of items _N_ to expect, followed by _N_ lines of data, so _N_+2 lines in total. All of these lines up to the next header line can safely be deleted.

## Other Significant Changes
- Singularity images of all versions of Dissolve are now available.
- Major code cleanup and modernisation throughout, with only a few legacy custom classes now remaining.
- Editing of forcefield terms is now parameter-based.
- Improved consistency of EPSR module.
- Detection of "invalid" system charges, and display of total atomtype- and atom-derived charges on species tabs.
- Generation of coordinate sets for species being added to configurations, giving better starting population of molecules.
