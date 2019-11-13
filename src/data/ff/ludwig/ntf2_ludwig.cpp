/*
        *** NTf2 Forcefield by Ludwig group
        *** src/data/ff/ludwig/ntf2_ludwig.cpp
        Copyright T. Youngs 2019

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

#include "data/ff/ludwig/ntf2_ludwig.h"
#include "data/ffangleterm.h"
#include "data/ffatomtype.h"
#include "data/ffbondterm.h"
#include "data/ffparameters.h"
#include "data/fftorsionterm.h"
#include "data/ffimproperterm.h"
#include "classes/atomtype.h"
#include "classes/speciesatom.h"
#include "base/sysfunc.h"

/*
 * Implements "Revisiting imidazolium based ionic liquids: Effect of the conformation bias of the [NTf2] anion studied by molecular dynamics simulations"
 * J. Neumann, B. Golub, LM. Odebrecht, R. Ludwig, D. Paschek
 * Journal of Chemical Physics 148 193828 (2018)
 * http://doi.org/10.1063/1.5013096
 * 
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are in kJ/mol.
 */

// Constructor / Destructor
Forcefield_NTf2_Ludwig::Forcefield_NTf2_Ludwig()
{
	static ForcefieldParameters parameters[] =
	{
		 //    Name    Form (if not default)    Param1(Epsilon)    Param2(Sigma)
		{ this, "F",               0.066516   2.655},
		{ this, "C",               0.082812   3.150},
		{ this, "S",               0.313705   4.0825},
		{ this, "O",               0.263568   3.4632},
		{ this, "N",               0.213349   3.2500}

	};
        static ForcefieldAtomType atomTypes[] =
        {
                //      Z     El          FFID    Name      Type	       Description
                //                                                q      Epsilon   Sigma
                { this,      ELEMENT_F,   1,      "F",       "-C(-F(n=2))"                "Fluorine",
                                                                -0.189,		"F" },
                { this,      ELEMENT_C,   2,      "C",       "-F(n=3),-S(-O(n=2))"                "Carbon",
                                                                0.494,   	"C"},
                { this,      ELEMENT_S,   3,      "S",       "-O(n=2),-C,-N"                "Sulfur",
                                                                1.076,   	"S" },
                { this,      ELEMENT_O,   4,      "O",       "-S(-O(n=2),-C,-N)(n=2)"                "Oxygen",
                                                                -0.579,   	"O" },
                { this,      ELEMENT_N,   5,      "N",       "-S(-O(n=2),-C)(n=2)"                "Nitrogen",
                                                                -0.69,   	"N" }
        };

	        static ForcefieldBondTerm bondTerms[] =
        {
                //      i       j       Type (Harmonic)                 k               eq
                { this, "C",   "F",   SpeciesBond::HarmonicForm,      3697.0,        1.323 },
                { this, "C",   "S",   SpeciesBond::HarmonicForm,      1979.0,        1.818 },
                { this, "S",   "O",   SpeciesBond::HarmonicForm,      5331.0,        1.442 },
                { this, "N",   "S",   SpeciesBond::HarmonicForm,      3113.0,        1.570 }
        };

        static ForcefieldAngleTerm angleTerms[] =
        {
                //      i       j       k       Type (Harmonic)                 k            eq
                { this, "F",   "C",   "F",   SpeciesAngle::HarmonicForm,     781.0,       107.1 },
                { this, "S",   "C",   "F",   SpeciesAngle::HarmonicForm,     694.0,       111.8 },
                { this, "C",   "S",   "O",   SpeciesAngle::HarmonicForm,     870.0,       102.6 },
                { this, "O",   "S",   "O",   SpeciesAngle::HarmonicForm,     969.0,       118.5 },
                { this, "O",   "S",   "N",   SpeciesAngle::HarmonicForm,     789.0,       113.6 },
                { this, "C",   "S",   "N",   SpeciesAngle::HarmonicForm,     816.0,       100.2 },
                { this, "S",   "N",   "S",   SpeciesAngle::HarmonicForm,     671.0,       125.6 }
        };
        static ForcefieldTorsionTerm torsionTerms[] =
        {
                //      i       j       k      l        Type (CosineForm)              k           n          eq        s
                { this, "F",   "C",   "S",   "N",   SpeciesTorsion::CosineForm,      2.0401,       3.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,     23.7647,       1.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,      6.2081,       2.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,     -2.3684,       3.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,     -0.0298,       4.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,      0.6905,       5.0,       0.0,       1 },
                { this, "S",   "N",   "S",   "C",   SpeciesTorsion::CosineForm,      1.0165,       6.0,       0.0,       1 }
        };

}

Forcefield_NTf2_Ludwig::~Forcefield_NTf2_Ludwig()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_NTf2_Ludwig::name() const
{
        return "bis(trifluoromethyl-sulfonyl)imide anion (NTf2) by Ludwig Group";
}

// Return description for Forcefield
const char* Forcefield_NTf2::description() const
{
        return "J. Neumann, B. Golub, L.-M. Odebrecht, R. Ludwig, D. Paschek , bis(trifluoromethyl-sulfonyl)imide anion by Ludwig Group,<em>J. Chem. Phys.</em> <b>148</b>, 193828 (2018).";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_NTf2_Ludwig::shortRangeType() const
{
        return Forcefield::LennardJonesType;
}

/*
 * Atom Type Data
 */

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield_NTf2::determineAtomType(SpeciesAtom* i) const
{
        return NULL;
}
