Version 1.3.2 is a patch release fixing a host of smaller issues in the code.

## Major Changes in v1.3.2

Handling of errors encountered when reading input files has been greatly "softened" - Dissolve will now try to read as much as possible of the file, and continue over "bad" keywords/data wherever possible.

## Minor Changes in v1.3.2

- UI: GUI is now much more flexible when it comes to minimum size.
- UI: Styling of keyword widgets has been improved.
- UI: SpeciesSite keyword controls are now clearly labelled.
- UI: Layer context menu targeted the wrong tabs on occasion.
- Fix: Limits on HistogramCN binning caused issues in certain cases.
- Fix: Added option to export data from HistogramCN.
- Fix: Unused atom types are now pruned strategically in the code.
- Fix: CIFs containing space-separated quoted strings are now handled correctly.
- Fix: Module timing estimates were polluted by modules that did not run for a legitimate reason.
- Fix: AtomShake module would crash if the "NShakes" keyword was set to anything other than 1.
- Fix: Keywords targeting vectors of Modules allowed the same module to be specified multiple times.
- Docs: Updated silica example, correcting a few errors.

## Major Changes in v1.3.1

- Fix: Accumulate module did not correctly work with multiple specified `Target`s.
- Fix: Angle, AxisAngle, IntraAngle, and DAngle modules did not normalise output data correctly.
- I/O: Errors in input files are handled less aggressively, trying to continue loading the file wherever possible.

## Minor Changes in v1.3.1

- Fix: Crashes when modifying some aspects of sites.
- Fix: Occasional crashes when empty expressions were present in keywords in the GUI.
- Fix: Specifying a non-existent restart file on the command-line now raises an error.
- Fix: Interaction potentials keywords now have default parameters populated everywhere.
- Fix: General UI improvements for menu item actions.
- UI: Hyperlink messages status label to messages tab.
- Docs: Updated to reflect new nodes, fixed some other issues.

## Major Changes in v1.3

### Input File Compatibility with v1.2

Some users may experience errors when reading input files containing an `AxisAngle` module due to a small change made in keywords which accepted axis arguments. These keywords now take simplified string arguments of "X", "Y", or "Z" rather than "XAxis", "YAxis", or "ZAxis".


### Coordination Number Histograms

A new module `HistogramCN` has been introduced to calculate histograms of coordination numbers between sites, complementing the integral "full average sum" available in the `SiteRDF` module.

### Fragment Sites

A third site type - "Fragment" - has been added to extend the utility of dynamically-generated sites for analysis / procedural purposes. These sites are constructed from chemical environment / connectivity by means of NETA descriptions, and provide rich new options for determining specific site locations within complex systems.

### Manipulation of Sites in Procedures

With the introduction of the "Fragment" site type a new procedure node `RotateFragment` is now available, and can operate on a selection of fragment sites and apply rotations about a selected axis.

## Minor Changes in v1.3

- New: Added `random()` and `randomi(int)` functions to the expression parser.
- Code: Early-stage TOML input file I/O introduced (not yet available to the end user).
- Code: Significant renaming of files in the codebase to conform more uniformly to a single naming convention (lowerCamelCase).

