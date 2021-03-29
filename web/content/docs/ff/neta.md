---
title: NETA
description: The Nested English Typing Approach (NETA)
---

## Overview

The Nested English Typing Approach (NETA) originated in [Aten](https://www.projectaten.com/aten) as an attempt to describe the chemical environment of a given atom in a molecule based on its connectivity, and in a clear and readable way. In Dissolve its purpose is to describe how to recognise atoms within molecules described by specific forcefields, and to subsequently allow all forcefield terms to be applied automatically.

## Basics

Each atom type in a forcefield is described by a chemical element, a unique integer index, and a NETA string describing how to recognise it. The NETA string describes the environment of the atom to recognise beginning from the atom itself - for instance, specifying the number of bonds it should have - and then "walks" to neighbouring atoms and describes what their local environment should be, and then their neighbours, and so on. The depth to which neighbours are checked depends on how complicated the molecule / forcefield is, and how unique the atom we are describing happens to be. Parentheses surround NETA elements that describe the atom which immediately precedes it, and in principle can be used to nest descriptions to any depth.

For example, we could uniquely describe the alcohol hydrogen of methanol (CH<sub>3</sub>OH) with the NETA string `-O` as this is enough to differentiate it from the aliphatic hydrogens on the carbon. If we wanted to, we could nest descriptions to cover the whole molecule, giving `-O(-C(nh=3))`. Starting off from our target alcohol hydrogen, this can be read as "is bound to an oxygen, which itself is bound to a carbon, which itself has three attached hydrogen atoms".

There is (at present) no use made of bond orders within the NETA implementation in Dissolve.  It is assumed that input structures are chemically accurate, and all typing can be inferred from the connectivity alone.

## Type References

Uniquely describing atoms based on connectivity can get quite laborious when trying to tackle larger molecules, since this often requires the repetition of NETA fragments to identify atoms that have already been described. In most situations, using _type references_ can dramatically reduce the amount of NETA required, as previously-defined atoms can be referenced by their unique integer index when describing other atoms. A good tactic to use is to describe the most unique, "central" atom in a molecule as completely as possible, and then reference it when describing other atoms.

Type references are written as `&X` where `X` is either the unique index of the type or the type name.

## Algorithm

In practice, the NETA algorithm starts on a given atom and attempts to find a "chain" of atoms that match the connectivity specifications given by the description. If all descriptive elements are fulfilled, the atom is considered to be a match. Within the context of a complex forcefield in which there are many atom types, of which several may have NETA descriptions that match a given atom, so to differentiate them a score is calculated for a successful match, based on the complexity of the description. In this way, a more complex description will be chosen in preference to a simpler one with a lower score.

One important aspect of the NETA algorithm to bear in mind is that an atom cannot appear more than once in a chain - in other words, you can't write a connectivity description that backtracks over atoms you've already matched earlier on. The primary reason for this is to force connectivity descriptions to "move out" from the starting, "root" atom. The starting atom for the match may be allowed as a match if required, by using the `root` keyword (see examples below).

## NETA Reference

### Root Atom

The current atom being checked for a match to the description is the "root" atom. As well as providing connectivity and other information, some basic requirements of the root atom itself can be requested with the following modifiers:

|Keyword|Syntax|Description|
|:------|------|-----------|
|`geometry`|`geometry` `=`|`!=` [`type`](#geometry-types)|States that the root atom must (or must not) have the bond geometry specified.|
|`nbonds`|`nbonds` [`operator`](#comparison-operators) `value`|States that the bond count on the root atom must satisfy the given criterion.|
|`nh`|`nh` [`operator`](#comparison-operators) `value`|States that the total number of hydrogen atoms bound to the root atom must satisfy the given criterion.|

#### Examples

```
geometry=tet    # ... the root atom must be tetrahedral
nbonds>=2       # ... the root atom has at least two bonds
nh=1            # ... the root atom has exactly one hydrogen bound to it
```

### "Bound To" (`-`)

The `-` keyword specifies a connection to another atom that the current atom must have. The other atom is expressed as a single element or type reference, or a mixed list of both enclosed in square brackets. The atom/type specification may be followed by parentheses containing further requirements for the match. Within those parentheses, a number of modifiers and flags controlling the match are available:

|Keyword|Syntax|Description|
|:------|------|-----------|
|`geometry`|`geometry` `=`|`!=` [`type`](#geometry-types)|States that the bound atom must (or must not) have the bond geometry specified.|
|`n`|`n` [`operator`](#comparison-operators) `value`|Requests that there must be a certain number of unique matches of the bound atom.|
|`nbonds`|`nbonds` [`operator`](#comparison-operators) `value`|States that the bond count on the bound atom root atom must satisfy the given criterion.|
|`nh`|`nh` [`operator`](#comparison-operators) `value`|States that the total number of hydrogen atoms on the bound atom must satisfy the given criterion.|
|`root`|`root`|Permits the root atom to be matched.|

#### Examples

```
-C                 # ... is bound to a carbon atom
-[P,S]             # ... is bound to a phosphorous or sulphur atom
-[&6,&7,N]         # ... is bound to atom type with id 6 or 7, or a nitrogen
-C(-O)             # ... is bound to a carbon, which is bound to an oxygen
-Si(root)          # ... is bound to a silicon atom, which may also be the originating (root) atom
!-[N,P]            # ... is *not* bound to a nitrogen or a phosphorus
-C(nh=3)           # ... is bound to a carbon with exactly three hydrogen atoms
-C(nh<=2)          # ... is bound to a carbon with zero, one, or two hydrogen atoms
-C(nh=3,n=3)       # ... is bound to three carbons, each having three bound hydrogen atoms
-O(nbonds=1)       # ... is bound to an oxygen atom with exactly one bond
-Zn(geometry=sqp)  # ... is bound to a zinc atom which has a square-planar geometry
```

### Ring

On its own, the `ring` keyword indicates that the atom should be present in some sort of cyclic moiety of any size within the molecule. The exact size of the ring can be specified using the `size` modifier within parentheses immediately following the keyword. Within the same parentheses, the exact composition of the ring can be provided with a comma-separated list of elements or type references.

|Keyword|Syntax|Description|
|:------|------|-----------|
|`n`|`n` [`operator`](#comparison-operators) `value`|Requests that there must be a certain number of unique matches of the ring.|
|`size`|`size` [`operator`](#comparison-operators) `value`|Limits the size of acceptable rings to the specified criterion.|

For example:

```
ring(size=4)         # ... exists within a four-atom ring
ring(size>=6)        # ... exists within a ring of six atoms or greater
ring(n>=2,size>=6)   # ... exists within at least two rings of six atoms or greater
ring(C,C,N,C,C)      # ... exists in a ring containing the elemental sequence `C`, `C`, `N`, `C`, `C`
```

### Comparison Operators

|Operator|Description|
|:------|----------|
|`=`|Equal to|
|`!=`|Not equal to|
|`>`|Greater than|
|`<`|Less than|
|`>=`|Greater than or equal to|
|`<=`|Less than or equal to|

### Geometry Types

|Keyword|Description|
|:------|----------|
|`unbound`|Free atom (i.e. no bonds).|
|`terminal`|Exactly one bond.|
|`linear`|Two bonds forming a bond angle approaching 180&deg;.|
|`ts`|Three bonds in a T-shape arrangement.|
|`tp`|Three bonds forming a trigonal planar arrangement.|
|`tet`|Three or four bonds in a tetrahedral arrangement.|
|`sqp`|Four bonds in a square-planar arrangement.|
|`tbp`|Five bonds in a trigonal bipyramidal arrangement.|
|`oct`|Six bonds forming an octahedral arrangement.|
