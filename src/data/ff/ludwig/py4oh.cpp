/*
	*** Py4OH cation Forcefield
	*** src/data/ff/ludwig/py4oh.cpp
	Copyright T. Youngs 2019-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation,	either version 3 of the License,	or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.	If not,	see <http://www.gnu.org/licenses/>.
*/

#include "data/ff/ludwig/py4oh.h"
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
 * Implements "1‐(4‐hydroxybutyl)pyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine"
 * W. L. Jorgensen,	D. S. Maxwell,	and J. Tirado-Rives
 * J. Am. Chem. Soc. 118,	11225-11236 (1996).
 * W. L. Jorgensen and N. A. McDonald,	Theochem 424,	145-155 (1998).
 * W. L. Jorgensen and N. A. McDonald,	J. Phys. Chem. B 102,	8049-8059 (1998).
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of JB.
 * All energy values are in kJ/mol.
 */

// Constructor / Destructor
Forcefield_Py4OH_Ludwig::Forcefield_Py4OH_Ludwig()
{
	static ForcefieldParameters parameters[]=
	{
		//	Name	Form (if not default)	Param1(Epsilon)	Param2(Sigma)
		{ this,	"nc",				0.711302,	3.250},
		{ this,	"ca",				0.292919,	3.550},
		{ this,	"ha",				0.125548,	2.420},
		{ this,	"ct",				0.276040,	3.50},
		{ this,	"hc",				0.125548,	2.50},
		{ this,	"oh",				0.711718,	3.12},
		{ this,	"ho",				0.000000,	0.0}
	};
	static ForcefieldAtomType atomTypes[] =
	{
		//		Z		FFID	Name	Type							Description
		//						q	Epsilon	Sigma
		{ this,	ELEMENT_N,	1,	"nc",	"nbonds=3,ring(size=6,C(n=5),N)",			"Nitrogen in pyridine ring",
								0.1014,	"nc" },
		{ this,	ELEMENT_C,	2,	"ca_o",	"nbonds=3,ring(size=6,C(n=5),N),-N,-C,-H",		"Carbon in aromatic ring, ortho",
								0.0568,	"ca" },
		{ this,	ELEMENT_C,	3,	"ca_m",	"nbonds=3,ring(size=6,C(n=5),N),-C,-H,-C" ,		"Carbon in aromatic ring, meta",
								-0.2214,	"ca" },
		{ this,	ELEMENT_C,	4,	"ca_p",	"nbonds=3,ring(size=6,C(n=5),N),-C(n=2,-C(-N))",	"Carbon in aromatic ring, para",
								0.1747,	"ca" },
		{ this,	ELEMENT_H,	5,	"ha_o",	"nbonds=1,-&2",						"Hydrogen bound to carbon in aromatic ring, ortho",
								0.1802,	"ha" },
		{ this,	ELEMENT_H,	6,	"ha_m",	"nbonds=1,-&3",						"Hydrogen bound to carbon in aromatic ring, meta",
								0.1759,	"ha" },
		{ this,	ELEMENT_H,	7,	"ha_p",	"nbonds=1,-&4",						"Hydrogen bound to carbon in aromatic ring, para",
								0.1275,	"ha" },
		{ this,	ELEMENT_C,	8,	"ct_1",	"nbonds=4,nh=2,-&1,-C",					"Alkane Carbon of Tail,	adjacent to N",
								-0.1509,	"ct" },
		{ this,	ELEMENT_C,	9,	"ct_2",	"nbonds=4,nh=2,-&8,-C",					"Alkane Carbon of Tail,nextnext=N",
								0.0160,	"ct" },
		{ this,	ELEMENT_C,	10,	"ct_3",	"nbonds=4,nh=2,-C(-H(n=2),-O)",				"Alkane Carbon of Tail,nextnext=O",
								0.0780,	"ct" },
		{ this,	ELEMENT_C,	11,	"ct_4",	"nbonds=4,nh=2,-C(-H(n=2),-C),-O",			"Alkane Carbon of Tail,	adjacent to O",
								0.2911,	"ct" },
		{ this,	ELEMENT_H,	12,	"hc_1",	"nbonds=1,-&8",						"Hydrogen of Tail, adjacent to N",
								0.1015,	"hc" },
		{ this,	ELEMENT_H,	13,	"hc_2",	"nbonds=1,-&9",						"Hydrogen of Tail, nextnext=N",
								0.0043,	"hc" },
		{ this,	ELEMENT_H,	14,	"hc_3",	"nbonds=1,-&10",					"Hydrogen of Tail, nextnext=O",
								0.0316,	"hc" },
		{ this,	ELEMENT_H,	15,	"hc_4",	"nbonds=1,-&11",					"Hydrogen of Tail, adjacent to O",
								-0.0205,	"hc" },
		{ this,	ELEMENT_O,	16,	"oh",	"nbonds=2,-H,-&11",					"Oxygen of Hydroxyl Group",
								-0.6916,	"oh" },
		{ this,	ELEMENT_H,	17,	"ho",	"nbonds=1,-&16",					"Hydrogen of Hydroxyl Group",
								0.4370,	"ho" }
	};

	static ForcefieldBondTerm bondTerms[] =
	{
		//	i	j	Type (Harmonic)			k	eq
		{ this,	"ha",	"ca",	SpeciesBond::HarmonicForm,	3071.,	1.080 },
		{ this,	"ca",	"ca",	SpeciesBond::HarmonicForm,	3925.,	1.400 },
		{ this,	"nc",	"ca",	SpeciesBond::HarmonicForm,	4042.,	1.339 },
		{ this,	"nc",	"ct",	SpeciesBond::HarmonicForm,	4042,	1.339 },
		{ this,	"ct",	"ct",	SpeciesBond::HarmonicForm,	2244.1,	1.529 },
		{ this,	"ct",	"hc",	SpeciesBond::HarmonicForm,	2847.0,	1.09 },
		{ this,	"ct",	"oh",	SpeciesBond::HarmonicForm,	2679.6,	1.41 },
		{ this,	"ho",	"oh",	SpeciesBond::HarmonicForm,	4630.6,	0.945 }
	};
	static ForcefieldAngleTerm angleTerms[] =
	{
		//	i	j	k	Type (Harmonic)			k	eq
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
		{ this,	"ct",	"ct",	"ct",	SpeciesAngle::HarmonicForm,	487.43,	112.7 },
		{ this,	"ho",	"oh",	"ct",	SpeciesAngle::HarmonicForm,	460.55,	108.5 },
		{ this,	"ct",	"ct",	"oh",	SpeciesAngle::HarmonicForm,	418.68,	109.5 },
		{ this,	"hc",	"ct",	"oh",	SpeciesAngle::HarmonicForm,	293.08,	109.5 }
	};
	static ForcefieldTorsionTerm torsionTerms[] =
	{
		//	i	j	k	l	Type (CosineForm)		k		n	eq	s
		{ this,	"*",	"ca",	"ca",	"*",	SpeciesTorsion::CosineForm,	15.178,		2.0,	180.0,	1 },
		{ this,	"*",	"ca",	"nc",	"*",	SpeciesTorsion::CosineForm,	15.178,		2.0,	180.0,	1 },
		{ this,	"ca",	"nc",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.3579,	2.0,	0.0,	1 },
		{ this,	"ca",	"nc",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.4037,	4.0,	0.0,	1 },
		{ this,	"nc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-3.7014,	1.0,	0.0,	1 },
		{ this,	"nc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	6.9923,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	-0.2825,	1.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.6065,		2.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	4.6858,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.7018,		4.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.4468,		5.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.4564,		6.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"oh",	SpeciesTorsion::CosineForm,	-2.3748,	1.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"oh",	SpeciesTorsion::CosineForm,	6.8089,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"ct",	"oh",	SpeciesTorsion::CosineForm,	0.9531,		4.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"oh",	"ho",	SpeciesTorsion::CosineForm,	-3.5552,	1.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"oh",	"ho",	SpeciesTorsion::CosineForm,	0.5886,		2.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"oh",	"ho",	SpeciesTorsion::CosineForm,	2.5272,		3.0,	0.0,	1 },
		{ this,	"ct",	"ct",	"oh",	"ho",	SpeciesTorsion::CosineForm,	0.1504,		4.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"hc",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"ct",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"nc",	"ca",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"nc",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"ct",	"oh",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 },
		{ this,	"hc",	"ct",	"oh",	"ho",	SpeciesTorsion::CosineForm,	0.0000,		3.0,	0.0,	1 }
	};

	static ForcefieldImproperTerm improperTerms[] =
	{
		//	i	j	k	l	Type (CosineForm)		Vn	n	phi
		{ this,	"ca",	"ca",	"ca",	"ha",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 },
		{ this,	"ca",	"nc",	"ca",	"ha",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 },
		{ this,	"ca",	"ca",	"nc",	"ct",	SpeciesImproper::CosineForm,	4.606,	2.0,	180.0 }
	};

}

Forcefield_Py4OH_Ludwig::~Forcefield_Py4OH_Ludwig()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_Py4OH_Ludwig::name() const
{
	return "1‐(4‐hydroxybutyl)pyridinium cation (Py4OH)";
}

// Return description for Forcefield
const char* Forcefield_Py4OH_Ludwig::description() const
{
	return "Implements of 1‐(4‐hydroxybutyl)pyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine; W. L. Jorgensen,	D. S. Maxwell, and J. Tirado-Rives, <i>Journal of the American Chemical Society</i>, <b>118</b>, 11225 (1996).";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Py4OH_Ludwig::shortRangeType() const
{
	return Forcefield::LennardJonesType;
}
