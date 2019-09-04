# Current Branch Worklist

## Minor 
- Indentation for the Configuration's Generator data [Dissolve::saveInput()] EndGenerator...Fix writing of 'end' keywords for Collect1DProcedureNode and Collect2DProcedureNode (and 3D? Any others? Looks like AddSpecies and Box may have the same issue)
- Fix formatting of written Element and AtomType lists by DynamicSiteProcedureNode (remove quotes around lists, and remove spaces before first item)
- Indentation for the Configuration's Generator data (a Procedure) written by Dissolve::saveInput() is incorrect.
- Make the '-x' option also turn off writing of the heartbeat file (update CLIOptions.md doc accordingly, and adjust help text printed out by program).
## Moderate
- Add option for Min/Max/Delta labels in Vec3Labels (and Vec3WidgetLabels), and set the Range keywords of Calculate1DProcedureNode and others to use them.
  - Need to add constructor argument to Vec3DoubleKeyword to take option for label type (see Vec3NodeKeyword for an example)
  - Update the Vec3DoubleKeywordWidget to have labels according to the specified labels type given to the Vec3DoubleKeyword (again, see Vec3NodeKeywordWidget ui for inspiration!)

## Main
- Convert ProcedureNodes to use keywords
  - Implement keyword types / widgets as necessary
<<<<<<< HEAD

=======
- Run Module::setUp() when Module is finished being read, rather than in the main set-up call (which we are trying to avoid using).
>>>>>>> 95b126e829c95bdc76d42293b2de00e61a505f22
