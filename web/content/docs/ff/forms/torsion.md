---
title: Torsion Functions
description: Functional forms for torsion interactions
---

Below are listed the interaction forms available for torsion angles (also called dihedral angles) within molecules, with energy and force equations provided in full. Parameters are listed in the input order expected by Dissolve.

{{< cimage src="../torsion.svg" caption="Torsion interaction between atoms $i$, $j$, $k$ and $l$, where the torsion angle $\phi_{ijkl}$ can be viewed as the angle made between the two planes defined by $i$-$j$-$k$ and $j$-$k$-$l$." >}}

Note that within input files any equilibrium angles $\phi_{eq}$ should be provided in degrees, but the equations as written below work internally in radians.

Force equations are derived by applying the chain rule, and where

$$ \frac{d\phi}{d\cos\phi} = -\frac{1}{\sin\phi} $$

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Cosine`|`k`</br>`n`</br>`$\phi_{eq}$`</br>`s`|Cosine torsion potential with periodicity $n$ $$ E_{ijkl} = k \left( 1 + s \cos(n \phi_{ijkl} - \phi_{eq}\right)) $$ $$ F_{ijkl} = k n s \sin(n \phi_{ijkl} - \phi_{eq}) \frac{d\phi}{d\cos\phi} $$ |
|`Cos3`|`$C_1$`</br>`$C_2$`</br>`$C_3$`|Three-term cosine potential $$ E_{ijkl} = \frac{1}{2} (C_1 (1 + \cos(\phi_{ijkl})) + C_2 (1 - \cos(2 \phi_{ijkl})) + C_3 (1 + \cos(3 \phi_{ijkl})) ) $$ $$ F_{ijkl} = -\frac{1}{2} (-C_1 \sin(\phi_{ijkl}) + 2 C_2 \sin(2 \phi_{ijkl}) - 3 C_3 \sin(3 \phi_{ijkl})) \frac{d\phi}{d\cos\phi} $$ |
|`Cos3C`|`$C_0$`</br>`$C_1$`</br>`$C_2$`</br>`$C_3$`|Three-term cosine potential with preceding constant$$ E_{ijkl} = C_0 \frac{1}{2} (C_1 (1 + \cos(\phi_{ijkl})) + C_2 (1 - \cos(2 \phi_{ijkl})) + C_3 (1 + \cos(3 \phi_{ijkl})) ) $$ $$ F_{ijkl} = -\frac{1}{2} (-C_1 \sin(\phi_{ijkl}) + 2 C_2 \sin(2 \phi_{ijkl}) - 3 C_3 \sin(3 \phi_{ijkl})) \frac{d\phi}{d\cos\phi} $$ |
|`Cos4`|`$C_1$`</br>`$C_2$`</br>`$C_3$`</br>`$C_4$`|Four-term cosine potential $$ E_{ijkl} = \frac{1}{2} (C_1 (1 + \cos(\phi_{ijkl})) + C_2 (1 - \cos(2 \phi_{ijkl})) + C_3 (1 + \cos(3 \phi_{ijkl})) + C_4 (1 - \cos(4 \phi_{ijkl})) ) $$ $$ F_{ijkl} = -\frac{1}{2} (-C_1 \sin(\phi_{ijkl}) + 2 C_2 \sin(2 \phi_{ijkl}) - 3 C_3 \sin(3 \phi_{ijkl}) + 4 C_4 \sin(4 \phi_{ijkl}) ) \frac{d\phi}{d\cos\phi} $$ |
|`CosN`|`$C_1$` ...|N-term cosine potential $$ E_{ijkl} = \sum_1^N C_N (1 + \cos(N \phi_{ijkl})) $$ $$ F_{ijkl} = -\sum_1^N -C_N N \sin(N \phi_{ijkl}) \frac{d\phi}{d\cos\phi} $$ |
|`CosNC`|`$C_0$` ...|N-term cosine potential plus constant $$ E_{ijkl} = \sum_0^N C_N (1 + \cos(N \phi_{ijkl})) $$ $$ F_{ijkl} = -\sum_0^N -C_N N \sin(N \phi_{ijkl}) \frac{d\phi}{d\cos\phi}$$ |
|`UFFCosine`|`k`</br>`n`</br>`$\phi_{eq}$`|Universal Force Field cosine potential $$ E_{ijkl} = \frac{1}{2} k (1 - \cos(n \phi_{eq}) \cos(n\phi_{ijkl})) $$ $$ F_{ijkl} = -\frac{1}{2} k \cos(n \phi_{eq}) n \sin(n\phi_{ijkl}) \frac{d\phi}{d\cos\phi} $$ |
|`FourierN`|`k`</br>`$C_0$` ...|N-term Fourier expansion $$ E_{ijkl} = k C_0 + \sum_1^N k C_N \cos(N \phi_{ijkl}) $$ $$ F_{ijkl} = -\sum_1^N -k C_N N \sin(N \phi_{ijkl}) \frac{d\phi}{d\cos\phi} $$ |
