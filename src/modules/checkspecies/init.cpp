/*
    *** CheckSpecies Module - Initialisation
    *** src/modules/checkspecies/init.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/types.h"
#include "modules/checkspecies/checkspecies.h"

// Perform any necessary initialisation for the Module
void CheckSpeciesModule::initialise()
{
    // Target Species
    keywords_.add("Target", new SpeciesKeyword(), "Species", "Target species to check");

    // Checks
    keywords_.add("Atoms", new IntegerStringVectorKeyword(atomTypes_, 1, 1), "AtomType",
                  "Check that atom index <id> has the atom type <type>", "<id> <type>");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(bondParameters_, 2), "Bond",
                  "Check that the bond <i>-<j> has the correct parameters", "<i> <j> <p1> [p2...]");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(angleParameters_, 3), "Angle",
                  "Check that the angle <i>-<j>-<k> has the correct parameters", "<i> <j> <k> <p1> [p2...]");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(torsionParameters_, 4), "Torsion",
                  "Check that the torsion <i>-<j>-<k>-<l> has the correct parameters", "<i> <j> <k> <l> <p1> [p2...]");
    keywords_.add("Intramolecular", new IntegerDoubleVectorKeyword(improperParameters_, 4), "Improper",
                  "Check that the improper <i>-<j>-<k>-<l> has the correct parameters", "<i> <j> <k> <l> <p1> [p2...]");
    keywords_.add("Intramolecular", new DoubleKeyword(1.0e-3), "Tolerance",
                  "Tolerance beyond which parameters are said to differ");
}
