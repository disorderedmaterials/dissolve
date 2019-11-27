/*
        *** Py5 Forcefield
        *** src/data/ff/ludwig/py5.cpp
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

#include "data/ff/ludwig/py5.h"
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
 * Implements "1-pentylpyridinium based on OPLS All Atom Forcefield for benzene and pyridine"
 * W. L. Jorgensen, D. S. Maxwell, and J. Tirado-Rives
 * J. Am. Chem. Soc. 118, 11225-11236 (1996). 
 * W. L. Jorgensen and N. A. McDonald, Theochem 424, 145-155 (1998).
 * W. L. Jorgensen and N. A. McDonald, J. Phys. Chem. B 102, 8049-8059 (1998).
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of JB.
 * All energy values are in kJ/mol.
 */

// Constructor / Destructor
Forcefield_Py5_Ludwig::Forcefield_Py5_Ludwig()
{
	static ForcefieldParameters parameters[]=
	{
		// 	Name   Form (if not default)    Param1(Epsilon)    Param2(Sigma)
		{ this,	"nc",				0.711302,	3.250},
		{ this,	"ca",				0.292919,	3.550},
		{ this,	"ha",				0.125548,	2.420},
		{ this,	"ct",				0.276040,	3.50},
		{ this,	"hc",				0.125548,	2.50},
		{ this,	"cm",				0.276040,	3.50},
		{ this,	"hm",				0.125548,	2.50}
	};
	static ForcefieldAtomType atomTypes[] =
	{
		//	Z		FFID    Name      Type       Description
		//						q      Epsilon   Sigma
		{ this,	ELEMENT_N,	1,	"nc",	"nbonds=3,ring(size=6,C(n=5),N)",			"Nitrogen in pyridine ring",
								0.0749,		"nc" },
		{ this,	ELEMENT_C,	2,	"ca_o",	"nbonds=3,ring(size=6,C(n=5),N),-N,-C,-H",		"Carbon in aromatic ring, ortho",
								0.0725,		"ca"},
		{ this,	ELEMENT_C,	3,	"ca_m",	"nbonds=3,ring(size=6,C(n=5),N),-C,-H,-C",		"Carbon in aromatic ring, meta",
								-0.2245,	"ca"},
		{ this,	ELEMENT_C,	4,	"ca_p",	"nbonds=3,ring(size=6,C(n=5),N),-C(-C(-N),n=2)",	"Carbon in aromatic ring, para",
								0.1706,		"ca"},
		{ this,	ELEMENT_H,	5,	"ha_o",	"nbonds=1,-&2",						"Hydrogen bound to carbon in aromatic ring, ortho",
								0.1751,		"ha"},
		{ this,	ELEMENT_H,	6,	"ha_m",	"nbonds=1,-&3",						"Hydrogen bound to carbon in aromatic ring, meta",
								0.1760,		"ha"},
		{ this,	ELEMENT_H,	7,	"ha_p",	"nbonds=1,-&4" ,					"Hydrogen bound to carbon in aromatic ring, para",
								0.1293,		"ha"},
		{ this,	ELEMENT_C,	8,	"ct_1",	"nbonds=4,nh=2,-C,-&1",					"Alkane Carbon of Tail,	adjacent to N",
								-0.1745,	"ct"},
		{ this,	ELEMENT_C,	9,	"ct_2",	"nbonds=4,nh=2,-C(-N)",					"Alkane Carbon of Tail,	2nd",
								0.1349,		"ct"},
		{ this,	ELEMENT_C,	10,	"ct_3",	"nbonds=4,nh=2,-C(-C(-N))",				"Alkane Carbon of Tail,	3rd",
								0.0170,		"ct"},
		{ this,	ELEMENT_C,	11,	"ct_4",	"nbonds=4,nh=2,-C(-C(-C(-N)))",				"Alkane Carbon of Tail,	4th",
								0.1023,		"ct"},
		{ this,	ELEMENT_H,	12,	"hc_1",	"nbonds=1,-&8",						"Hydrogen of Tail, next to N",
								0.1070,		"hc"},
		{ this,	ELEMENT_H,	13,	"hc_2",	"nbonds=1,-&9" ,					"Hydrogen of Tail, 2nd",
								-0.0208,	"hc"},
		{ this,	ELEMENT_H,	14,	"hc_3",	"nbonds=1,-&10" ,					"Hydrogen of Tail, 3rd",
								0.0097,		"hc"},
		{ this,	ELEMENT_H,	15,	"hc_4",	"nbonds=1,-&11"  ,					"Hydrogen of Tail, 4th",
								-0.0107,	"hc"},
		{ this,	ELEMENT_C,	16,	"cm",	"nbonds=4,nh=3,-&11",					"Carbon of Tail End",
								-0.1671,	"cm"},
		{ this,	ELEMENT_H,	17,	"hm",	"nbonds=1,-&16",					"Hydrogen of Tail End",
								0.0480,		"hm"}
	};
	static ForcefieldBondTerm bondTerms[] =
	{
		//      i       j       Type (Harmonic)			k	eq
		{ this,	"ha",	"ca",	SpeciesBond::HarmonicForm,	3071.,	1.080 },
		{ this,	"ca",	"ca",	SpeciesBond::HarmonicForm,	3925.,	1.400 },
		{ this,	"nc",	"ca",	SpeciesBond::HarmonicForm,	4042.,	1.339 },
		{ this,	"nc",	"ct",	SpeciesBond::HarmonicForm,	4042,	1.339 },
		{ this,	"ct",	"ct",	SpeciesBond::HarmonicForm,	2244.1,	1.529 },
		{ this,	"ct",	"hc",	SpeciesBond::HarmonicForm,	2847.0,	1.09 },
		{ this,	"ct",	"cm",	SpeciesBond::HarmonicForm,	2244.1,	1.529 },
		{ this,	"cm",	"hm",	SpeciesBond::HarmonicForm,	2847.0,	1.09 }
	};
	static ForcefieldAngleTerm angleTerms[] =
	{
		//      i       j       k       Type (Harmonic)			k	eq
		{ this,	"ca",	"ca",	"ca",	SpeciesAngle::HarmonicForm,	527.2,	120.0 },
		{ this,	"ca",	"ca",	"nc",	SpeciesAngle::HarmonicForm,	585.8,	124.0 },
		{ this,	"ca",	"nc",	"ca",	SpeciesAngle::HarmonicForm,	585.8,	117.0 },
		{ this,	"ca",	"ca",	"ha",	SpeciesAngle::HarmonicForm,	292.9,	120.0 },
		{ this,	"nc",	"ca",	"ha",	SpeciesAngle::HarmonicForm,	292.9,	116.0 },
		{ this,	"ca",	"nc",	"ct",	SpeciesAngle::HarmonicForm,	585.8,	121.5 },
		{ this,	"nc",	"ct",	"ct",	SpeciesAngle::HarmonicForm,	487.43,	112.7 },
		{ this,	"hc",	"ct",	"nc",	SpeciesAngle::HarmonicForm,	313.26,	110.7 },
		{ this,	"hc",	"ct",	"hc",	SpeciesAngle::HarmonicForm,	275.7,	107.8 },
		{ this,	"hc",	"ct",	"ct",	SpeciesAngle::HarmonicForm,	313.26,	110.7 },
		{ this,	"hc",	"ct",	"cm",	SpeciesAngle::HarmonicForm,	313.26,	110.7 },
		{ this,	"hm",	"cm",	"ct",	SpeciesAngle::HarmonicForm,	313.26,	110.7 },
		{ this,	"hm",	"cm",	"hm",	SpeciesAngle::HarmonicForm,	275.7,	107.8 },
		{ this,	"ct",	"ct",	"ct",	SpeciesAngle::HarmonicForm,	487.43,	112.7 },
		{ this,	"ct",	"ct",	"cm",	SpeciesAngle::HarmonicForm,	487.43,	112.7 }
	};
	static ForcefieldTorsionTerm torsionTerms[] =
	{
		//      i       j       k      l	Type (CosineForm)		k		n	eq	s
		{ this,	"*",	"ca",	"ca",	"*",	SpeciesTorsion::CosineForm,	15.178,		2.0,	180.0,	1 },
		{ this,	"*",	"ca",	"nc",	"*",	SpeciesTorsion::CosineForm,	15.178,		2.0,	180.0,	1 },
		{ this,	"ca",	"nc",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.4172,	2.0,	0.0,	1 },
		{ this,	"ca",	"nc",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.4759,	4.0,	0.0,	1 },
		{ this,	"nc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-2.9885,	1.0,	0.0,	1 },
		{ this,	"nc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	6.7221,		3.0,	0.0,	1 },
		{ this,	"nc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.3547,		4.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.4882,	1.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.2620,		2.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	5.3908,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.6635,		4.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.3339,		5.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"cm",	SpeciesTorsion::CosineForm,	6.0396,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"cm",	SpeciesTorsion::CosineForm,	0.5463,		4.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"cm",	"hm",	SpeciesTorsion::CosineForm,	1.8773,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"hc",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"cm",	"hm",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"cm",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"nc",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"nc",	"ca",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 }
	};

	static ForcefieldImproperTerm improperTerms[] =
	{
		//      i       j       k      l	Type (CosineForm)		Vn	n	phi
		{ this,	"ca",	"ca",	"ca",	"ha",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 },
		{ this,	"ca",	"nc",	"ca",	"ha",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 },
		{ this,	"ca",	"ca",	"nc",	"ct",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 }
	};
}

Forcefield_Py5_Ludwig::~Forcefield_Py5_Ludwig()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_Py5_Ludwig::name() const
{
	return "1-pentylpyridinium (Py5)";
}

// Return description for Forcefield
const char* Forcefield_Py5_Ludwig::description() const
{
	return "Implements of 1‐pentylpyridinium based on OPLS All Atom Forcefield for benzene and pyridine; W. L. Jorgensen,	D. S. Maxwell,	and J. Tirado-Rives,	<i>Journal of the American Chemical Society</i> <b>118</b> 11225 (1996)";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Py5_Ludwig::shortRangeType() const
{
	return Forcefield::LennardJonesType;
}
