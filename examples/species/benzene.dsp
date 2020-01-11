# OPLS-AA Benzene with adjusted bond distances

Master
  Bond   CH   Harmonic  3071.056  1.09
  Bond   CC   Harmonic  3924.592  1.38
  Angle  HCC  Harmonic  292.8800  120.0
  Angle  CCC  Harmonic  527.1840  120.0
  Torsion  HCCH  Cos3  0.0  30.334  0.0
  Torsion  HCCC  Cos3  0.0  30.334  0.0
  Torsion  CCCC  Cos3  0.0  30.334  0.0
EndMaster

Species 'Benzene'
  # Atoms
  Atom   1  C   -1.203775     0.695000     0.000000   CA
  Atom   2  H   -2.069801     1.195000     0.000000   HA
  Atom   3  C   -0.000000     1.390000     0.000000   CA
  Atom   4  H   -0.000000     2.390000     0.000000   HA
  Atom   5  C    1.203775     0.695000     0.000000   CA
  Atom   6  H    2.069801     1.195000     0.000000   HA
  Atom   7  C    1.203775    -0.695000     0.000000   CA
  Atom   8  H    2.069801    -1.195000     0.000000   HA
  Atom   9  C   -0.000000    -1.390000     0.000000   CA
  Atom  10  H   -0.000000    -2.390000     0.000000   HA
  Atom  11  C   -1.203775    -0.695000     0.000000   CA
  Atom  12  H   -2.069801    -1.195000     0.000000   HA

  # Intramolecular Terms
  Bond  1     2  @CH
  Bond  1     3  @CC
  Bond  1    11  @CC
  Bond  3     4  @CH
  Bond  3     5  @CC
  Bond  5     6  @CH
  Bond  5     7  @CC
  Bond  7     8  @CH
  Bond  7     9  @CC
  Bond  9    10  @CH
  Bond  9    11  @CC
  Bond 11    12  @CH
  Angle    2     1     3   @HCC
  Angle    2     1    11   @HCC
  Angle    3     1    11   @CCC
  Angle    1     3     4   @HCC
  Angle    1     3     5   @CCC
  Angle    4     3     5   @HCC
  Angle    3     5     6   @HCC
  Angle    3     5     7   @CCC
  Angle    6     5     7   @HCC
  Angle    5     7     8   @HCC
  Angle    5     7     9   @CCC
  Angle    8     7     9   @HCC
  Angle    7     9    10   @HCC
  Angle    7     9    11   @CCC
  Angle   10     9    11   @HCC
  Angle    1    11     9   @CCC
  Angle    1    11    12   @HCC
  Angle    9    11    12   @HCC
  Torsion   2     1     3     4  @HCCH
  Torsion   2     1     3     5  @HCCC
  Torsion  11     1     3     4  @HCCC
  Torsion  11     1     3     5  @CCCC
  Torsion   2     1    11     9  @HCCC
  Torsion   2     1    11    12  @HCCH
  Torsion   3     1    11     9  @CCCC
  Torsion   3     1    11    12  @HCCC
  Torsion   1     3     5     6  @HCCC
  Torsion   1     3     5     7  @CCCC
  Torsion   4     3     5     6  @HCCH
  Torsion   4     3     5     7  @HCCC
  Torsion   3     5     7     8  @HCCC
  Torsion   3     5     7     9  @CCCC
  Torsion   6     5     7     8  @HCCH 
  Torsion   6     5     7     9  @HCCC
  Torsion   5     7     9    10  @HCCC
  Torsion   5     7     9    11  @CCCC
  Torsion   8     7     9    10  @HCCH
  Torsion   8     7     9    11  @HCCC
  Torsion   7     9    11     1  @CCCC
  Torsion   7     9    11    12  @HCCC
  Torsion  10     9    11     1  @HCCC
  Torsion  10     9    11    12  @HCCH

  Isotopologue  Deuterated  HA=2
EndSpecies

PairPotentials
  Parameters  'CA'  -0.115  LJGeometric  0.29288  3.55
  Parameters  'HA'   0.115  LJGeometric  0.12552  2.42
EndPairPotentials

