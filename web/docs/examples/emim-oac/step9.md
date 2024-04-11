---
title: Step 9 - Ion-Ion RDFs
type: docs
weight: 1
---

Here we'll set up calculation of some basic ionic liquid properties in the form of the three unique ion-ion radial distribution functions - the cation-anion, cation-cation, and anion-anion flavours - and the cation-anion coordination number.

So, let's create and prepare suitable layer:

{{< action type="menu" text="Layer &#8680; Create &#8680; Analysis &#8680; RDF & Coordination Number" />}}
{{< action type="tabs" >}}Go to the {{< gui-tab text="Analyse RDF/CN" type="Layer" >}} layer {{< /action >}}
{{< action type="edit" >}}Double-click the "SiteRDF" module in the list and rename it to `RDF-CA`{{< /action >}}
{{< action type="edit" >}}Change the **DistanceRange** maximum to `14.0` (slightly less than our half-cell length) {{< /action >}}
{{< action type="edit" >}}Set **SiteA** to `RingNN` on the cation {{< /action >}}
{{< action type="edit" >}}Set **SiteB** to `COM` on the anion {{< /action >}}

The {{< module "SiteRDF" >}} module can also calculate simple coordination numbers over specific distance ranges, however typically we don't know what this range is until we've seen the actual RDF data, so we'll leave that calculation turned off for now.

Now to add on two more {{< module "SiteRDF" >}} modules for the other correlations of interest:

{{< action type="mouse">}}Drag a {{< module "SiteRDF" >}} module from the palette up to the module list{{< /action >}}
{{< action type="mouse">}}Double-click the new {{< module "SiteRDF" >}} module and rename it to `RDF-CC`{{< /action >}}
{{< action type="edit" >}}Change the **DistanceRange** maximum to `14.0` {{< /action >}}
{{< action type="edit" >}}Set **SiteA** and **SiteB** to `RingNN` on the cation {{< /action >}}
{{< action type="mouse">}}Drag another {{< module "SiteRDF" >}} module from the palette up to the module list{{< /action >}}
{{< action type="mouse">}}Double-click the second new {{< module "SiteRDF" >}} module and rename it to `RDF-AA`{{< /action >}}
{{< action type="edit" >}}Change the **DistanceRange** maximum to `14.0` {{< /action >}}
{{< action type="edit" >}}Set **SiteA** and **SiteB** to `COM` on the anion {{< /action >}}

Now start up the simulation again:

{{< action type="key" >}}Ctrl-F{{< /action >}}
{{< step >}}Set the number of iterations to `250` and hit {{< gui-button text="OK" >}}{{< /step >}}

Because our system is quite small (100 ion pairs) the RDFs will take a few thousand iterations to become properly averaged, but 250 steps should be enough to get a feel for the results. If you look at the output from the RDF-CA module you should see that the first minimum occurs at around 7 &#8491;, so we'll set this value as the upper limit for our coordination number calculation:

{{< action type="mouse" >}}Find the _Coordination Numbers_ options in the {{< module "SiteRDF" >}} module{{< /action >}}
{{< action type="edit" >}}Set **RangeA** to span `0.0` to `7.0`{{< /action >}}
{{< action type="edit" >}}Click **RangeAEnabled** to request the calculation{{< /action >}}

Start the simulation up once more:

{{< action type="key" >}}Ctrl-R{{< /action >}}

The coordination number should average out at about 6.2 anions (within 7.0 &#8491; of a central cation), compared to 6.8 (EPSR) or 6.9 (pure molecular dynamics) in the [2010 paper](https://doi.org/10.1021/jp102180q).

* * *
{{< button pos="left" text="Previous Step" path="step8/">}}
{{< button pos="right" text="Exercises" path="exercises/">}}
