# Current Branch Worklist

## Minor 

## Moderate
- Add option for Min/Max/Delta labels in Vec3Labels (and Vec3WidgetLabels), and set the Range keywords of Calculate1DProcedureNode and others to use them.
  - Need to add constructor argument to Vec3DoubleKeyword to take option for label type (see Vec3NodeKeyword for an example)
  - Update the Vec3DoubleKeywordWidget to have labels according to the specified labels type given to the Vec3DoubleKeyword (again, see Vec3NodeKeywordWidget ui for inspiration!)

## Main
- Convert ProcedureNodes to use keywords
  - Implement keyword types / widgets as necessary
- Run Module::setUp() when Module is finished being read, rather than in the main set-up call (which we are trying to avoid using).
