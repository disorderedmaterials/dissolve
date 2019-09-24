# Develop Branch Worklist

## Prep
- Spatial density function calculation
- 2D plotting
- 3D plotting of surfaces (marching cubes)

## Main
- Fix documentation (include path for mermaid)
- Add water example
- Add benzene example (inc. sdf/slicing comparison between equilibrated / refined simulations)
- Add silica example
- Convert quickstart guide to page in docs.

## Other
- [General] Make Module::frequency_ default to 1 in all cases?  Then suggests layers for shake / calc / refine, which is quite neat... Still allow adjusting.
- [General] Harmonise command-line arguments between CLI and GUI versions (use gengetopt?)
- [Workflow] Run Module::setUp() when Module is finished being read, rather than in the main set-up call (which we are trying to avoid using).

## GUI
- Add option for Min/Max/Delta labels in Vec3Labels (and Vec3WidgetLabels), and set the Range keywords of Calculate1DProcedureNode and others to use them.
  - Need to add constructor argument to Vec3DoubleKeyword to take option for label type (see Vec3NodeKeyword for an example)
  - Update the Vec3DoubleKeywordWidget to have labels according to the specified labels type given to the Vec3DoubleKeyword (again, see Vec3NodeKeywordWidget ui for inspiration!)

## More
- Importing CIF structures as Species - break up into molecular units? Need to provide basic information on the contained units, probably as empirical formula (e.g. for CuBTC would be 'Cu' and 'C9O6H3'). This info can then be used to find, rotate, and re-map those molecules in a unit cell (folded molecularly). The Species then contains a list of copies of that particular molecular unit, at real coordinates.  These can then be pasted into a supercell, replicating the crystal. Need to have options in the AddSpecies node to utilise symmetry copies. A species may also contain multiple variants of coordinates in the form of conformers (potentially weighted by some factor, e.g. deltaE??). Store both these in a list of copies_, storing coordinates only (we will always use the basic Species information as a template for elements, bonds etc., as well as forcefield terms.
