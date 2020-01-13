---
title: Step 3 - Set up Equilibration
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Liquid Water

## 3. Set up Equilibration

To equilibrate our system we'll now add a standard Monte Carlo / Molecular Dynamics processing layer:

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)
{: .action .action_menu}

We can now start our simulation running and start to equilibrate the configuration box. If you're watching the box in the configuration tab you'll notice that it intially "explodes" because of our defined size factor of 10.0, but the box will quickly reduce in size back to normal and result in a condensed phase with no interlocked benzene rings.

When the size factor has reduced the equilibrated box should have a total energy of around -200 kJ mol<sup>-1</sup> (for 100 molecules). Once this has been achieved, you can move on to set up the analysis of the structural properties we're interested in.

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
