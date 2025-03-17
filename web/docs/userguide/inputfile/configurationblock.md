---
title: Configuration Block
description: Describes the contents of a representative system, and how to generate it
---

## Overview

A `Configuration` block provides a complete description of an atomic / molecular system - e.g. a liquid, crystal, glass, or mixture thereof. A definition of how this system should be created is constructed in a subsection called the 'generator' - within that section, references are made to the species present in the configuration. As such, they must have already been defined in one or more [`Species`]({{< ref "speciesblock" >}}) blocks _before_ the `Configuration` block in the input file.

The block keyword itself takes a single (required) argument - the name of the configuration. This name is used to reference the configuration when, for example, specifying [module]({{< ref "moduleblock" >}}) targets.

## Keywords

|Keyword|                           Arguments                           |Default| Description                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|:------|:-------------------------------------------------------------:|:-----:|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|`CellDivisionLength`|                           `double`                            |`7.0`| Set the side length, in Angstroms, of sub-cell regions into which the configuration's unit cell will be partitioned for the purposes of near-neighbour calculation. The actual side length used to partition the cell will likely differ from the supplied value, in order to ensure full coverage of the whole unit cell volume.                                                                                                                       |
|`EndConfiguration`|                              --                               |--| Indicates the end of the current `Configuration` block.                                                                                                                                                                                                                                                                                                                                                                                                 |
|`Generator`|            [`Generator`]({{< ref "generators" >}})            |--| Specifies a stepwise [`Generator`]({{< ref "generators" >}}) able to generate the unit cell and contents of the configuration. The generator is called before the main simulation loop begins, unless an existing snapshot of the configuration has been loaded in from a restart file                                                                                                                                                       |
|`InputCoordinates`| [`CoordinatesFileAndFormat`]({{< ref "coordinatesformat" >}}) |--| Load atomic coordinates from the specified file and format. The `Generator` procedure is still used to construct the initial contents in terms of species etc., but the coordinates of the atoms are subsequently overwritten by those from the file.                                                                                                                                                                                                   |
|`SizeFactor`|                           `double`                            |`1.0`| Factor by which to temporarily scale the unit cell and molecule centres of geometry. This option is typically used to remove overlaps between molecules containing ring structures, which would otherwise become interlocked for the duration of the simulation. If greater than 1.0, the intermolecular energy is assessed at the beginning of each main loop iteration. If this energy is negative, the size factor is reduced by a predefined factor. |
|`Temperature`|                           `double`                            |`300`| Temperature of the configuration in Kelvin.                                                                                                                                                                                                                                                                                                                                                                                                             |

## Example

```r
Configuration  'Liquid'

  Generator
  	Box
  	  Lengths  1.0  1.0  1.0
  	  Angles  90.0  90.0  90.0
  	EndBox
  	Add
  	  Species  'SpeciesA'
  	  Population  '200'
  	  Density  '0.95'  g/cm3
  	EndAdd
  EndGenerator

  Temperature  300.000000

EndConfiguration
```

## Generator Examples

### Random One-Component System

Simple one-component system, e.g. a pure liquid, with a set number of molecules randomly added to a cubic unit cell. The size of the cell is not explicitly specified - giving a density value (here, the atomic density in atoms/&#8491;<sup>3</sup>) to the {{< node "Add" >}} node allows it to calculate the required volume for the number of molecules requested.

```r
Generator
  Box
	Lengths  1.0  1.0  1.0
	Angles  90.0  90.0  90.0
  EndBox
  Add
	Species  'Water'
	Population  '1000'
	Density  '0.1'  atoms/A3
	Positioning  Random
  EndAdd
EndGenerator
```

### Random Two-Component System

Two-component system, e.g. a liquid mixture, with a set number of molecules of the second species, and an implied ratio with the first. Again the size of the final cell is not explicitly stated, and the density units are provided in chemical units of the final mix (g/cm<sup>3</sup>). All control variables are defined in a separate {{< node "Parameters" >}}block at the beginning of the generator.

```r
Generator
  Parameters
	Parameter   N  1000
	Parameter   ratio   2.0
	Parameter   rho    0.9
  EndParameters
  Box
	Angles  90.0  90.0  90.0
	Lengths  1.0  1.0  1.0
  EndBox
  Add
	Species  'Water'
	Population  N*ratio
	Density  rho  g/cm3
  EndAdd
  Add
	Species  'Alcohol'
	Population  N
	Density  rho  g/cm3
  EndAdd
EndGenerator
```
