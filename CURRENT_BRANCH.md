# Current Branch Worklist

## Minor
- Fix writing of 'end' keywords for Collect1DProcedureNode and Collect2DProcedureNode (and 3D? Any others? Looks like AddSpecies and Box may have the same issue)
- Fix formatting of written Element and AtomType lists by DynamicSiteProcedureNode (remove quotes around lists, and remove spaces before first item)
- Indentation for the Configuration's Generator data (a Procedure) written by Dissolve::saveInput() is incorrect.
- Make the '-x' option also turn off writing of the heartbeat file (update CLIOptions.md doc accordingly, and adjust help text printed out by program).

## Moderate
- Convert BinWidth/RMax/RMin keywords in CalculateRDFModule::setUpKeyword() to use same keyword type as Calculate1DProcedureNode does for 'RangeX'
  - Will need to change CalculateRDFModule::process() to search for and retrieve the new keyword value, instead of search for "BinWidth", "RMin", and "RMax".
  - Will need to change tests/calculate_rdf to use new keywords.
- Do same conversion for CalculateDAngleModule

## Main
- Convert ProcedureNodes to use keywords
  - Implement keyword types / widgets as necessary
