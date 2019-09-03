# Current Branch Worklist

## Minor 
- Indentation for the Configuration's Generator data [Dissolve::saveInput()] EndGenerator....
## Moderate
- Convert BinWidth/RMax/RMin keywords in CalculateRDFModule::setUpKeyword() to use same keyword type as Calculate1DProcedureNode does for 'RangeX'
  - Will need to change CalculateRDFModule::process() to search for and retrieve the new keyword value, instead of search for "BinWidth", "RMin", and "RMax".
  - Will need to change tests/calculate_rdf to use new keywords.
- Do same conversion for CalculateDAngleModule

## Main
- Convert ProcedureNodes to use keywords
  - Implement keyword types / widgets as necessary

