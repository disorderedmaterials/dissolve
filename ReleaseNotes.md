Version 1.3 is a minor release adding new functionality across several areas.

## Input File Compatibility with v1.2

Some users may experience errors when reading input files containing an `AxisAngle` module due to a small change made in keywords which accepted axis arguments. These keywords now take simplified string arguments of "X", "Y", or "Z" rather than "XAxis", "YAxis", or "ZAxis".

## Major Changes in v1.3

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

