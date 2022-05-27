Version 0.9 is a major release adding a significant amount of new functionality and bugfixes.

## Major Changes

### Streamlined GUI

The GUI has been heavily modified in order to reorganise and simplify the layout, as well as to remove many custom display widgets (e.g. those displaying module layers) in favour of using Qt's own model/view architecture. The start screen has been removed, and messages are now contained on their own tab instead of living at the bottom of the main window. Run controls on the left-hand side have been removed.

Many other small improvements to the GUI have also been made, covering aesthetics such as new icons, additional menu options for common tasks (e.g. saving restart files), and usability features such as better awareness of data invalidation (e.g. when changing critical module options) and the ability to filter datasets on graphs (e.g. partial g(r) and S(Q)).

### Input File Compatibility

Input files from 0.8.X are 99% compatible with 0.9, but require a few small edits:

#### AddSpecies Nodes

The `AddSpecies` procedure node used in configuration generators has been renamed to just `Add`. So, any configuration generators should be modified from:

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

#### RDF Module Configuration Targets

The `RDF` module has a `Configurations` (plural) keyword rather than a `Configuration`(singular) keyword. This is to distinguish between modules that acept only a single configuration target and those that accept many (of which the only example is currently `RDF`). `RDF` modules in the input file should be changed from:

```
Module  RDF  'RDF01'
  Frequency  1
  Configuration  'Bulk'
  ...
```
to read:
```
Module  RDF  'RDF01'
  Frequency  1
  Configurations  'Bulk'
  ...
```

#### Functional Form Parameter Checking

More strict checking on the number of parameters given to forcefield functions is now made. Previous versions of Dissolve wrote extraneous parameters (all zero) to some of these lines, which will lead to errors such as this when opening the input file:
```
*** ERROR 'ShortRangeType' keyword 'LJ' requires exactly 2 arguments, but 4 were provided.
```

The additional parameters referenced can simply be removed, for example:
```
Parameters  HD  H  1.342000e-01  LJ  1.255200e-01  2.500000e+00  0.000000e+00  0.000000e+00
```
should be changed to:
```
Parameters  HD  H  1.342000e-01  LJ  1.255200e-01  2.500000e+00
```

#### Simulation Block

The `Simulation` block has been removed as it was no longer used (random seed setting is now performed by the command-line option `--seed`).  The block and its content can be safely deleted.

### Restart File Compatibility

From version 0.9.0 Dissolve will try to detect the version of the restart file being read in, and adjust its strategy accordingly.  As such, restart files from the 0.8.X versions can be seamlessly loaded into the 0.9.0 version. The corresponding input file will still need some adjustment as detailed above.

## Other Significant Changes
- Singularity images of all versions of Dissolve are now available.
- Major code cleanup and modernisation throughout, with only a few legacy custom classes now remaining.
- Forcefield parameter editing and input/output has been extended to "name=value" to ensure clarity on which parameters are being given which values.
- Improved consistency of EPSR module.
- Detection of "invalid" system charges, and display of total atomtype- and atom-derived charges on species tabs.
- Generation of coordinate sets for species being added to configurations, giving better starting population of molecules.
