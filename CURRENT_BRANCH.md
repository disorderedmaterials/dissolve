# Current Branch Remit

Create new nodes for generating Configurations from 'recipes', for the purpose of beginning a simulation from scratch, and with control over parameters of imprtance so that aspect of the Configuration's composition / geometry / and general form may be 'fitted' by some means.

- [DONE] Plenty of atomic operations common with AnalysisNodes (e.g. selection of Species) so merge existing and new nodes into a ProcedureNode class. Each node deriving from this base class will implement a pure virtual context() function, returning in which contexts (e.g analysis, config generation) the node is relevant.
- [DONE] Numerical arguments to any of the keywords for any node will need to be able to recognise when an Expression has been provided. Sounds like a new keyword type that is able to deduce whether a simple value has been given, or has been given an expression that should be parsed.
- [DONE] ParameterNode: Variables/parameters that should be listable / accessible / targetable by Modules should be created within a ParameterNode.
- [DONE] CellNode: Provides basic or complete information for the unit cell to be used for the Configuration. It will encompass Configuration's existing Density, CellLengths, and CellAngles keywords. Defaults will be as currently set - cubic, 1.0 A box length. Density shall *not* have a default value - default behaviour will be that the provided cell lengths and angles are absolute, and cannot be modified. Providing a Density value will allow automatic box resizing.
- [DONE] AddSpeciesNode: Immediately adds the specified Species to the current cell, following the options set within. If the unit cell does not exist, it is created according to the definition provided in the UnitCell node (following specification of density if provided). If it already exists and has a prescribed density rather than volume, the volume is expanded to accommodate the new molecules to be created. Replicates and extends the content of the SpeciesInfo Configuration block
- [AS ISSUE] RandomiseConformers (or a better name): randomise individual molecules of the specified type by randomly rotating about bonds throughout the molecule. Need to check for internal overlaps (i.e. calculate internal energy)
- Generate errors for argument numbers to nodes.
- Implement ProcedureNode::save() virtuals properly.
- Implement Expression::asString() to return Expression as a string for saving.
- Tidy Configuration header / functions - move stuff that was in Composition to be in Content, and put input file coordinates somewhere sensible.

FOLLOW_ON:
- Importing CIF structures as Species - break up into molecular units? Need to provide basic information on the contained units, probably as empirical formula (e.g. for CuBTC would be 'Cu' and 'C9O6H3'). This info can then be used to find, rotate, and re-map those molecules in a unit cell (folded molecularly). The Species then contains a list of copies of that particular molecular unit, at real coordinates.  These can then be pasted into a supercell, replicating the crystal. Need to have options in the AddSpecies node to utilise symmetry copies. A species may also contain multiple variants of coordinates in the form of conformers (potentially weighted by some factor, e.g. deltaE??). Store both these in a list of copies_, storing coordinates only (we will always use the basic Species information as a template for elements, bonds etc., as well as forcefield terms.

Configuration  'Bulk'

  // Standard N-molecule mix
  Generation
    Parameters
      Integer  multiplier  100
    EndParameters
    Cell
      Density  0.1  atoms/ang3
    EndCell
    AddSpecies
      Species  'Water'
      Population  3*multiplier
    EndAddSpecies
    AddSpecies
      Species  'Methanol'
      Population  6*multiplier
      #NoRotation
    EndSpecies
  EndGeneration

  // 3-molecule mix of A solvating B and C, with ratio control between B and C
  Generation
    Parameters
      Double   ratioBC     0.8
    EndParameters
    Cell
      Density  0.111  atoms/ang3
    EndCell
    AddSpecies
      Species  'A'
      Population  1000
    EndAddSpecies
    AddSpecies
      Species  'B'
      Population  100*ratioBC
    EndAddSpecies
    AddSpecies
      Species  'C'
      Population  100*(1.0-ratioBC)
    EndAddSpecies
  EndGeneration

  // Porous crystal structure with variable adsorbate loading
  Generation
    Parameters
      Integer  loading  30
    EndParameters
    Cell
    EndCell
    // Externally-available control options for generation
    Option  double  radius  21.0;
    Recipe
      copyConfiguration();
    EndRecipe
  EndGeneration

  // Variable pore radius MCM-41 with adsorbates
  Generation
    // Externally-available control options for generation
    Option  double  radius  21.0;
    Recipe
      //double radius = 21.0;	// Satisfied by the Option command
      
    EndRecipe
  EndGeneration

EndConfiguration

    Select  'O1'
      Site  Water  'O'
      ForEach
        Select  'O2'
          Site  Water  'O'
          ExcludeSameMolecule  'A'
          ForEach
            Calculate  'rOO'
              Distance  'O1'  'O2'
            EndCalculate
            Collect1D  'HistoOO'
              QuantityX  'rOO'
              RangeX  0.0  20.0  0.01
            EndCollect1D
          EndForEach
        EndSelect
      EndForEach
    EndSelect

