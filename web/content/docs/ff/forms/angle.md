---
title: Angle Functions
description: Functional forms for angle interactions
---

Below are listed the interaction forms available for angles within molecules, with energy and force equations provided in full. Parameters are listed in the input order expected by Dissolve.

{{< cimage src="../angle.svg" caption="Angle interaction between atoms $i$, $j$ and $k$, where the bond angle is $\theta_{ijk}$." >}}

Note that within input files any equilibrium angles $\theta_{eq}$ should be provided in degrees, but the equations as written below work internally in radians.

Force equations are derived by applying the chain rule, and where

$$ \frac{d\theta}{d\cos\theta} = -\frac{1}{\sin\theta} $$

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Harmonic`|`k`</br>`$\theta_{eq}$`|Simple harmonic angle bend $$ E_{ijk} = \frac{1}{2} k (\theta_{ijk} - \theta_{eq})^2 $$ $$ F_{ijk} = \frac{d\theta}{d\cos\theta} k (\theta_{ijk} - \theta_{eq}) $$ |
|`Cosine`|`k`</br>`n`</br>`$\theta_{eq}$`</br>`s`|Cosine angle bend with periodicity $n$ $$ E_{ijk} = k \left( 1 + s \cos(n \theta_{ijk} - \theta_{eq}\right)) $$ $$ F_{ijk} = \frac{d\theta}{d\cos\theta} k n s \sin(n \theta_{ijk} - \theta_{eq}) $$ |
|`Cos2`|`k`</br>`$C_0$`</br>`$C_1$`</br>`$C_2$`|Double cosine angle potential $$ E_{ijk} = k (C_0 + C_1 \cos(\theta_{ijk}) + C_2 \cos(2 \theta_{ijk}) ) $$ $$ F_{ijk} = \frac{d\theta}{d\cos\theta} k (C_1 \sin(\theta_{ijk}) + 2 C_2 \sin(2 \theta_{ijk}) )  $$ |
