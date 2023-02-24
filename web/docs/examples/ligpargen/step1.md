---
title: Step 1 - SMILES Code
type: docs
weight: 1
---

[LigParGen](http://zarbi.chem.yale.edu/ligpargen/) needs to know which molecule you're interested in - we can provide that information either by uploading the full atomic coordinates or by giving it a Simplified Molecular-Input Line-Entry System (SMILES) code. Finding out the SMILES code for your molecule is often just a matter of checking chemical information on a supplied website, or just performing a web search (e.g. for "caffeine smiles code"). So that you know what to expect, some example codes are:

| Molecule | SMILES Code |
|:--------:|-------------|
|Ethanol|`OCC`|
|Hexane|`CCCCCC`|
|Benzene|`C1CCCCC1`|
|Imidazole|`c1c[nH]cn1`|
|Caffeine|`Cn1cnc2n(C)c(=O)n(C)c(=O)c12`|

### Step 1

With your SMILES code in hand, you can go to the [LigParGen service](http://zarbi.chem.yale.edu/ligpargen/) and enter it in "Step 1".

### Step 2

The service will convert your SMILES code into a full 3D molecule which can be further optimised if you wish by choosing a number of _Molecule Optimisation Iterations_ if you wish. If your molecule is charged you can enter that here as well in order to get suitable atomic charges on the molecule (noting the caveats stated on the website).

When you're ready, click the {{< gui-button text="Submit Molecule" >}} button.

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2/">}}
