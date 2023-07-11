---
title: Simple Potential Functions
description: Functional forms for simple additional potentials
---

Below are listed the interaction forms available for simple additional potentials acting on individual atoms. All functional forms have an implied origin / reference coordinate defined by some other means (e.g. specified when adding additional potentials to configurations) with $r = r_{i} - r_{origin}$.

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Harmonic`|`k`|Simple harmonic well potential$$ E_{ij} = \frac{1}{2} k r^2 $$ $$ F_{ij} = -k r $$ |
|`LJ`|`$\epsilon$`</br>`$\sigma$`|Standard Lennard-Jones 12-6 potential. $$ E = 4 \epsilon \left( \left(\frac{\sigma}{r}\right)^{12} - \left(\frac{\sigma}{r}\right)^6 \right) $$ $$ F = -48 \epsilon \left( \frac{\sigma^{12}}{r^{13}} - \frac{1}{2} \frac{\sigma^6}{r^7} \right) $$ $epsilon$ and $sigma$ are used as-is and are not combined with the target atom's parameters.|
