---
title: Bond Functions
description: Functional forms for bond interactions
---

Below are listed the interaction forms available for bonds within molecules, with energy and force equations provided in full. Parameters are listed in the input order expected by Dissolve.

{{< cimage src="../bond.svg" caption="Bond interaction between atoms $i$ and $j$, where the bond length is $r_{ij}$." >}}

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Harmonic`|`k`</br>`$r_{eq}$`|Simple harmonic bond strech $$ E_{ij} = \frac{1}{2} k (r_{ij} - r_{eq})^2 $$ $$ F_{ij} = -k (r_{ij} - r_{eq}) $$ |
|`EPSR`|`C'`</br>`$r_{eq}$`|Harmonic bond weighted by the masses of the atoms - equivalent to that implemented in EPSR, where $ C' = C/2$. $$ E_{ij} = C' (r_{ij} - r_{eq})^2 / \omega $$ $$  F_{ij} = - 2 C' (r_{ij} - r_{eq}) / \omega $$ $$ \omega = \frac{r_{eq}}{\sqrt{ \frac{m_i + m_j}{m_i m_j}}} $$|
