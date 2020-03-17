- New forcefields added (OPLS-AA alkanes, alkenes, alcohols, diols, and triols)
- Improved Add Forcefield Terms wizard, including assignment of terms to atom selections

### Updating to 0.6.X

#### Input File Compatibility

Some small changes in the input file have been made and will cause errors on load. To fix these:

1) In the PairPotentials block, the `Parameters` keyword now takes the element associated to the atom type as the second argument, followed by the atomic charge and short range form/parameters. To maintain compatibility, these elements must be added in by hand.

#### Export Module(s)

The existing ExportModule has been split into separate ExportCoordinates, ExportPairPotentials, and ExportTrajectory modules for greater control. So, change any usages as follows:

```
ExportModule
  WriteCoordinates  xyz  saved/config.xyz
  EndWriteCoordinates
EndExportModule
```

... should now be ...

```
ExportCoordinatesModule
  Format  xyz  saved/config.xyz
  EndFormat
EndExportCoordinatesModule
```
