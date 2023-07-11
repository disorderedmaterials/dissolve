---
title: Restraint Potential Functions
description: Functional forms for restraint potentials
---

Below are listed the interaction forms available for restraining potentials acting on individual atoms. All functional forms have an implied origin / reference coordinate defined by some other means (e.g. specified when adding additional potentials to configurations) with $r = r_{i} - r_{origin}$.

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Harmonic`|`k`|Simple harmonic well potential$$ E_{ij} = \frac{1}{2} k r^2 $$ $$ F_{ij} = -k r $$ |
