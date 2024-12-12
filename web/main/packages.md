+++
title = "Dissolve"
linkTitle = "Packages"
+++

{{< blocks/cover title="Pre-Built Packages" image_anchor="top" height="min" >}}
{{< /blocks/cover >}}

<a name="release">

{{< blocks/section color="primary" >}}
<div class="mx-auto">
    <h2>Release {{< releaseversion >}}</h2>
</div>
{{< /blocks/section >}}

{{< blocks/section color="primary" >}}

<div class="mx-auto">
  <p>Released on {{< releasedate >}}. For the full release notes see the <a href="https://github.com/disorderedmaterials/dissolve/releases/tag/{{< releaseversion >}}">{{< releaseversion >}} release on GitHub</a>. All binary versions are 64 bit.</p>
</div>

{{< /blocks/section >}}

{{< blocks/section color="primary" >}}

{{% blocks/feature icon="fab fa-linux" title="Linux" %}}
{{< releaselink target="GUI" urlSuffix="" textSuffix="Binary" >}}
{{< releaselink target="GUI" urlSuffix=".sif" textSuffix="GUI Singularity Container" >}}
{{< releaselink target="CLI" urlSuffix=".sif" textSuffix="CLI Singularity Container" >}}
<a href='https://docs.projectdissolve.com/userguide/get/packages/#linux'>Singularity Usage Guide</a>
{{% /blocks/feature %}}

{{% blocks/feature icon=" fab fa-apple" title="Mac OS" %}}
{{< releaselink target="GUI" urlSuffix="-X64.dmg" textSuffix="DMG (Intel)" >}}
{{< releaselink target="GUI" urlSuffix="-ARM64.dmg" textSuffix="DMG (Silicon)" >}}
<a href='https://docs.projectdissolve.com/userguide/get/packages/#mac-osx'>OSX Installation</a>
{{% /blocks/feature %}}

{{% blocks/feature icon="fab fa-windows" title="Windows" %}}
{{< releaselink target="GUI" urlSuffix="-Win64.exe" textSuffix="Installer" >}}
{{< releaselink target="GUI" urlSuffix="-Win64.zip" textSuffix="Zip Archive" >}}
<a href='https://docs.projectdissolve.com/userguide/get/packages/#windows-10'>Windows Usage Guide</a>
{{% /blocks/feature %}}

{{% blocks/feature icon="fa fa-archive" title="Example Data" %}}
{{< releaselink target="examples" urlSuffix=".tar.gz" textSuffix="GZipped Tarball" >}}
{{< releaselink target="examples" urlSuffix=".zip" textSuffix="Zip Archive" >}}
{{% /blocks/feature %}}

{{< /blocks/section >}}

{{< blocks/section color="primary" >}}
{{< /blocks/section >}}

</a>


<a name="development">

{{< blocks/section color="secondary" >}}
<div class="mx-auto">
<h2>Development Version</h2>
</div>
{{< /blocks/section >}}

{{< blocks/section color="secondary" >}}

<div class="mx-auto">
<p>Packages built dynamically from the <a href="https://github.com/disorderedmaterials/dissolve/commits/develop">development branch</a> can be <a href="https://github.com/disorderedmaterials/dissolve/releases/tag/continuous">found on GitHub</a>. These packages contain the most up-to-date functionality, but may also contain bugs and other undocumented features. If you use these versions and discover a bug, please <a href="https://github.com/disorderedmaterials/dissolve/issues/new/choose">submit an issue</a> to highlight it. Please include the unique hash value of the version you were using in your report.</p>
</div>

{{< /blocks/section >}}

{{< blocks/section color="secondary" >}}
{{< /blocks/section >}}

</a>

<a name="archive">

{{< blocks/section color="white" >}}
<div class="mx-auto">
<h2>Legacy Versions</h2>
</div>
{{< /blocks/section >}}

{{< blocks/section color="white" >}}
<div class="mx-auto">
<p>Older versions of Dissolve can be found on the <a href="https://github.com/disorderedmaterials/dissolve/releases">releases page on GitHub</a>.
</div>
{{< /blocks/section >}}

{{< blocks/section color="white" >}}
{{< /blocks/section >}}

</a>
