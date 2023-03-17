Version 1.0 is a major release adding a significant amount of new functionality and bugfixes.

## Major Changes in v1.0

### Compatibility with Earlier Versions

Input files from 0.9.X are 99% compatible with 0.9, but may require a few small edits as detailed below.

Restart files from v0.9.X should work in v1.0.0 as written.

### Removed Modules

The `CalculateCN` module has been removed and its functionality absorbed into the `CalculateRDF` (now `SiteRDF` - see below) module. Input files containing references to the `CalculateCN` module will need to be modified in the following way:

Input file for v0.9.X
```
Module  CalculateRDF  'X-Y'
  Frequency  1
  Configuration  'NewConfiguration'
  DistanceRange  0.000000e+00  1.000000e+01  5.000000e-02
  SiteA  'X'  'COM'
  SiteB  'Y'  'COM'
EndModule

Module  CalculateCN  'X-YCN'
  Frequency  1
  RangeA  0.000000e+00  3.000000e+00
  RangeB  3.000000e+00  6.000000e+00
  RangeC  6.000000e+00  9.000000e+00
  SourceRDF  'X-Y'
EndModule
```

For v1.0.0 the `CalculateRDF` module becomes `SiteRDF` and absorbs the range-specification keywords. Note that in the existing `CalculateCN` module calculation of the coordination number over the first range was implicitly enabled, whereas now it must be explicitly requested:
```
Module  SiteRDF  'X-Y'
  Frequency  1
  Configuration  'NewConfiguration'
  DistanceRange  0.000000e+00  1.000000e+01  5.000000e-02
  SiteA  'X'  'COM'
  SiteB  'Y'  'COM'

  RangeAEnabled  True
  RangeA  0.000000e+00  3.000000e+00
  RangeB  3.000000e+00  6.000000e+00
  RangeC  6.000000e+00  9.000000e+00
EndModule
```

### Renamed Modules

Several modules have been renamed in order to simplify the general naming scheme. Mostly this is related to analysis modules which have had the "Calculate" prefix removed (e.g. `CalculateDAngle` is now just called `DAngle`) with the notable exception of `CalculateRDF` which has been renamed to `SiteRDF` to avoid clashes with the old `RDF` module, which itself has been renamed to `GR` for consistency.

When loading in input files from v0.9.X the old module names will still be recognised. However, when writing the input file out again they will be converted to the new naming scheme.

### Keyword Layout

A new layout scheme has been adopted in the GUI in order to better group keywords for modules and nodes by their purpose. An additional "Advanced" set of options is now available for some modules, and is designed to contain options that so not typically require user editing. All of this is designed to present the most relevant keywords and options in a clearer and more accessible manner.

## Other Significant Changes
- Dynamic sites can now be created from the GUI
- Optional integer and double keywords with explicit text values indicating a specific state are now used where appropriate.
- Better handling of name=value specification in GUI when changing functional forms.
- Saving the input file to a new filename now correctly writes to a restart file named after the new filename.
- Procedures are now fully editable from the GUI.
- Reference points have now been removed from the GUI.
- Miscellaneous small fixes to the GUI - signal handling, editability etc.

## Patch v1.0.1

- Fix crash when creating an RDF/CN layer from the main menu.
- Treat removed `UseHalfCellWidth` of the `GR` module as deprecated.
- Fix an issue where referenced restart files (with the `--restart` CLI keyword, or opened via File->Load Restart File in the GUI) became the target restart file to write to.

## Patch v1.0.2

- Fix crash when creating a SDF layer from the main menu.
- Better handling of errors/warnings from bad file/format specifications.
