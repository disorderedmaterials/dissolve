---
title: Step 6 - Run the Analysis
type: docs
weight: 1
---

At this point, you can set the simulation running (although it is a pure analysis pipeline rather than a simulation):

{{< action type="key" >}}Ctrl-R{{< /action >}}

Dissolve will churn through the trajectory file until it hits iteration 21, at which point you will encounter some errors. This is because, for practical distribution reasons, the trajectory file only has twenty steps in it, hence attempting to read past the end of the twentieth frame (in step 21) causes an error.

Also, our {{< gui-module "SiteRDF" >}} module missed data from the first frame (which we used to test the RDF/F(Q) calculation). So let's reset everything and do it again:

{{< action type="menu" text="Simulation &#8680; Clear Module Data" />}}
{{< step text="Be brave, and click _OK_" />}}

This removes all of the calculated data (RDFs, S(Q), F(Q), and the analysed data) but also crucially gets rid of any information the {{< gui-module "ImportTrajectory" >}} module had on where it was looking at in the file. So, if you now run Dissolve again, the import will begin at the first frame of the file. Of course, it will still fail at iteration 21!

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
