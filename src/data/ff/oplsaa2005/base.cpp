/*
	*** OPLSAA (2005) Base Forcefield
	*** src/data/ff/oplsaa2005/base.cpp
	Copyright T. Youngs 2019-2020

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

#include "data/ff/oplsaa2005/base.h"
#include "data/ffangleterm.h"
#include "data/ffbondterm.h"
#include "data/fftorsionterm.h"

/*
 * OPLS-AA (2005) Base (Intramolecular Terms)
 */

// Constructor
OPLSAA2005BaseForcefield::OPLSAA2005BaseForcefield()
{
	static ForcefieldBondTerm bondTerms[] =
	{
		//	i	j	Type (Harmonic)			k	eq
		{ this, "OW",	"HW",	SpeciesBond::HarmonicForm,	5020.8,		0.9572 },
		/* { this, "OW",	"LP",	SpeciesBond::HarmonicForm,	7531.2,	0.175 }, */
		{ this, "C*",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	1.08 },
		{ this, "C",	"C2",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		{ this, "C",	"C3",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		{ this, "C",	"CA",	SpeciesBond::HarmonicForm,	3347.2,		1.49 },
		{ this, "C",	"CB",	SpeciesBond::HarmonicForm,	3740.5,		1.419 },
		{ this, "C",	"CD",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "C",	"CH",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		/* { this, "C",	"CJ",	SpeciesBond::HarmonicForm,	3430.88,	1.444 }, */
		{ this, "C",	"CM",	SpeciesBond::HarmonicForm,	3430.88,	1.444 },
		{ this, "C",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		{ this, "C$",	"CY",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		{ this, "C$",	"N$",	SpeciesBond::HarmonicForm,	4100.32,	1.335 },
		{ this, "C",	"N",	SpeciesBond::HarmonicForm,	4100.32,	1.335 },
		{ this, "C",	"NM",	SpeciesBond::HarmonicForm,	4100.32,	1.335 },
		{ this, "C",	"N*",	SpeciesBond::HarmonicForm,	3548.03,	1.383 },
		{ this, "C",	"NA",	SpeciesBond::HarmonicForm,	3497.82,	1.388 },
		{ this, "C",	"NC",	SpeciesBond::HarmonicForm,	3824.18,	1.358 },
		/* { this, "C",	"Neq",	SpeciesBond::HarmonicForm,	3824.18,	1.29 }, */
		{ this, "C",	"O",	SpeciesBond::HarmonicForm,	4769.76,	1.229 },
		{ this, "C$",	"O",	SpeciesBond::HarmonicForm,	4769.76,	1.229 },
		{ this, "C",	"O2",	SpeciesBond::HarmonicForm,	5489.41,	1.25 },
		{ this, "C",	"OH",	SpeciesBond::HarmonicForm,	3765.6,		1.364 },
		/* { this, "C",	"Seq",	SpeciesBond::HarmonicForm,	3347.2,		1.64 }, */
		{ this, "NO",	"ON",	SpeciesBond::HarmonicForm,	4602.4,		1.225 },
		{ this, "CT",	"NO",	SpeciesBond::HarmonicForm,	3138,		1.49 },
		{ this, "CA",	"NO",	SpeciesBond::HarmonicForm,	3347.2,		1.46 },
		{ this, "CA",	"OH",	SpeciesBond::HarmonicForm,	3765.6,		1.364 },
		{ this, "CA",	"OS",	SpeciesBond::HarmonicForm,	3765.6,		1.364 },
		{ this, "CB",	"OS",	SpeciesBond::HarmonicForm,	2845.12,	1.36 },
		{ this, "CM",	"OS",	SpeciesBond::HarmonicForm,	3765.6,		1.37 },
		{ this, "CM",	"OH",	SpeciesBond::HarmonicForm,	3765.6,		1.37 },
		{ this, "C",	"OS",	SpeciesBond::HarmonicForm,	1790.75,	1.327 },
		{ this, "C*",	"C2",	SpeciesBond::HarmonicForm,	2652.66,	1.495 },
		{ this, "C*",	"CB",	SpeciesBond::HarmonicForm,	3246.78,	1.459 },
		/* { this, "C*",	"CG",	SpeciesBond::HarmonicForm,	4568.93,	1.352 }, */
		/* { this, "C*",	"CC",	SpeciesBond::HarmonicForm,	4568.93,	1.352 }, */
		{ this, "C*",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.495 },
		{ this, "CS",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.495 },
		{ this, "C*",	"CW",	SpeciesBond::HarmonicForm,	4568.93,	1.352 },
		{ this, "CA",	"CW",	SpeciesBond::HarmonicForm,	4568.93,	1.367 },
		{ this, "CS",	"CW",	SpeciesBond::HarmonicForm,	4568.93,	1.367 },
		{ this, "CS",	"CS",	SpeciesBond::HarmonicForm,	3924.59,	1.424 },
		{ this, "CS",	"CB",	SpeciesBond::HarmonicForm,	3924.59,	1.424 },
		{ this, "CS",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CU",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.32 },
		{ this, "CU",	"CA",	SpeciesBond::HarmonicForm,	3924.59,	1.421 },
		{ this, "CU",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "NA",	"NB",	SpeciesBond::HarmonicForm,	3347.2,		1.349 },
		{ this, "NB",	"NB",	SpeciesBond::HarmonicForm,	3347.2,		1.28 },
		{ this, "OS",	"NB",	SpeciesBond::HarmonicForm,	3866.02,	1.399 },
		{ this, "OS",	"CR",	SpeciesBond::HarmonicForm,	3866.02,	1.357 },
		{ this, "C2",	"C2",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "C2",	"C3",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "C3",	"C3",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "C2",	"CA",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "C2",	"CD",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		/* { this, "C2",	"CC",	SpeciesBond::HarmonicForm,	2652.66,	1.504 }, */
		{ this, "C2",	"CH",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "C2",	"N",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "C2",	"N2",	SpeciesBond::HarmonicForm,	2820.02,	1.463 },
		{ this, "C2",	"N3",	SpeciesBond::HarmonicForm,	3071.06,	1.471 },
		{ this, "CH",	"NT",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "C2",	"NT",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "C3",	"NT",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "CT",	"NT",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "NT",	"NT",	SpeciesBond::HarmonicForm,	2928.8,		1.445 },
		{ this, "C2",	"OH",	SpeciesBond::HarmonicForm,	3230.05,	1.425 },
		{ this, "C2",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.425 },
		{ this, "CO",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.38 },
		{ this, "CO",	"C2",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "CO",	"C3",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "CW",	"OS",	SpeciesBond::HarmonicForm,	2845.12,	1.36 },
		{ this, "C2",	"S",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "C2",	"SH",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "C3",	"CH",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "C3",	"CM",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "C3",	"N",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "C3",	"N*",	SpeciesBond::HarmonicForm,	2820.02,	1.475 },
		{ this, "C3",	"N2",	SpeciesBond::HarmonicForm,	2820.02,	1.463 },
		{ this, "C3",	"N3",	SpeciesBond::HarmonicForm,	3071.06,	1.471 },
		{ this, "C3",	"OH",	SpeciesBond::HarmonicForm,	3230.05,	1.425 },
		{ this, "C3",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.425 },
		{ this, "C3",	"S",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "C3",	"SH",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "CA",	"CA",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "CA",	"C!",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "C!",	"C!",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CS",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CU",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CV",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CW",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CR",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"C",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"CM",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "C!",	"NA",	SpeciesBond::HarmonicForm,	3221.68,	1.44 },
		{ this, "CA",	"CB",	SpeciesBond::HarmonicForm,	3924.59,	1.404 },
		{ this, "CA",	"CD",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		/* { this, "CA",	"CJ",	SpeciesBond::HarmonicForm,	3573.14,	1.433 }, */
		{ this, "CA",	"CM",	SpeciesBond::HarmonicForm,	3573.14,	1.433 },
		{ this, "CA",	"CN",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "CA",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "CA",	"CY",	SpeciesBond::HarmonicForm,	2652.66,	1.49 },
		{ this, "CW",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.504 },
		{ this, "CV",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.504 },
		{ this, "CX",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.504 },
		{ this, "CX",	"CX",	SpeciesBond::HarmonicForm,	4351.36,	1.37 },
		{ this, "CX",	"NA",	SpeciesBond::HarmonicForm,	3573.14,	1.381 },
		{ this, "CX",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CA",	"NY",	SpeciesBond::HarmonicForm,	3196.58,	1.385 },
		{ this, "CA",	"NZ",	SpeciesBond::HarmonicForm,	4.184,		1.261 },
		{ this, "NY",	"H",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "NZ",	"H",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "CT",	"NY",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "CA",	"N2",	SpeciesBond::HarmonicForm,	4025.01,	1.34 },
		{ this, "CQ",	"N2",	SpeciesBond::HarmonicForm,	4025.01,	1.34 },
		{ this, "CR",	"N2",	SpeciesBond::HarmonicForm,	4025.01,	1.34 },
		{ this, "CA",	"NT",	SpeciesBond::HarmonicForm,	4025.01,	1.34 },
		{ this, "CA",	"NA",	SpeciesBond::HarmonicForm,	3573.14,	1.381 },
		{ this, "CQ",	"N",	SpeciesBond::HarmonicForm,	3573.14,	1.381 },
		{ this, "CA",	"NC",	SpeciesBond::HarmonicForm,	4041.74,	1.339 },
		{ this, "C!",	"NC",	SpeciesBond::HarmonicForm,	4041.74,	1.339 },
		{ this, "NC",	"NC",	SpeciesBond::HarmonicForm,	4.184,		1.32 },
		{ this, "NC",	"NZ",	SpeciesBond::HarmonicForm,	4602.4,		1.24 },
		{ this, "NZ",	"NZ",	SpeciesBond::HarmonicForm,	4602.4,		1.12 },
		{ this, "CT",	"NZ",	SpeciesBond::HarmonicForm,	3263.52,	1.43 },
		{ this, "CA",	"NZ",	SpeciesBond::HarmonicForm,	3347.2,		1.41 },
		{ this, "CA",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CB",	"CB",	SpeciesBond::HarmonicForm,	4351.36,	1.37 },
		{ this, "CR",	"CS",	SpeciesBond::HarmonicForm,	4351.36,	1.37 },
		{ this, "CV",	"CW",	SpeciesBond::HarmonicForm,	4351.36,	1.37 },
		{ this, "CU",	"CW",	SpeciesBond::HarmonicForm,	4351.36,	1.37 },
		{ this, "CB",	"CD",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "CB",	"CN",	SpeciesBond::HarmonicForm,	3740.5,	1.419 },
		{ this, "CB",	"N*",	SpeciesBond::HarmonicForm,	3648.45,	1.374 },
		{ this, "CB",	"NA",	SpeciesBond::HarmonicForm,	3648.45,	1.374 },
		{ this, "CB",	"NB",	SpeciesBond::HarmonicForm,	3464.35,	1.391 },
		{ this, "CB",	"NC",	SpeciesBond::HarmonicForm,	3857.65,	1.354 },
		{ this, "CR",	"NC",	SpeciesBond::HarmonicForm,	3857.65,	1.354 },
		/* { this, "CC",	"CF",	SpeciesBond::HarmonicForm,	4284.42,	1.375 }, */
		/* { this, "CC",	"CG",	SpeciesBond::HarmonicForm,	4334.62,	1.371 }, */
		/* { this, "CC",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.504 }, */
		/* { this, "CC",	"CV",	SpeciesBond::HarmonicForm,	4284.42,	1.375 }, */
		{ this, "CW",	"CW",	SpeciesBond::HarmonicForm,	4284.42,	1.375 },
		/* { this, "CC",	"CW",	SpeciesBond::HarmonicForm,	4334.62,	1.371 }, */
		/* { this, "CC",	"NA",	SpeciesBond::HarmonicForm,	3531.3,	1.385 }, */
		/* { this, "CC",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 }, */
		{ this, "CD",	"CD",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		{ this, "CD",	"CN",	SpeciesBond::HarmonicForm,	3924.59,	1.4 },
		/* { this, "CA",	"CC",	SpeciesBond::HarmonicForm,	3924.59,	1.4 }, */
		/* { this, "CD",	"CC",	SpeciesBond::HarmonicForm,	3924.59,	1.4 }, */
		/* { this, "CE",	"N*",	SpeciesBond::HarmonicForm,	3681.92,	1.371 }, */
		/* { this, "CE",	"NB",	SpeciesBond::HarmonicForm,	4426.67,	1.304 }, */
		/* { this, "CF",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 }, */
		/* { this, "CG",	"NA",	SpeciesBond::HarmonicForm,	3573.14,	1.381 }, */
		{ this, "CH",	"CH",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "CH",	"N",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CH",	"N*",	SpeciesBond::HarmonicForm,	2820.02,	1.475 },
		{ this, "CH",	"OH",	SpeciesBond::HarmonicForm,	3230.05,	1.425 },
		{ this, "CH",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.425 },
		/* { this, "CI",	"NC",	SpeciesBond::HarmonicForm,	4200.74,	1.324 }, */
		/* { this, "CJ",	"CJ",	SpeciesBond::HarmonicForm,	4594.03,	1.35 }, */
		/* { this, "CJ",	"CM",	SpeciesBond::HarmonicForm,	4594.03,	1.35 }, */
		/* { this, "CJ",	"N*",	SpeciesBond::HarmonicForm,	3748.86,	1.365 }, */
		{ this, "CK",	"HA",	SpeciesBond::HarmonicForm,	2845.12,	1.08 },
		{ this, "CK",	"H5",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CK",	"N*",	SpeciesBond::HarmonicForm,	3681.92,	1.371 },
		{ this, "CK",	"NA",	SpeciesBond::HarmonicForm,	3681.92,	1.371 },
		{ this, "CK",	"NB",	SpeciesBond::HarmonicForm,	4426.67,	1.304 },
		{ this, "CM",	"CM",	SpeciesBond::HarmonicForm,	4594.03,	1.34 },
		{ this, "CM",	"C=",	SpeciesBond::HarmonicForm,	4594.03,	1.34 },
		{ this, "CW",	"C=",	SpeciesBond::HarmonicForm,	4594.03,	1.365 },
		{ this, "C=",	"C=",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		/* { this, "C=",	"Neq",	SpeciesBond::HarmonicForm,	3472.72,	1.428 }, */
		{ this, "C",	"C",	SpeciesBond::HarmonicForm,	2928.8,		1.51 },
		{ this, "C=",	"C",	SpeciesBond::HarmonicForm,	3221.68,	1.46 },
		{ this, "CT",	"C+",	SpeciesBond::HarmonicForm,	4458.47,	1.46 },
		{ this, "C+",	"HC",	SpeciesBond::HarmonicForm,	4458.47,	1.084 },
		{ this, "CM",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "C=",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "CM",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	1.08 },
		{ this, "CM",	"H4",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "C=",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	1.08 },
		{ this, "HC",	"C",	SpeciesBond::HarmonicForm,	2845.12,	1.09 },
		{ this, "CT",	"CZ",	SpeciesBond::HarmonicForm,	3263.52,	1.47 },
		{ this, "CA",	"CZ",	SpeciesBond::HarmonicForm,	3347.2,		1.451 },
		{ this, "CY",	"CZ",	SpeciesBond::HarmonicForm,	3347.2,		1.451 },
		{ this, "CZ",	"NZ",	SpeciesBond::HarmonicForm,	5439.2,		1.157 },
		{ this, "CZ",	"CZ",	SpeciesBond::HarmonicForm,	9623.2,		1.21 },
		{ this, "HC",	"CZ",	SpeciesBond::HarmonicForm,	3514.56,	1.08 },
		{ this, "CM",	"N*",	SpeciesBond::HarmonicForm,	3748.86,	1.365 },
		{ this, "CM",	"NA",	SpeciesBond::HarmonicForm,	3748.86,	1.365 },
		{ this, "CN",	"NA",	SpeciesBond::HarmonicForm,	3581.5,	1.38 },
		/* { this, "CP",	"NA",	SpeciesBond::HarmonicForm,	3991.54,	1.343 }, */
		/* { this, "CP",	"NB",	SpeciesBond::HarmonicForm,	4083.58,	1.335 }, */
		{ this, "CQ",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CQ",	"H5",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CQ",	"NC",	SpeciesBond::HarmonicForm,	4200.74,	1.324 },
		{ this, "CR",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CR",	"H5",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CR",	"NA",	SpeciesBond::HarmonicForm,	3991.54,	1.343 },
		{ this, "CR",	"NB",	SpeciesBond::HarmonicForm,	4083.58,	1.335 },
		{ this, "CT",	"CT",	SpeciesBond::HarmonicForm,	2242.62,	1.529 },
		{ this, "CT",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	1.09 },
		{ this, "DM",	"Cl",	SpeciesBond::HarmonicForm,	2510.4,		0.3 },
		{ this, "DM",	"Br",	SpeciesBond::HarmonicForm,	2510.4,		0.3 },
		{ this, "DM",	"F",	SpeciesBond::HarmonicForm,	2510.4,		0.3 },
		{ this, "DM",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "DM",	"HO",	SpeciesBond::HarmonicForm,	2845.12,	0.1 },
		{ this, "DM",	"HS",	SpeciesBond::HarmonicForm,	2845.12,	0.1 },
		{ this, "DM",	"HA",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "DM",	"CA",	SpeciesBond::HarmonicForm,	3071.06,	0.3 },
		{ this, "DM",	"NC",	SpeciesBond::HarmonicForm,	3071.06,	0.3 },
		{ this, "DM",	"NB",	SpeciesBond::HarmonicForm,	3071.06,	0.3 },
		{ this, "DM",	"CT",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "DM",	"SZ",	SpeciesBond::HarmonicForm,	2845.12,	0.5 },
		{ this, "DM",	"S",	SpeciesBond::HarmonicForm,	2845.12,	0.5 },
		{ this, "DM",	"OS",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "DM",	"OY",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "DM",	"OH",	SpeciesBond::HarmonicForm,	2845.12,	0.3 },
		{ this, "CT",	"N",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CT",	"NM",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CT",	"NC",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CY",	"N$",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CT",	"N$",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CY",	"N",	SpeciesBond::HarmonicForm,	2820.02,	1.449 },
		{ this, "CT",	"N*",	SpeciesBond::HarmonicForm,	2820.02,	1.475 },
		{ this, "CO",	"N*",	SpeciesBond::HarmonicForm,	2820.02,	1.475 },
		{ this, "CT",	"NA",	SpeciesBond::HarmonicForm,	2820.02,	1.475 },
		{ this, "CT",	"N2",	SpeciesBond::HarmonicForm,	2820.02,	1.463 },
		{ this, "CT",	"N3",	SpeciesBond::HarmonicForm,	3071.06,	1.471 },
		{ this, "CA",	"N3",	SpeciesBond::HarmonicForm,	3347.2,		1.45 },
		{ this, "CT",	"OH",	SpeciesBond::HarmonicForm,	2677.76,	1.41 },
		{ this, "NT",	"OH",	SpeciesBond::HarmonicForm,	2677.76,	1.45 },
		{ this, "NT",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.45 },
		{ this, "N",	"OH",	SpeciesBond::HarmonicForm,	3347.2,		1.38 },
		{ this, "CT",	"OS",	SpeciesBond::HarmonicForm,	2677.76,	1.41 },
		{ this, "OS",	"OS",	SpeciesBond::HarmonicForm,	2092,		1.47 },
		{ this, "OS",	"OH",	SpeciesBond::HarmonicForm,	2092,		1.47 },
		{ this, "OS",	"Cl",	SpeciesBond::HarmonicForm,	1673.6,		1.69 },
		/* ####  0.0  0 */
		{ this, "Si",	"CT",	SpeciesBond::HarmonicForm,	1564.82,	1.86 },
		{ this, "Si",	"Si",	SpeciesBond::HarmonicForm,	786.592,	2.32 },
		{ this, "Si",	"HC",	SpeciesBond::HarmonicForm,	1389.09,	1.48 },
		{ this, "Si",	"H",	SpeciesBond::HarmonicForm,	1389.09,	1.48 },
		{ this, "Si",	"F",	SpeciesBond::HarmonicForm,	3857.65,	1.57 },
		{ this, "Si",	"Cl",	SpeciesBond::HarmonicForm,	1866.06,	2.02 },
		{ this, "Si",	"Br",	SpeciesBond::HarmonicForm,	1263.57,	2.19 },
		{ this, "Si",	"I",	SpeciesBond::HarmonicForm,	903.744,	2.44 },
		{ this, "Si",	"OH",	SpeciesBond::HarmonicForm,	3129.63,	1.64 },
		{ this, "Si",	"OS",	SpeciesBond::HarmonicForm,	3129.63,	1.64 },
		{ this, "Si",	"P",	SpeciesBond::HarmonicForm,	903.744,	2.25 },
		{ this, "Si",	"NT",	SpeciesBond::HarmonicForm,	2225.89,	1.74 },
		{ this, "Si",	"S",	SpeciesBond::HarmonicForm,	1204.99,	2.15 },
		/* ####  0.0  0 */
		{ this, "CT",	"S",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "CY",	"S",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "CR",	"S",	SpeciesBond::HarmonicForm,	2092,		1.76 },
		{ this, "CW",	"S",	SpeciesBond::HarmonicForm,	2092,		1.74 },
		{ this, "CA",	"SH",	SpeciesBond::HarmonicForm,	2092,		1.74 },
		{ this, "CT",	"SH",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "CT",	"Cl",	SpeciesBond::HarmonicForm,	2050.16,	1.781 },
		{ this, "CA",	"Cl",	SpeciesBond::HarmonicForm,	2510.4,		1.725 },
		{ this, "CM",	"Cl",	SpeciesBond::HarmonicForm,	2510.4,		1.725 },
		{ this, "C",	"Cl",	SpeciesBond::HarmonicForm,	2510.4,		1.79 },
		{ this, "CZ",	"Cl",	SpeciesBond::HarmonicForm,	2761.44,	1.637 },
		{ this, "CT",	"Br",	SpeciesBond::HarmonicForm,	2050.16,	1.945 },
		{ this, "CA",	"Br",	SpeciesBond::HarmonicForm,	2510.4,		1.87 },
		{ this, "CM",	"Br",	SpeciesBond::HarmonicForm,	2510.4,		1.9 },
		{ this, "C",	"Br",	SpeciesBond::HarmonicForm,	2510.4,		1.98 },
		{ this, "CZ",	"Br",	SpeciesBond::HarmonicForm,	2761.44,	1.784 },
		{ this, "CA",	"I",	SpeciesBond::HarmonicForm,	2092,		2.08 },
		{ this, "CT",	"I",	SpeciesBond::HarmonicForm,	1673.6,		2.19 },
		{ this, "CV",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CV",	"H4",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CV",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 },
		{ this, "CW",	"NB",	SpeciesBond::HarmonicForm,	3430.88,	1.394 },
		{ this, "CW",	"H4",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CW",	"HA",	SpeciesBond::HarmonicForm,	3071.06,	1.08 },
		{ this, "CW",	"NA",	SpeciesBond::HarmonicForm,	3573.14,	1.381 },
		{ this, "CY",	"CY",	SpeciesBond::HarmonicForm,	2175.68,	1.52 },
		{ this, "CY",	"CT",	SpeciesBond::HarmonicForm,	2343.04,	1.51 },
		{ this, "CY",	"HC",	SpeciesBond::HarmonicForm,	2845.12,	1.088 },
		{ this, "H",	"N",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H",	"N3",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H",	"N*",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H",	"N2",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H",	"NA",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		/* { this, "H2",	"N",	SpeciesBond::HarmonicForm,	3631.71,	1.01 }, */
		/* { this, "H2",	"N2",	SpeciesBond::HarmonicForm,	3631.71,	1.01 }, */
		{ this, "H",	"NT",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H3",	"N2",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "H3",	"N3",	SpeciesBond::HarmonicForm,	3631.71,	1.01 },
		{ this, "HO",	"OH",	SpeciesBond::HarmonicForm,	4627.5,		0.945 },
		/* { this, """"""",	"460.5",	SpeciesBond::HarmonicForm,	8.03328,	0.0 }, */
		{ this, "HO",	"OS",	SpeciesBond::HarmonicForm,	4627.5,		0.945 },
		{ this, "HS",	"SH",	SpeciesBond::HarmonicForm,	2292.83,	1.336 },
		{ this, "O2",	"P",	SpeciesBond::HarmonicForm,	4393.2,		1.48 },
		{ this, "O",	"P",	SpeciesBond::HarmonicForm,	4393.2,		1.48 },
		{ this, "OH",	"P",	SpeciesBond::HarmonicForm,	1924.64,	1.61 },
		{ this, "OS",	"P",	SpeciesBond::HarmonicForm,	1924.64,	1.61 },
		{ this, "CT",	"P",	SpeciesBond::HarmonicForm,	1774.02,	1.843 },
		{ this, "CA",	"P",	SpeciesBond::HarmonicForm,	1840.96,	1.78 },
		{ this, "CT",	"P+",	SpeciesBond::HarmonicForm,	1774.02,	1.82 },
		{ this, "S",	"S",	SpeciesBond::HarmonicForm,	1389.09,	2.038 },
		{ this, "C9",	"C9",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C9",	"C8",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C9",	"C7",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C8",	"C8",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C8",	"C7",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C7",	"C7",	SpeciesBond::HarmonicForm,	4435.04,	1.34 },
		{ this, "C8",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C8",	"CH",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C8",	"C2",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C8",	"C3",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C7",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C7",	"CH",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C7",	"C2",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "C7",	"C3",	SpeciesBond::HarmonicForm,	2652.66,	1.5 },
		{ this, "CT",	"C3",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "CT",	"C2",	SpeciesBond::HarmonicForm,	2175.68,	1.526 },
		{ this, "CA",	"NB",	SpeciesBond::HarmonicForm,	3464.35,	1.391 },
		{ this, "CA",	"N",	SpeciesBond::HarmonicForm,	3573.14,	1.381 },
		{ this, "CB",	"CT",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		/* { this, "CC",	"CB",	SpeciesBond::HarmonicForm,	4351.36,	1.37 }, */
		{ this, "CT",	"F",	SpeciesBond::HarmonicForm,	3071.06,	1.332 },
		{ this, "C2",	"F",	SpeciesBond::HarmonicForm,	3071.06,	1.38 },
		{ this, "CA",	"F",	SpeciesBond::HarmonicForm,	3514.56,	1.354 },
		{ this, "CM",	"F",	SpeciesBond::HarmonicForm,	3514.56,	1.34 },
		{ this, "CZ",	"F",	SpeciesBond::HarmonicForm,	3765.6,		1.279 },
		{ this, "C",	"F",	SpeciesBond::HarmonicForm,	3514.56,	1.357 },
		{ this, "CT",	"CO",	SpeciesBond::HarmonicForm,	2242.62,	1.529 },
		{ this, "OH",	"CO",	SpeciesBond::HarmonicForm,	2677.76,	1.38 },
		{ this, "HC",	"CO",	SpeciesBond::HarmonicForm,	2845.12,	1.09 },
		{ this, "SY",	"C3",	SpeciesBond::HarmonicForm,	1857.7,		1.81 },
		{ this, "SY",	"CA",	SpeciesBond::HarmonicForm,	2845.12,	1.77 },
		{ this, "SY",	"C8",	SpeciesBond::HarmonicForm,	1857.7,		1.76 },
		{ this, "SY",	"OY",	SpeciesBond::HarmonicForm,	5857.6,		1.44 },
		{ this, "SZ",	"OY",	SpeciesBond::HarmonicForm,	5857.6,		1.53 },
		{ this, "SY",	"N",	SpeciesBond::HarmonicForm,	3631.71,	1.67 },
		{ this, "SY",	"CT",	SpeciesBond::HarmonicForm,	2845.12,	1.77 },
		{ this, "SZ",	"CT",	SpeciesBond::HarmonicForm,	2845.12,	1.79 },
		{ this, "U",	"OU",	SpeciesBond::HarmonicForm,	4.184,		1.8 },
		{ this, "CA",	"S",	SpeciesBond::HarmonicForm,	2092,		1.76 },
		{ this, "CM",	"S",	SpeciesBond::HarmonicForm,	2092,		1.76 },
		{ this, "CM",	"CY",	SpeciesBond::HarmonicForm,	2652.66,	1.51 },
		{ this, "CY",	"NT",	SpeciesBond::HarmonicForm,	3196.58,	1.448 },
		{ this, "SY",	"NT",	SpeciesBond::HarmonicForm,	2845.12,	1.77 },
		{ this, "C",	"NT",	SpeciesBond::HarmonicForm,	2652.66,	1.522 },
		{ this, "C",	"CW",	SpeciesBond::HarmonicForm,	3347.2,		1.49 },
		{ this, "Zn",	"N",	SpeciesBond::HarmonicForm,	334.72,		2.05 },
		{ this, "Zn",	"OH",	SpeciesBond::HarmonicForm,	786.592,	1.8 },
		{ this, "Zn",	"OW",	SpeciesBond::HarmonicForm,	334.72,		2.05 }
	};

	static ForcefieldAngleTerm angleTerms[] =
	{
		//	i	j	k	Type (Harmonic)			k	eq
		{ this, "HW",	"OW",	"HW",	SpeciesAngle::HarmonicForm,	627.6,		109.5 },
		/* { this, "HW",	"OW",	"LP",	SpeciesAngle::HarmonicForm,	418.4,		54.75 }, */
		{ this, "OU",	"U",	"OU",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "HC",	"C*",	"CW",	SpeciesAngle::HarmonicForm,	292.88,		126.8 },
		{ this, "HC",	"C*",	"CB",	SpeciesAngle::HarmonicForm,	292.88,		126.8 },
		{ this, "HC",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	292.88,		126.8 },
		{ this, "HC",	"CS",	"CB",	SpeciesAngle::HarmonicForm,	292.88,		126.8 },
		{ this, "HA",	"CA",	"CW",	SpeciesAngle::HarmonicForm,	292.88,		126.9 },
		{ this, "HC",	"C=",	"CW",	SpeciesAngle::HarmonicForm,	292.88,		122.0 },
		{ this, "HA",	"CW",	"CA",	SpeciesAngle::HarmonicForm,	292.88,		130.7 },
		{ this, "HA",	"CW",	"C=",	SpeciesAngle::HarmonicForm,	292.88,		130.7 },
		{ this, "HA",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		121.6 },
		{ this, "CT",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "C!",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "CT",	"CW",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "C!",	"CW",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "CA",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "HA",	"CW",	"CV",	SpeciesAngle::HarmonicForm,	292.88,		130.7 },
		{ this, "HA",	"CV",	"CW",	SpeciesAngle::HarmonicForm,	292.88,		128.2 },
		{ this, "HC",	"CT",	"CZ",	SpeciesAngle::HarmonicForm,	292.88,		108.5 },
		{ this, "CT",	"CT",	"CZ",	SpeciesAngle::HarmonicForm,	488.273,	112.7 },
		{ this, "CT",	"CZ",	"CZ",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "CA",	"CZ",	"CZ",	SpeciesAngle::HarmonicForm,	1338.88,		180.0 },
		{ this, "HC",	"CZ",	"CZ",	SpeciesAngle::HarmonicForm,	937.216,	180.0 },
		{ this, "CA",	"CA",	"CZ",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CT",	"CZ",	"NZ",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "N2",	"CZ",	"NZ",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "CA",	"CZ",	"NZ",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "HC",	"CT",	"NZ",	SpeciesAngle::HarmonicForm,	292.88,		108.5 },
		{ this, "CA",	"CA",	"NZ",	SpeciesAngle::HarmonicForm,	669.44,		120.0 },
		{ this, "CT",	"NZ",	"CZ",	SpeciesAngle::HarmonicForm,	1255.2,		180.0 },
		{ this, "CA",	"NZ",	"CZ",	SpeciesAngle::HarmonicForm,	1422.56,		180.0 },
		{ this, "CT",	"CX",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "HA",	"CX",	"CX",	SpeciesAngle::HarmonicForm,	292.88,		130.7 },
		{ this, "CX",	"CX",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		106.3 },
		{ this, "CX",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		109.8 },
		{ this, "C2",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "C2",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "C2",	"C",	"O2",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "C3",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "C3",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "C3",	"C",	"O2",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CA",	"C",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CA",	"C",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"CA",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"CA",	"SH",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"CA",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CM",	"CM",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		123.0 },
		{ this, "C=",	"CM",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		123.0 },
		{ this, "CM",	"CM",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		123.0 },
		{ this, "C=",	"CM",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		123.0 },
		{ this, "CB",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		111.3 },
		{ this, "CB",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		128.8 },
		{ this, "CB",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		111.3 },
		{ this, "CS",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		128.2 },
		{ this, "CD",	"C",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CD",	"C",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CH",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "CH",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "CH",	"C",	"O2",	SpeciesAngle::HarmonicForm,	543.92,		117.0 },
		{ this, "CH",	"C",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		115.0 },
		/* { this, "CJ",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		114.1 }, */
		/* { this, "CJ",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		125.3 }, */
		{ this, "CM",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		114.1 },
		{ this, "CM",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		125.3 },
		{ this, "C=",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		124.0 },
		{ this, "C=",	"C",	"HC",	SpeciesAngle::HarmonicForm,	669.44,		116.0 },
		{ this, "CT",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "CT",	"C",	"NM",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "CA",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		115.5 },
		{ this, "CM",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		115.5 },
		{ this, "CT",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "CA",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		/* { this, "CT",	"C",	"S=",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		{ this, "CT",	"NO",	"ON",	SpeciesAngle::HarmonicForm,	669.44,		117.5 },
		{ this, "CA",	"NO",	"ON",	SpeciesAngle::HarmonicForm,	669.44,		117.5 },
		{ this, "CT",	"CT",	"NO",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "HC",	"CT",	"NO",	SpeciesAngle::HarmonicForm,	292.88,		105.0 },
		{ this, "CA",	"CA",	"NO",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "HC",	"C",	"N",	SpeciesAngle::HarmonicForm,	334.72,		114.0 },
		{ this, "HC",	"C",	"OS",	SpeciesAngle::HarmonicForm,	334.72,		115.0 },
		{ this, "HC",	"C",	"OH",	SpeciesAngle::HarmonicForm,	334.72,		115.0 },
		{ this, "O",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		123.0 },
		/* { this, "S=",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		127.0 }, */
		{ this, "NC",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		122.0 },
		{ this, "CT",	"C",	"OH",	SpeciesAngle::HarmonicForm,	585.76,		108.0 },
		{ this, "CT",	"C",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "CT",	"C",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "C=",	"C",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "CT",	"C",	"O2",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CA",	"C",	"O2",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CT",	"CT",	"Cl",	SpeciesAngle::HarmonicForm,	577.392,	109.8 },
		{ this, "C",	"CT",	"Cl",	SpeciesAngle::HarmonicForm,	577.392,	109.8 },
		{ this, "CA",	"CA",	"Cl",	SpeciesAngle::HarmonicForm,	627.6,		120.0 },
		{ this, "CM",	"CM",	"Cl",	SpeciesAngle::HarmonicForm,	627.6,		121.5 },
		{ this, "Cl",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	502.08,		114.0 },
		{ this, "Cl",	"CT",	"Cl",	SpeciesAngle::HarmonicForm,	652.704,	111.7 },
		{ this, "HC",	"CT",	"Cl",	SpeciesAngle::HarmonicForm,	426.768,	107.6 },
		{ this, "CT",	"CT",	"Br",	SpeciesAngle::HarmonicForm,	577.392,	110.0 },
		{ this, "CA",	"CT",	"Br",	SpeciesAngle::HarmonicForm,	577.392,	110.0 },
		{ this, "C",	"CT",	"Br",	SpeciesAngle::HarmonicForm,	577.392,	109.8 },
		{ this, "CT",	"C",	"Cl",	SpeciesAngle::HarmonicForm,	627.6,		109.0 },
		{ this, "CT",	"C",	"Br",	SpeciesAngle::HarmonicForm,	627.6,		109.0 },
		{ this, "O",	"C",	"Cl",	SpeciesAngle::HarmonicForm,	627.6,		119.0 },
		{ this, "O",	"C",	"Br",	SpeciesAngle::HarmonicForm,	627.6,		119.0 },
		{ this, "CA",	"CA",	"Br",	SpeciesAngle::HarmonicForm,	627.6,		120.0 },
		{ this, "CM",	"CM",	"Br",	SpeciesAngle::HarmonicForm,	627.6,		120.0 },
		{ this, "Br",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	502.08,		114.0 },
		{ this, "Br",	"CT",	"Br",	SpeciesAngle::HarmonicForm,	652.704,	111.7 },
		{ this, "HC",	"CT",	"Br",	SpeciesAngle::HarmonicForm,	426.768,	107.6 },
		{ this, "CA",	"CA",	"I",	SpeciesAngle::HarmonicForm,	627.6,		120.0 },
		{ this, "CT",	"CT",	"I",	SpeciesAngle::HarmonicForm,	627.6,		112.0 },
		{ this, "HC",	"CT",	"I",	SpeciesAngle::HarmonicForm,	627.6,		111.0 },
		{ this, "CA",	"CA",	"F",	SpeciesAngle::HarmonicForm,	669.44,		120.0 },
		{ this, "CM",	"CM",	"F",	SpeciesAngle::HarmonicForm,	669.44,		121.5 },
		{ this, "C",	"CM",	"F",	SpeciesAngle::HarmonicForm,	669.44,		121.5 },
		{ this, "F",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	418.4,		112.0 },
		{ this, "F",	"CM",	"F",	SpeciesAngle::HarmonicForm,	669.44,		108.0 },
		{ this, "F",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		121.0 },
		{ this, "F",	"C",	"CT",	SpeciesAngle::HarmonicForm,	669.44,		111.0 },
		{ this, "N",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		122.9 },
		{ this, "NM",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		122.9 },
		{ this, "N",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		114.2 },
		/* { this, "N",	"C",	"S=",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		{ this, "N*",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		115.4 },
		{ this, "N*",	"C",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		118.6 },
		{ this, "NA",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		118.6 },
		{ this, "N*",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.9 },
		{ this, "NA",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.6 },
		{ this, "NC",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		122.5 },
		{ this, "NC",	"C",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		118.6 },
		{ this, "NA",	"CM",	"H4",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "N*",	"CM",	"H4",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "N",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "O",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		126.0 },
		{ this, "ON",	"NO",	"ON",	SpeciesAngle::HarmonicForm,	669.44,		125.0 },
		{ this, "O",	"C",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		121.0 },
		{ this, "O2",	"C",	"O2",	SpeciesAngle::HarmonicForm,	669.44,		126.0 },
		{ this, "C2",	"C*",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		128.6 },
		{ this, "C2",	"CA",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		128.6 },
		/* { this, "C2",	"C*",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		/* { this, "C2",	"C*",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		{ this, "C2",	"C*",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		/* { this, "CB",	"C*",	"CG",	SpeciesAngle::HarmonicForm,	711.28,		106.4 }, */
		{ this, "CB",	"C*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.6 },
		{ this, "CB",	"C*",	"CW",	SpeciesAngle::HarmonicForm,	711.28,		106.4 },
		{ this, "CT",	"C*",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		/* { this, "C2",	"CS",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		/* { this, "C2",	"CS",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		125.0 }, */
		{ this, "C2",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		/* { this, "CB",	"CS",	"CG",	SpeciesAngle::HarmonicForm,	711.28,		106.4 }, */
		{ this, "CB",	"CS",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.6 },
		{ this, "CB",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	711.28,		106.4 },
		{ this, "CT",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		{ this, "C",	"C2",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C",	"C2",	"N",	SpeciesAngle::HarmonicForm,	669.44,		110.3 },
		{ this, "C",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "C*",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	115.6 },
		{ this, "C2",	"C2",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"C2",	"C3",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C3",	"C2",	"C3",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"CH",	"C3",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"CH",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"CH",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"C2",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C2",	"C2",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C2",	"C2",	"N",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "C2",	"C2",	"N2",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "C2",	"C2",	"N3",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		/* { this, "**C2",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		111.2 }, */
		{ this, "CT",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C3",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C2",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "CH",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "CT",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C3",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C2",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "CH",	"C2",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "CT",	"CH",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C3",	"CH",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C2",	"CH",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "CH",	"CH",	"NT",	SpeciesAngle::HarmonicForm,	470.282,	109.47 },
		{ this, "C2",	"C2",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"CH",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C2",	"C2",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "C3",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C3",	"C2",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CA",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CD",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		/* { this, "CC",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	113.1 }, */
		{ this, "CH",	"C2",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "CH",	"C2",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CH",	"C2",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CH",	"C2",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "CH",	"C2",	"SH",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "C3",	"C2",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C2",	"C2",	"CA",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "CH",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "C2",	"CH",	"CA",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "C",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C2",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "C2",	"CA",	"CD",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "C2",	"CD",	"CD",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CA",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"C!",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"CA",	"C!",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CR",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CS",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CW",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CU",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"C!",	"CV",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"CA",	"CB",	SpeciesAngle::HarmonicForm,	527.184,	120.0 },
		{ this, "CA",	"CA",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CA",	"CA",	"CM",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CA",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	711.28,		119.7 },
		{ this, "CA",	"CA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"CA",	"NT",	SpeciesAngle::HarmonicForm,	585.76,		120.1 },
		{ this, "CA",	"CA",	"N3",	SpeciesAngle::HarmonicForm,	585.76,		120.1 },
		{ this, "CA",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C!",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CA",	"CA",	"DM",	SpeciesAngle::HarmonicForm,	292.88,		90.0 },
		{ this, "HA",	"CA",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		90.0 },
		{ this, "OY",	"SZ",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		90.0 },
		{ this, "CT",	"NC",	"NZ",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NC",	"NZ",	"NZ",	SpeciesAngle::HarmonicForm,	836.8,		180.0 },
		{ this, "CT",	"CT",	"NC",	SpeciesAngle::HarmonicForm,	543.92,		109.0 },
		{ this, "NC",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		116.0 },
		{ this, "CA",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CA",	"C!",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C!",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C!",	"C!",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CA",	"NC",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CA",	"NC",	"C!",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CA",	"CA",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		107.4 },
		{ this, "CW",	"NA",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		109.8 },
		{ this, "CV",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		106.3 },
		{ this, "CW",	"CV",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "CW",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		109.8 },
		{ this, "CB",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		109.8 },
		{ this, "CW",	"C=",	"C=",	SpeciesAngle::HarmonicForm,	292.88,		106.0 },
		{ this, "CA",	"NA",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		109.8 },
		{ this, "NC",	"CA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "NC",	"CQ",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		115.5 },
		{ this, "NB",	"CV",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.5 },
		{ this, "CA",	"CV",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "CA",	"NC",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "CT",	"NC",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "OS",	"CW",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		110.6 },
		{ this, "OS",	"CW",	"C=",	SpeciesAngle::HarmonicForm,	585.76,		110.0 },
		{ this, "CW",	"OS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		106.5 },
		{ this, "CW",	"OS",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		106.5 },
		{ this, "CR",	"OS",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		106.5 },
		{ this, "OS",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		113.4 },
		{ this, "S",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		113.4 },
		{ this, "S",	"CR",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		113.4 },
		{ this, "S",	"CR",	"NT",	SpeciesAngle::HarmonicForm,	585.76,		120.2 },
		{ this, "NB",	"CR",	"NT",	SpeciesAngle::HarmonicForm,	585.76,		126.1 },
		{ this, "S",	"CW",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "S",	"CW",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "NA",	"CW",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		107.7 },
		{ this, "CW",	"CS",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		107.3 },
		{ this, "CW",	"CS",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		125.7 },
		{ this, "CW",	"CS",	"C!",	SpeciesAngle::HarmonicForm,	585.76,		125.7 },
		{ this, "CS",	"CW",	"C!",	SpeciesAngle::HarmonicForm,	585.76,		132.1 },
		{ this, "CS",	"CS",	"C!",	SpeciesAngle::HarmonicForm,	585.76,		127.5 },
		{ this, "CS",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		132.1 },
		{ this, "CS",	"CW",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		132.1 },
		{ this, "CS",	"CW",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		132.1 },
		{ this, "CS",	"CS",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		127.5 },
		{ this, "CU",	"NB",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		104.1 },
		{ this, "CW",	"NB",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		104.1 },
		{ this, "NB",	"CU",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		118.9 },
		{ this, "NB",	"CU",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.9 },
		{ this, "NB",	"CW",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.9 },
		{ this, "CU",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		103.8 },
		{ this, "CW",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		103.8 },
		{ this, "NB",	"CU",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		111.9 },
		{ this, "NB",	"CW",	"CS",	SpeciesAngle::HarmonicForm,	585.76,		111.9 },
		{ this, "CA",	"CU",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		128.6 },
		{ this, "CU",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		128.2 },
		{ this, "CU",	"NB",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		105.3 },
		{ this, "NB",	"NA",	"CW",	SpeciesAngle::HarmonicForm,	468.608,	113.1 },
		{ this, "CB",	"NA",	"NB",	SpeciesAngle::HarmonicForm,	468.608,	113.1 },
		{ this, "CR",	"NA",	"NB",	SpeciesAngle::HarmonicForm,	468.608,	113.1 },
		{ this, "NB",	"NA",	"H",	SpeciesAngle::HarmonicForm,	468.608,	118.4 },
		{ this, "NB",	"NA",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		118.4 },
		{ this, "NB",	"NA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.4 },
		{ this, "CW",	"OS",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		108.9 },
		{ this, "NB",	"CR",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		115.0 },
		{ this, "NB",	"CR",	"S",	SpeciesAngle::HarmonicForm,	585.76,		115.0 },
		{ this, "CR",	"OS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		104.0 },
		{ this, "CV",	"CW",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		108.0 },
		{ this, "HA",	"CR",	"OS",	SpeciesAngle::HarmonicForm,	292.88,		117.0 },
		{ this, "OS",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		114.5 },
		{ this, "CB",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		/* { this, "CC",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		{ this, "CB",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		123.5 },
		{ this, "CB",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		117.3 },
		{ this, "CD",	"CA",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		/* { this, "CJ",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		120.1 }, */
		/* { this, "CJ",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		121.5 }, */
		{ this, "CM",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		120.1 },
		{ this, "CA",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		120.1 },
		{ this, "CM",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		121.5 },
		{ this, "CM",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.5 },
		{ this, "CN",	"CA",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "NY",	"CA",	"NY",	SpeciesAngle::HarmonicForm,	585.76,		111.8 },
		{ this, "NZ",	"CA",	"NY",	SpeciesAngle::HarmonicForm,	585.76,		124.1 },
		{ this, "CA",	"NZ",	"H",	SpeciesAngle::HarmonicForm,	292.88,		113.0 },
		{ this, "CA",	"NY",	"H",	SpeciesAngle::HarmonicForm,	418.4,		112.5 },
		{ this, "CA",	"NY",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		120.5 },
		{ this, "H",	"NY",	"H",	SpeciesAngle::HarmonicForm,	364.845,	106.4 },
		{ this, "CT",	"NY",	"H",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CT",	"CT",	"NY",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "HC",	"CT",	"NY",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "N2",	"CA",	"N2",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "N2",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "N2",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		119.3 },
		{ this, "N2",	"CQ",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		119.3 },
		{ this, "N2",	"CQ",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "NA",	"CA",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		123.3 },
		{ this, "C",	"CB",	"CB",	SpeciesAngle::HarmonicForm,	711.28,		119.2 },
		{ this, "C",	"CB",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		130.0 },
		{ this, "N",	"CQ",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		123.3 },
		{ this, "C",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		130.0 },
		{ this, "C",	"CB",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		130.0 },
		{ this, "C*",	"CB",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		134.9 },
		{ this, "C*",	"CB",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		134.9 },
		{ this, "CA",	"CB",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		134.9 },
		{ this, "C*",	"CB",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		108.8 },
		/* { this, "CA",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
		/* { this, "C*",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
		{ this, "CS",	"CB",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		134.9 },
		{ this, "CS",	"CB",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		134.9 },
		{ this, "CS",	"CB",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		108.8 },
		/* { this, "CS",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		108.8 }, */
		{ this, "CA",	"CB",	"CB",	SpeciesAngle::HarmonicForm,	711.28,		117.3 },
		{ this, "CA",	"CB",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		116.2 },
		{ this, "CA",	"CB",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		132.4 },
		{ this, "CB",	"CB",	"N*",	SpeciesAngle::HarmonicForm,	585.76,		106.2 },
		{ this, "CB",	"CB",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		106.2 },
		{ this, "CS",	"CR",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		106.2 },
		{ this, "CB",	"CB",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "CR",	"CS",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		110.4 },
		{ this, "CB",	"CB",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		127.7 },
		{ this, "CB",	"CB",	"N",	SpeciesAngle::HarmonicForm,	585.76,		127.7 },
		{ this, "CS",	"CR",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		127.7 },
		{ this, "CD",	"CB",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		116.2 },
		{ this, "N*",	"CB",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		126.2 },
		{ this, "NA",	"CB",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		126.2 },
		{ this, "NB",	"CB",	"N",	SpeciesAngle::HarmonicForm,	585.76,		126.2 },
		{ this, "NB",	"CR",	"N",	SpeciesAngle::HarmonicForm,	585.76,		126.2 },
		{ this, "NA",	"CR",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		126.2 },
		/* { this, "C2",	"CC",	"CF",	SpeciesAngle::HarmonicForm,	585.76,		131.9 }, */
		/* { this, "C2",	"CC",	"CG",	SpeciesAngle::HarmonicForm,	585.76,		129.05 }, */
		/* { this, "C2",	"CC",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		131.9 }, */
		/* { this, "C2",	"CC",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		129.05 }, */
		/* { this, "C2",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		122.2 }, */
		/* { this, "C2",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		121.05 }, */
		/* { this, "CF",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		105.9 }, */
		/* { this, "CG",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.75 }, */
		/* { this, "CG",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		109.9 }, */
		/* { this, "CT",	"CC",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		{ this, "CT",	"CW",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		130.7 },
		{ this, "CT",	"CV",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		130.7 },
		{ this, "CT",	"CX",	"CX",	SpeciesAngle::HarmonicForm,	585.76,		130.7 },
		{ this, "CT",	"CW",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		/* { this, "CT",	"CC",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		/* { this, "CT",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		/* { this, "CT",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		/* { this, "CV",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		{ this, "CW",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		/* { this, "CW",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		/* { this, "CW",	"CC",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		{ this, "C",	"CD",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CA",	"CD",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CB",	"CD",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CD",	"CD",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CD",	"CD",	"CN",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		/* { this, "CD",	"CD",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
		/* { this, "N*",	"CE",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		113.9 }, */
		/* { this, "CC",	"CF",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		109.9 }, */
		/* { this, "C*",	"CG",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 }, */
		{ this, "CA",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 },
		/* { this, "C*",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 }, */
		/* { this, "CC",	"CG",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		105.9 }, */
		{ this, "C",	"CH",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "C",	"CH",	"C3",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "C",	"CH",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "C",	"CH",	"N",	SpeciesAngle::HarmonicForm,	527.184,	110.1 },
		{ this, "C",	"CH",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "C2",	"CH",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	111.5 },
		{ this, "C2",	"CH",	"N",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "C2",	"CH",	"N*",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C2",	"CH",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"CH",	"C3",	SpeciesAngle::HarmonicForm,	527.184,	111.5 },
		{ this, "C3",	"CH",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	111.5 },
		{ this, "C3",	"CH",	"N",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"CH",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CH",	"CH",	"CH",	SpeciesAngle::HarmonicForm,	527.184,	111.5 },
		{ this, "CH",	"CH",	"N",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "CH",	"CH",	"N*",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CH",	"CH",	"OH",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "CH",	"CH",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "N*",	"CH",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		/* { this, "NC",	"CI",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		129.1 }, */
		/* { this, "C",	"CJ",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		120.7 }, */
		/* { this, "CA",	"CJ",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		117.0 }, */
		/* { this, "CJ",	"CJ",	"N*",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
		/* { this, "CM",	"CJ",	"N*",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
		{ this, "N*",	"CK",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		113.9 },
		{ this, "NA",	"CK",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		113.9 },
		{ this, "NA",	"CK",	"H5",	SpeciesAngle::HarmonicForm,	292.88,		123.05 },
		{ this, "N*",	"CK",	"H5",	SpeciesAngle::HarmonicForm,	292.88,		123.05 },
		{ this, "NB",	"CK",	"H5",	SpeciesAngle::HarmonicForm,	292.88,		123.05 },
		{ this, "C",	"CM",	"C3",	SpeciesAngle::HarmonicForm,	711.28,		119.7 },
		/* { this, "C",	"CM",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		120.7 }, */
		{ this, "C",	"CM",	"CM",	SpeciesAngle::HarmonicForm,	711.28,		120.7 },
		{ this, "C",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		119.7 },
		{ this, "C",	"CA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		119.7 },
		{ this, "C",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		119.7 },
		/* { this, "C3",	"CM",	"CJ",	SpeciesAngle::HarmonicForm,	711.28,		119.7 }, */
		{ this, "CA",	"CM",	"CM",	SpeciesAngle::HarmonicForm,	711.28,		117.0 },
		{ this, "CA",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		123.3 },
		/* { this, "CJ",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	711.28,		119.7 }, */
		{ this, "CM",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CM",	"C=",	"C=",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CM",	"C=",	"C",	SpeciesAngle::HarmonicForm,	585.76,		118.7 },
		{ this, "CM",	"C=",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C=",	"C=",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"CM",	"C=",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CM",	"CT",	"CM",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "CM",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CM",	"CM",	"H4",	SpeciesAngle::HarmonicForm,	292.88,		119.7 },
		{ this, "CM",	"C=",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C=",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C=",	"C=",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CT",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		115.0 },
		{ this, "CA",	"C",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		115.0 },
		{ this, "CT",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		117.0 },
		{ this, "HC",	"CM",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		117.0 },
		{ this, "CT",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		130.0 },
		{ this, "CT",	"C+",	"CT",	SpeciesAngle::HarmonicForm,	1445.99,		120.0 },
		{ this, "CT",	"C+",	"HC",	SpeciesAngle::HarmonicForm,	1204.99,		120.0 },
		{ this, "CT",	"CT",	"C+",	SpeciesAngle::HarmonicForm,	527.184,	105.0 },
		{ this, "HC",	"CT",	"C+",	SpeciesAngle::HarmonicForm,	292.88,		105.0 },
		{ this, "CM",	"CM",	"N*",	SpeciesAngle::HarmonicForm,	585.76,		121.2 },
		{ this, "CM",	"CM",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		121.2 },
		{ this, "HC",	"CM",	"N*",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "HC",	"CM",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "CA",	"CN",	"CB",	SpeciesAngle::HarmonicForm,	711.28,		122.7 },
		{ this, "CA",	"CN",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 },
		{ this, "CB",	"CN",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		122.7 },
		/* { this, "CB",	"CC",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		122.7 }, */
		/* { this, "CB",	"CC",	"CD",	SpeciesAngle::HarmonicForm,	711.28,		122.7 }, */
		{ this, "CB",	"CA",	"CW",	SpeciesAngle::HarmonicForm,	527.184,	106.4 },
		{ this, "CB",	"CA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.6 },
		{ this, "CB",	"CN",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		104.4 },
		/* { this, "CB",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		104.4 }, */
		{ this, "CD",	"CN",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 },
		/* { this, "CA",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 }, */
		/* { this, "CD",	"CC",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		132.8 }, */
		/* { this, "NA",	"CP",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		110.75 }, */
		/* { this, "NA",	"CP",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
		{ this, "HA",	"CQ",	"NC",	SpeciesAngle::HarmonicForm,	292.88,		115.45 },
		{ this, "H5",	"CQ",	"NC",	SpeciesAngle::HarmonicForm,	292.88,		115.45 },
		{ this, "NC",	"CQ",	"NC",	SpeciesAngle::HarmonicForm,	585.76,		129.1 },
		{ this, "HA",	"CR",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CX",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CR",	"NB",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CK",	"N*",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CK",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CK",	"NB",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "NA",	"CR",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NA",	"CR",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NA",	"CR",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		{ this, "NB",	"CR",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		125.0 },
		{ this, "NA",	"CR",	"SY",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NB",	"CR",	"SY",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NB",	"CR",	"S",	SpeciesAngle::HarmonicForm,	585.76,		113.6 },
		{ this, "C",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "CM",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "CW",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CV",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CX",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "C",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "C",	"CT",	"N",	SpeciesAngle::HarmonicForm,	527.184,	110.1 },
		{ this, "C",	"CT",	"NC",	SpeciesAngle::HarmonicForm,	527.184,	110.1 },
		{ this, "C*",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	115.6 },
		{ this, "C*",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CS",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	115.6 },
		{ this, "CS",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CA",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CA",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CA",	"N3",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		/* { this, "CC",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	113.1 }, */
		{ this, "CW",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CV",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CX",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		/* { this, "CC",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 }, */
		{ this, "CM",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "C=",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CT",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	488.273,	112.7 },
		{ this, "C3",	"CT",	"C3",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "C2",	"CT",	"C2",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "C2",	"CT",	"C3",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "C3",	"CT",	"C",	SpeciesAngle::HarmonicForm,	527.184,	109.5 },
		{ this, "CT",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	313.8,		110.7 },
		{ this, "CT",	"HC",	"DM",	SpeciesAngle::HarmonicForm,	313.8,		109.47 },
		{ this, "CT",	"OS",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "CT",	"OH",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "OH",	"HO",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "CT",	"S",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "CT",	"SH",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "SH",	"HS",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		109.47 },
		{ this, "NZ",	"CZ",	"DM",	SpeciesAngle::HarmonicForm,	83.68,		90.0 },
		{ this, "CT",	"CT",	"N",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "CT",	"CT",	"NM",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "CT",	"CT",	"N*",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"CO",	"N*",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"CT",	"N2",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "C",	"CT",	"N3",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "C",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "CA",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "CA",	"CT",	"NA",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "CT",	"CT",	"N3",	SpeciesAngle::HarmonicForm,	669.44,		111.2 },
		{ this, "CT",	"CT",	"OH",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CA",	"CT",	"OH",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CA",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"CT",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "CT",	"CT",	"SH",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "CA",	"CT",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "CW",	"CT",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "CT",	"NT",	"H",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CA",	"NT",	"H",	SpeciesAngle::HarmonicForm,	292.88,		111.0 },
		{ this, "CA",	"NT",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"NT",	"CT",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CT",	"NT",	"CH",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CT",	"NT",	"C2",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CT",	"NT",	"C3",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CH",	"NT",	"CH",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CH",	"NT",	"C2",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "CH",	"NT",	"C3",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "C2",	"NT",	"C2",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "C2",	"NT",	"C3",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "C3",	"NT",	"C3",	SpeciesAngle::HarmonicForm,	433.462,	107.2 },
		{ this, "HC",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	276.144,	107.8 },
		{ this, "DM",	"HC",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"HO",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"HS",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"OS",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"OH",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"S",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"SH",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"F",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"Cl",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"Br",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "DM",	"I",	"DM",	SpeciesAngle::HarmonicForm,	276.144,	109.47 },
		{ this, "CY",	"CY",	"HC",	SpeciesAngle::HarmonicForm,	313.8,		117.2 },
		{ this, "CY",	"CY",	"CY",	SpeciesAngle::HarmonicForm,	251.04,		79.2 },
		{ this, "CY",	"CY",	"CT",	SpeciesAngle::HarmonicForm,	313.8,		117.2 },
		{ this, "CY",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	313.8,		110.7 },
		{ this, "HC",	"CY",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		114.3 },
		{ this, "HC",	"CY",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		114.3 },
		{ this, "CY",	"CY",	"N$",	SpeciesAngle::HarmonicForm,	669.44,		89.0 },
		{ this, "CY",	"N$",	"C$",	SpeciesAngle::HarmonicForm,	418.4,		94.0 },
		{ this, "N$",	"C$",	"CY",	SpeciesAngle::HarmonicForm,	585.76,		91.0 },
		{ this, "CY",	"CY",	"C$",	SpeciesAngle::HarmonicForm,	527.184,	85.0 },
		{ this, "N$",	"C$",	"O",	SpeciesAngle::HarmonicForm,	669.44,		134.0 },
		{ this, "CY",	"C$",	"O",	SpeciesAngle::HarmonicForm,	669.44,		134.0 },
		{ this, "HC",	"CY",	"N$",	SpeciesAngle::HarmonicForm,	292.88,		111.0 },
		{ this, "HC",	"CY",	"N",	SpeciesAngle::HarmonicForm,	292.88,		108.0 },
		{ this, "HC",	"CY",	"C$",	SpeciesAngle::HarmonicForm,	313.8,		110.0 },
		{ this, "CY",	"CY",	"N",	SpeciesAngle::HarmonicForm,	313.8,		126.0 },
		{ this, "HC",	"CY",	"S",	SpeciesAngle::HarmonicForm,	313.8,		108.0 },
		{ this, "CY",	"CY",	"S",	SpeciesAngle::HarmonicForm,	460.24,		128.0 },
		{ this, "CY",	"N",	"C",	SpeciesAngle::HarmonicForm,	460.24,		128.0 },
		{ this, "CY",	"N",	"H",	SpeciesAngle::HarmonicForm,	334.72,		113.0 },
		{ this, "N",	"CY",	"C$",	SpeciesAngle::HarmonicForm,	585.76,		117.0 },
		{ this, "N$",	"CY",	"S",	SpeciesAngle::HarmonicForm,	460.24,		109.0 },
		{ this, "C$",	"N$",	"CT",	SpeciesAngle::HarmonicForm,	460.24,		127.0 },
		{ this, "CY",	"S",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	94.0 },
		{ this, "CY",	"N$",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		126.0 },
		{ this, "N$",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	669.44,		110.0 },
		{ this, "N$",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "N$",	"CT",	"C",	SpeciesAngle::HarmonicForm,	669.44,		113.0 },
		/* { this, "CY",	"O$",	"CY",	SpeciesAngle::HarmonicForm,	502.08,		90.0 }, */
		/* { this, "CY",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	418.4,		90.0 }, */
		/* { this, "CT",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	313.8,		117.2 }, */
		/* { this, "HC",	"CY",	"O$",	SpeciesAngle::HarmonicForm,	313.8,		117.2 }, */
		{ this, "HC",	"CT",	"N",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"NM",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"N*",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CO",	"N*",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"N2",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"N3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"NT",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"NC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"OH",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"S",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CT",	"P",	SpeciesAngle::HarmonicForm,	343.088,	109.5 },
		{ this, "CT",	"CT",	"P",	SpeciesAngle::HarmonicForm,	359.824,	109.5 },
		{ this, "CA",	"CT",	"P",	SpeciesAngle::HarmonicForm,	359.824,	109.5 },
		{ this, "CT",	"CT",	"P+",	SpeciesAngle::HarmonicForm,	359.824,	109.5 },
		{ this, "CT",	"P+",	"CT",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "HC",	"CT",	"P+",	SpeciesAngle::HarmonicForm,	343.088,	109.5 },
		{ this, "HC",	"CT",	"SH",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "N*",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "N*",	"CO",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		/* { this, "CC",	"CV",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		/* { this, "CC",	"CV",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		{ this, "CW",	"CW",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "HA",	"CV",	"NB",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C*",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C*",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 },
		/* { this, "CC",	"CW",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		/* { this, "CC",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		{ this, "H4",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "HA",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C",	"N",	"C2",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"N",	"C3",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"N",	"CH",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"N",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"NM",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"N",	"CA",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		{ this, "C",	"N",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.8 },
		{ this, "C2",	"N",	"C3",	SpeciesAngle::HarmonicForm,	418.4,		121.9 },
		/* { this, "C",	"N",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		{ this, "C2",	"N",	"CH",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "C2",	"N",	"H",	SpeciesAngle::HarmonicForm,	317.984,	118.4 },
		{ this, "C3",	"N",	"H",	SpeciesAngle::HarmonicForm,	317.984,	118.4 },
		{ this, "CH",	"N",	"H",	SpeciesAngle::HarmonicForm,	317.984,	118.4 },
		{ this, "CT",	"N",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "CT",	"N2",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "CT",	"NM",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "CA",	"N",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "CA",	"NC",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		118.0 },
		{ this, "CT",	"N",	"H",	SpeciesAngle::HarmonicForm,	317.984,	118.4 },
		{ this, "H",	"N",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "H",	"N2",	"H",	SpeciesAngle::HarmonicForm,	292.88,		113.0 },
		{ this, "H3",	"N",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C",	"N*",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		117.6 },
		/* { this, "C",	"N*",	"CJ",	SpeciesAngle::HarmonicForm,	585.76,		121.6 }, */
		{ this, "C",	"N*",	"CM",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "C",	"NA",	"CM",	SpeciesAngle::HarmonicForm,	585.76,		121.6 },
		{ this, "C",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		117.6 },
		{ this, "C",	"N*",	"CO",	SpeciesAngle::HarmonicForm,	585.76,		117.6 },
		{ this, "C",	"N*",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.2 },
		{ this, "C3",	"N*",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		125.8 },
		/* { this, "C3",	"N*",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
		{ this, "C3",	"N*",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		128.8 },
		/* { this, "CB",	"N*",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		105.4 }, */
		{ this, "CB",	"N*",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		125.8 },
		/* { this, "CE",	"N*",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
		/* { this, "CE",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.8 }, */
		/* { this, "CE",	"N*",	"H",	SpeciesAngle::HarmonicForm,	292.88,		127.3 }, */
		/* { this, "CH",	"N*",	"CJ",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
		{ this, "CH",	"N*",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		128.8 },
		/* { this, "CJ",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		121.2 }, */
		/* { this, "CJ",	"N*",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.2 }, */
		{ this, "CM",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		121.2 },
		{ this, "CM",	"N*",	"CO",	SpeciesAngle::HarmonicForm,	585.76,		121.2 },
		{ this, "CM",	"N*",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.2 },
		{ this, "CM",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.2 },
		{ this, "C2",	"N2",	"CA",	SpeciesAngle::HarmonicForm,	418.4,		123.2 },
		/* { this, "C2",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		118.4 }, */
		{ this, "C2",	"N2",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		118.4 },
		{ this, "C3",	"N2",	"CA",	SpeciesAngle::HarmonicForm,	418.4,		123.2 },
		/* { this, "C3",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		118.4 }, */
		{ this, "CA",	"N2",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		123.2 },
		{ this, "CA",	"N2",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CQ",	"N2",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		/* { this, "CA",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		{ this, "CA",	"N2",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CT",	"N2",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		118.4 },
		{ this, "CT",	"N2",	"H",	SpeciesAngle::HarmonicForm,	292.88,		118.4 },
		/* { this, "H2",	"N2",	"H2",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		{ this, "H3",	"N2",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "C2",	"N3",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "C3",	"N3",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CT",	"N3",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "H3",	"N3",	"H3",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CT",	"N3",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		113.0 },
		{ this, "CA",	"N3",	"CT",	SpeciesAngle::HarmonicForm,	460.24,		114.0 },
		{ this, "C",	"NA",	"C",	SpeciesAngle::HarmonicForm,	585.76,		126.4 },
		{ this, "C",	"N",	"C",	SpeciesAngle::HarmonicForm,	585.76,		126.4 },
		{ this, "C",	"NA",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		125.2 },
		{ this, "C",	"N",	"CQ",	SpeciesAngle::HarmonicForm,	585.76,		125.2 },
		{ this, "C",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		116.8 },
		{ this, "CA",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		118.0 },
		{ this, "CQ",	"N",	"H",	SpeciesAngle::HarmonicForm,	292.88,		118.0 },
		/* { this, "CC",	"NA",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
		/* { this, "CC",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		120.0 }, */
		/* { this, "CC",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 }, */
		/* { this, "CG",	"NA",	"CN",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
		/* { this, "CA",	"NA",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
		/* { this, "CC",	"NA",	"CC",	SpeciesAngle::HarmonicForm,	585.76,		111.6 }, */
		/* { this, "CG",	"NA",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
		/* { this, "CG",	"NA",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		107.3 }, */
		/* { this, "CG",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		126.35 }, */
		{ this, "CN",	"NA",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		111.6 },
		{ this, "CN",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		123.1 },
		/* { this, "CP",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		126.35 }, */
		{ this, "CR",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CW",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CX",	"NA",	"H",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CB",	"N*",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		105.4 },
		{ this, "CB",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		125.8 },
		{ this, "CB",	"N*",	"CO",	SpeciesAngle::HarmonicForm,	585.76,		125.8 },
		{ this, "CB",	"N*",	"H",	SpeciesAngle::HarmonicForm,	251.04,		125.8 },
		{ this, "CK",	"N*",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.8 },
		{ this, "CK",	"N*",	"CO",	SpeciesAngle::HarmonicForm,	585.76,		128.8 },
		{ this, "CK",	"N*",	"H",	SpeciesAngle::HarmonicForm,	251.04,		128.8 },
		{ this, "CB",	"NA",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		105.4 },
		{ this, "CB",	"NA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		125.8 },
		{ this, "CB",	"NA",	"H",	SpeciesAngle::HarmonicForm,	251.04,		125.8 },
		{ this, "CK",	"NA",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		128.8 },
		{ this, "CK",	"NA",	"H",	SpeciesAngle::HarmonicForm,	251.04,		128.8 },
		/* { this, "CB",	"NB",	"CE",	SpeciesAngle::HarmonicForm,	585.76,		103.8 }, */
		{ this, "CB",	"NB",	"CK",	SpeciesAngle::HarmonicForm,	585.76,		103.8 },
		/* { this, "CC",	"NB",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
		/* { this, "CC",	"NB",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		117.0 }, */
		/* { this, "CF",	"NB",	"CP",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
		/* { this, "CF",	"NB",	"CR",	SpeciesAngle::HarmonicForm,	585.76,		105.3 }, */
		{ this, "CR",	"NB",	"CV",	SpeciesAngle::HarmonicForm,	585.76,		110.0 },
		{ this, "CR",	"NB",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		110.0 },
		{ this, "CR",	"NB",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		110.0 },
		{ this, "C",	"NC",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		120.5 },
		{ this, "CA",	"NC",	"CB",	SpeciesAngle::HarmonicForm,	585.76,		112.2 },
		/* { this, "CA",	"NC",	"CI",	SpeciesAngle::HarmonicForm,	585.76,		118.6 }, */
		{ this, "CA",	"NC",	"CQ",	SpeciesAngle::HarmonicForm,	585.76,		118.6 },
		{ this, "CQ",	"NC",	"CQ",	SpeciesAngle::HarmonicForm,	585.76,		118.6 },
		/* { this, "CB",	"NC",	"CI",	SpeciesAngle::HarmonicForm,	585.76,		111.0 }, */
		{ this, "CB",	"NC",	"CQ",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "CR",	"NC",	"CQ",	SpeciesAngle::HarmonicForm,	585.76,		111.0 },
		{ this, "C2",	"NT",	"H",	SpeciesAngle::HarmonicForm,	361.498,	108.1 },
		{ this, "C3",	"NT",	"H",	SpeciesAngle::HarmonicForm,	361.498,	108.1 },
		{ this, "CH",	"NT",	"H",	SpeciesAngle::HarmonicForm,	361.498,	108.1 },
		{ this, "H",	"NT",	"H",	SpeciesAngle::HarmonicForm,	364.845,	106.4 },
		{ this, "H",	"N3",	"H",	SpeciesAngle::HarmonicForm,	364.845,	109.5 },
		{ this, "H",	"N",	"OH",	SpeciesAngle::HarmonicForm,	292.88,		110.2 },
		{ this, "C",	"N",	"OH",	SpeciesAngle::HarmonicForm,	384.928,	115.7 },
		{ this, "N",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	410.032,	105.4 },
		{ this, "C",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	292.88,		113.0 },
		{ this, "CA",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	292.88,		113.0 },
		{ this, "CM",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	292.88,		109.0 },
		{ this, "C2",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "C3",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "CH",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "CT",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "HO",	"OH",	"P",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "C2",	"OS",	"C2",	SpeciesAngle::HarmonicForm,	836.8,		111.8 },
		{ this, "C2",	"OS",	"C3",	SpeciesAngle::HarmonicForm,	836.8,		111.8 },
		{ this, "CH",	"C",	"OS",	SpeciesAngle::HarmonicForm,	677.808,	111.4 },
		{ this, "C",	"OS",	"CH",	SpeciesAngle::HarmonicForm,	694.544,	116.9 },
		{ this, "C",	"OS",	"C2",	SpeciesAngle::HarmonicForm,	694.544,	116.9 },
		{ this, "C",	"OS",	"C3",	SpeciesAngle::HarmonicForm,	694.544,	116.9 },
		{ this, "O",	"C",	"OS",	SpeciesAngle::HarmonicForm,	694.544,	123.4 },
		{ this, "C",	"OS",	"CT",	SpeciesAngle::HarmonicForm,	694.544,	116.9 },
		{ this, "OS",	"C",	"CT",	SpeciesAngle::HarmonicForm,	677.808,	111.4 },
		{ this, "OS",	"C",	"CA",	SpeciesAngle::HarmonicForm,	677.808,	111.4 },
		{ this, "C",	"OS",	"CA",	SpeciesAngle::HarmonicForm,	694.544,	116.9 },
		{ this, "CA",	"CH",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "OS",	"CO",	"OH",	SpeciesAngle::HarmonicForm,	774.877,	111.55 },
		{ this, "OS",	"CO",	"OS",	SpeciesAngle::HarmonicForm,	774.877,	111.55 },
		{ this, "C3",	"OS",	"CO",	SpeciesAngle::HarmonicForm,	836.8,		113.0 },
		{ this, "C2",	"OS",	"CO",	SpeciesAngle::HarmonicForm,	836.8,		113.0 },
		{ this, "CH",	"OS",	"CO",	SpeciesAngle::HarmonicForm,	836.8,		113.0 },
		{ this, "C2",	"CO",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"CO",	"OS",	SpeciesAngle::HarmonicForm,	669.44,		109.5 },
		{ this, "C3",	"CO",	"C3",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "C2",	"C2",	"CO",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "C3",	"C2",	"CO",	SpeciesAngle::HarmonicForm,	527.184,	112.4 },
		{ this, "OS",	"CO",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CO",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	488.273,	112.7 },
		{ this, "CT",	"CO",	"OH",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CT",	"CO",	"HC",	SpeciesAngle::HarmonicForm,	313.8,		110.7 },
		{ this, "CO",	"OH",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "OS",	"CO",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CO",	"OS",	"CT",	SpeciesAngle::HarmonicForm,	502.08,		109.5 },
		{ this, "CO",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	313.8,		110.7 },
		{ this, "CO",	"CT",	"OH",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "OH",	"CO",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"CO",	"HC",	SpeciesAngle::HarmonicForm,	276.144,	109.5 },
		{ this, "C2",	"OS",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "CA",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "C2",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "C3",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "CH",	"OS",	"CH",	SpeciesAngle::HarmonicForm,	836.8,		111.8 },
		{ this, "CH",	"OS",	"HO",	SpeciesAngle::HarmonicForm,	460.24,		108.5 },
		{ this, "CH",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "CT",	"OS",	"CT",	SpeciesAngle::HarmonicForm,	502.08,		109.5 },
		{ this, "Si",	"OS",	"Si",	SpeciesAngle::HarmonicForm,	167.36,		145.0 },
		{ this, "CT",	"OS",	"Si",	SpeciesAngle::HarmonicForm,	334.72,		130.0 },
		{ this, "CT",	"Si",	"OS",	SpeciesAngle::HarmonicForm,	502.08,		100.0 },
		{ this, "CT",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	502.08,		110.0 },
		{ this, "CT",	"CT",	"Si",	SpeciesAngle::HarmonicForm,	502.08,		112.0 },
		{ this, "OS",	"Si",	"OS",	SpeciesAngle::HarmonicForm,	502.08,		110.0 },
		{ this, "Si",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"Si",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "HC",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "H",	"Si",	"H",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "H",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "F",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "Cl",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "Br",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "I",	"Si",	"CT",	SpeciesAngle::HarmonicForm,	292.88,		110.5 },
		{ this, "CT",	"OS",	"CA",	SpeciesAngle::HarmonicForm,	627.6,		111.0 },
		{ this, "CT",	"OS",	"CM",	SpeciesAngle::HarmonicForm,	627.6,		111.0 },
		{ this, "CT",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "CT",	"OH",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "O",	"C",	"O2",	SpeciesAngle::HarmonicForm,	669.44,		126.0 },
		{ this, "O2",	"P",	"O2",	SpeciesAngle::HarmonicForm,	1171.52,	119.9 },
		{ this, "O2",	"P",	"OH",	SpeciesAngle::HarmonicForm,	376.56,		108.23 },
		{ this, "O2",	"P",	"OS",	SpeciesAngle::HarmonicForm,	836.8,		108.23 },
		{ this, "OH",	"P",	"OS",	SpeciesAngle::HarmonicForm,	376.56,		102.6 },
		{ this, "OS",	"P",	"OS",	SpeciesAngle::HarmonicForm,	376.56,		102.6 },
		{ this, "O",	"P",	"OH",	SpeciesAngle::HarmonicForm,	836.8,		108.23 },
		{ this, "O",	"P",	"OS",	SpeciesAngle::HarmonicForm,	836.8,		108.23 },
		{ this, "OH",	"P",	"OH",	SpeciesAngle::HarmonicForm,	376.56,		102.6 },
		{ this, "CT",	"P",	"OS",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "CT",	"P",	"O2",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "CT",	"P",	"O",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "CA",	"P",	"OS",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "CA",	"P",	"OH",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "CA",	"P",	"O",	SpeciesAngle::HarmonicForm,	376.56,		109.5 },
		{ this, "C2",	"S",	"C3",	SpeciesAngle::HarmonicForm,	518.816,	98.9 },
		{ this, "C2",	"S",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "C2",	"S",	"S",	SpeciesAngle::HarmonicForm,	569.024,	103.7 },
		{ this, "C3",	"S",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "C3",	"S",	"S",	SpeciesAngle::HarmonicForm,	569.024,	103.7 },
		{ this, "CT",	"S",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	98.9 },
		{ this, "CR",	"S",	"CW",	SpeciesAngle::HarmonicForm,	619.232,	90.0 },
		{ this, "CW",	"S",	"CW",	SpeciesAngle::HarmonicForm,	619.232,	97.0 },
		{ this, "CT",	"S",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "CT",	"S",	"S",	SpeciesAngle::HarmonicForm,	569.024,	103.7 },
		{ this, "LP",	"S",	"LP",	SpeciesAngle::HarmonicForm,	0,		160.0 },
		{ this, "LP",	"S",	"S",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "C2",	"SH",	"HS",	SpeciesAngle::HarmonicForm,	368.192,	96.0 },
		{ this, "C2",	"SH",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "C3",	"SH",	"HS",	SpeciesAngle::HarmonicForm,	368.192,	96.0 },
		{ this, "C3",	"SH",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "CT",	"SH",	"HS",	SpeciesAngle::HarmonicForm,	368.192,	96.0 },
		{ this, "CA",	"SH",	"HS",	SpeciesAngle::HarmonicForm,	418.4,		96.0 },
		{ this, "CT",	"SH",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "HS",	"SH",	"HS",	SpeciesAngle::HarmonicForm,	292.88,		92.07 },
		{ this, "HS",	"SH",	"LP",	SpeciesAngle::HarmonicForm,	1255.2,		96.7 },
		{ this, "LP",	"SH",	"LP",	SpeciesAngle::HarmonicForm,	0,		160.0 },
		{ this, "P",	"OS",	"P",	SpeciesAngle::HarmonicForm,	836.8,		120.5 },
		{ this, "C9",	"C8",	"C8",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C8",	"C7",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C8",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C8",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C8",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"C8",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"C7",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C9",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"C8",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"C7",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"C8",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"C7",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C7",	"C7",	"C7",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C7",	"C7",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C7",	"C7",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C7",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C7",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "CT",	"C8",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C8",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C8",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CH",	"C8",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CH",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C2",	"C8",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C2",	"C8",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C2",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C3",	"C8",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C7",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C7",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CT",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CH",	"C7",	"CH",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CH",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CH",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C2",	"C7",	"C2",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C2",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "C3",	"C7",	"C3",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CA",	"OS",	"C2",	SpeciesAngle::HarmonicForm,	836.8,		111.8 },
		{ this, "CA",	"CT",	"CA",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "CA",	"CT",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CA",	"CT",	"C",	SpeciesAngle::HarmonicForm,	527.184,	112.0 },
		{ this, "N",	"CT",	"C2",	SpeciesAngle::HarmonicForm,	669.44,		109.7 },
		{ this, "HC",	"CT",	"C2",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "C",	"CT",	"C2",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "CT",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"NA",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		125.2 },
		{ this, "CA",	"CA",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		108.7 },
		{ this, "NA",	"CA",	"NB",	SpeciesAngle::HarmonicForm,	585.76,		123.3 },
		{ this, "CA",	"NB",	"CA",	SpeciesAngle::HarmonicForm,	585.76,		125.2 },
		{ this, "HA",	"CA",	"NB",	SpeciesAngle::HarmonicForm,	292.88,		119.1 },
		{ this, "CA",	"CA",	"N",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"N",	"H",	SpeciesAngle::HarmonicForm,	292.88,		119.8 },
		{ this, "CB",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CA",	"CB",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CB",	"CA",	"NA",	SpeciesAngle::HarmonicForm,	585.76,		108.7 },
		/* { this, "CA",	"CA",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
		{ this, "CB",	"CB",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		/* { this, "CB",	"CB",	"CC",	SpeciesAngle::HarmonicForm,	711.28,		120.0 }, */
		{ this, "CB",	"CT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "CT",	"CT",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "C2",	"C2",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CA",	"CT",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "CM",	"CT",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "F",	"CT",	"F",	SpeciesAngle::HarmonicForm,	644.336,	109.1 },
		{ this, "HC",	"CT",	"F",	SpeciesAngle::HarmonicForm,	334.72,		107.0 },
		{ this, "CT",	"C",	"C",	SpeciesAngle::HarmonicForm,	669.44,		117.2 },
		{ this, "C",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		121.4 },
		{ this, "C",	"C",	"N",	SpeciesAngle::HarmonicForm,	585.76,		116.6 },
		{ this, "C9",	"C8",	"SY",	SpeciesAngle::HarmonicForm,	585.76,		118.0 },
		{ this, "C8",	"SY",	"C3",	SpeciesAngle::HarmonicForm,	518.816,	98.9 },
		{ this, "OY",	"SY",	"N",	SpeciesAngle::HarmonicForm,	1004.16,	107.0 },
		{ this, "OY",	"SY",	"OY",	SpeciesAngle::HarmonicForm,	870.272,	119.0 },
		{ this, "OY",	"SZ",	"CT",	SpeciesAngle::HarmonicForm,	619.232,	107.0 },
		{ this, "OY",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	619.232,	108.9 },
		{ this, "OY",	"SY",	"CA",	SpeciesAngle::HarmonicForm,	619.232,	107.2 },
		{ this, "N",	"SY",	"CA",	SpeciesAngle::HarmonicForm,	836.8,		103.0 },
		{ this, "SY",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "SY",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "SZ",	"CT",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CT",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	102.0 },
		{ this, "CA",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	102.0 },
		{ this, "CR",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	102.0 },
		{ this, "CT",	"SZ",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	96.0 },
		{ this, "CT",	"CT",	"SY",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "CT",	"CT",	"SZ",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "N",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	836.8,		103.0 },
		{ this, "SY",	"N",	"CT",	SpeciesAngle::HarmonicForm,	418.4,		120.0 },
		{ this, "H",	"N",	"SY",	SpeciesAngle::HarmonicForm,	836.8,		111.0 },
		{ this, "OS",	"C",	"N",	SpeciesAngle::HarmonicForm,	677.808,	111.4 },
		{ this, "CT",	"NT",	"SY",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "C",	"CT",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "SY",	"CT",	"F",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "SY",	"NT",	"H",	SpeciesAngle::HarmonicForm,	292.88,		115.0 },
		{ this, "C",	"CW",	"NA",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "NT",	"C",	"CW",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "C",	"CW",	"CS",	SpeciesAngle::HarmonicForm,	711.28,		120.0 },
		{ this, "CB",	"CS",	"HA",	SpeciesAngle::HarmonicForm,	292.88,		120.0 },
		{ this, "CW",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "C",	"NT",	"CT",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "C",	"CT",	"C",	SpeciesAngle::HarmonicForm,	527.184,	111.1 },
		{ this, "C",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "N",	"CT",	"OS",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "NT",	"C",	"O",	SpeciesAngle::HarmonicForm,	669.44,		120.4 },
		{ this, "NT",	"C",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "CA",	"NT",	"C",	SpeciesAngle::HarmonicForm,	527.184,	112.0 },
		{ this, "CA",	"NT",	"SY",	SpeciesAngle::HarmonicForm,	418.4,		108.6 },
		{ this, "OY",	"SY",	"NT",	SpeciesAngle::HarmonicForm,	619.232,	108.9 },
		{ this, "NT",	"SY",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	102.0 },
		{ this, "NT",	"CT",	"S",	SpeciesAngle::HarmonicForm,	418.4,		114.7 },
		{ this, "HC",	"CY",	"NT",	SpeciesAngle::HarmonicForm,	292.88,		114.3 },
		{ this, "CY",	"CY",	"NT",	SpeciesAngle::HarmonicForm,	313.8,		117.2 },
		{ this, "CA",	"NT",	"CY",	SpeciesAngle::HarmonicForm,	418.4,		109.5 },
		{ this, "NC",	"CA",	"Cl",	SpeciesAngle::HarmonicForm,	627.6,		120.0 },
		{ this, "CA",	"NT",	"CA",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "NC",	"CA",	"NT",	SpeciesAngle::HarmonicForm,	585.76,		116.0 },
		{ this, "CM",	"CM",	"CY",	SpeciesAngle::HarmonicForm,	585.76,		124.0 },
		{ this, "CM",	"CY",	"HC",	SpeciesAngle::HarmonicForm,	292.88,		109.5 },
		{ this, "CM",	"CY",	"CY",	SpeciesAngle::HarmonicForm,	527.184,	114.0 },
		{ this, "C",	"CM",	"CY",	SpeciesAngle::HarmonicForm,	585.76,		119.7 },
		{ this, "N*",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NA",	"CM",	"CT",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "S",	"CM",	"CM",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "S",	"CM",	"N*",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "S",	"CM",	"NA",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "N*",	"CM",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "NA",	"CM",	"OS",	SpeciesAngle::HarmonicForm,	585.76,		120.0 },
		{ this, "CA",	"S",	"CM",	SpeciesAngle::HarmonicForm,	518.816,	104.2 },
		{ this, "CM",	"OS",	"CA",	SpeciesAngle::HarmonicForm,	627.6,		111.0 },
		{ this, "CM",	"CT",	"CA",	SpeciesAngle::HarmonicForm,	334.72,		109.5 },
		{ this, "S",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "P",	"CA",	"CA",	SpeciesAngle::HarmonicForm,	711.28,		119.4 },
		{ this, "CA",	"S",	"CT",	SpeciesAngle::HarmonicForm,	518.816,	104.2 },
		{ this, "C",	"N",	"Zn",	SpeciesAngle::HarmonicForm,	167.36,		126.0 },
		{ this, "HO",	"OH",	"Zn",	SpeciesAngle::HarmonicForm,	836.8,		126.0 },
		{ this, "N",	"Zn",	"N",	SpeciesAngle::HarmonicForm,	167.36,		109.5 },
		{ this, "N",	"Zn",	"O",	SpeciesAngle::HarmonicForm,	167.36,		109.5 }
	};

	static ForcefieldTorsionTerm torsionTerms[] =
	{
		//	i	j	k	l	Type (CosineForm)		k		n	eq	s
		{ this, "HC",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// hydrocarbon
		{ this, "HC",	"C",	"C",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// 
		{ this, "HC",	"Ceq",	"Ceq",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// 
		{ this, "CT",	"Ceq",	"Ceq",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// 
		{ this, "CT",	"Ceq",	"Ceq",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// 
		{ this, "H3",	"N3",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// ammonium
		{ this, "HC",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// hydrocarbon
		{ this, "HC",	"CT",	"CT",	"CO",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// acetal
		{ this, "HC",	"CO",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// acetal
		{ this, "CT",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	5.4392,	-0.2092,	0.8368,	0 },	// hydrocarbon
		{ this, "CT",	"CT",	"CT",	"CO",	SpeciesTorsion::Cos3Form,	5.4392,	-0.2092,	0.8368,	0 },	// hydrocarbon
		{ this, "HC",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	0,	0,	1.95811,	0 },	// alcohols,
		{ this, "HC",	"CT",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	0,	0,	1.95811,	0 },	// alcohols,
		{ this, "HC",	"CT",	"CO",	"OS",	SpeciesTorsion::Cos3Form,	0,	0,	1.95811,	0 },	// alcohols,
		{ this, "CT",	"CT",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	7.15882,	-2.092,	2.77399,	0 },	// alcohols,
		{ this, "CT",	"CT",	"CO",	"O*",	SpeciesTorsion::Cos3Form,	7.15882,	-2.092,	2.77399,	0 },	// alcohols,
		{ this, "HC",	"CM",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	0,	0,	1.95811,	0 },	// alcohols,
		{ this, "CT",	"CM",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	7.15882,	-2.092,	2.77399,	0 },	// alcohols,
		{ this, "CT",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	-6.49357,	0,	0,	0 },	// polyols
		{ this, "HC",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	1.47444,	0 },	// alcohols
		{ this, "CT",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-1.4895,	-0.728016,	2.05853,	0 },	// alcohols
		{ this, "OH",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	39.7815,	0,	0,	0 },	// diols
		{ this, "OH",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	51.1871,	0,	0,	0 },	// triols
		{ this, "HC",	"CT",	"OS",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "HC",	"CT",	"OS",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "HC",	"CT",	"OS",	"CM",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "HC",	"CM",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "HC",	"CO",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "HC",	"CT",	"OS",	"CO",	SpeciesTorsion::Cos3Form,	0,	0,	3.17984,	0 },	// ethers
		{ this, "CT",	"OS",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	2.7196,	-1.046,	2.80328,	0 },	// ethers
		{ this, "CT",	"OS",	"CM",	"CT",	SpeciesTorsion::Cos3Form,	2.7196,	-1.046,	2.80328,	0 },	// ethers
		{ this, "CT",	"OS",	"CO",	"CT",	SpeciesTorsion::Cos3Form,	2.7196,	-1.046,	2.80328,	0 },	// ethers
		{ this, "CT",	"OS",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	-2.17986,	-8.44331,	8.35126,	0 },	// acetals
		{ this, "NT",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	33.472,	0,	0,	0 },	// 2-aminoethanol
		{ this, "OS",	"CT",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	-2.3012,	0,	0,	0 },	// polyethers,
		{ this, "CT",	"OS",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	12.552,	0,	0 },	// anisole
		{ this, "CM",	"Ceq",	"Ceq",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// diene-
		{ this, "CM",	"Ceq",	"Ceq",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// diene-
		{ this, "CM",	"Ceq",	"Ceq",	"CM",	SpeciesTorsion::Cos3Form,	5.95383,	16.9661,	3.58987,	0 },	// diene
		{ this, "HC",	"CT",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// ethyl
		{ this, "H3",	"N3",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// anilinium
		{ this, "HC",	"CT",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CV",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CQ",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"CK",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "HC",	"CT",	"C*",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// ethyl
		{ this, "CT",	"N3",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// anilinium
		{ this, "CT",	"CT",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CV",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CQ",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"CK",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "CT",	"CT",	"C*",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "O*",	"CT",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// benzyl
		{ this, "C*",	"CT",	"NA",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "H*",	"CT",	"NA",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "C*",	"CT",	"N*",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "H*",	"CT",	"N*",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "O",	"C",	"CR",	"O*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "O",	"C",	"CR",	"N*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// heterocycles
		{ this, "CA",	"CA",	"CT",	"N*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aromatics
		{ this, "*",	"Zn",	"N",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// JACS
		{ this, "*",	"Zn",	"O",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// JACS
		{ this, "HC",	"CT",	"CT",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// ethyl
		{ this, "HC",	"CT",	"N3",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// anilinium
		{ this, "HC",	"CT",	"CT",	"CW",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CV",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CR",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CS",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CQ",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CU",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"CK",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "HC",	"CT",	"CT",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	1.93301,	0 },	// aromatics
		{ this, "CT",	"S",	"S",	"CT",	SpeciesTorsion::Cos3Form,	0,	-31.0202,	7.13372,	0 },	// disulfide
		{ this, "HC",	"CT",	"S",	"S",	SpeciesTorsion::Cos3Form,	0,	0,	2.33467,	0 },	// disulfide
		{ this, "CT",	"CT",	"S",	"S",	SpeciesTorsion::Cos3Form,	8.12114,	-3.49782,	3.91204,	0 },	// disulfide
		{ this, "HC",	"CT",	"SH",	"HS",	SpeciesTorsion::Cos3Form,	0,	0,	2.00832,	0 },	// thiol
		{ this, "CT",	"CT",	"SH",	"HS",	SpeciesTorsion::Cos3Form,	-3.17566,	-1.17989,	2.84512,	0 },	// thiol
		{ this, "HC",	"CT",	"CT",	"SH",	SpeciesTorsion::Cos3Form,	0,	0,	1.89117,	0 },	// thiol
		{ this, "HC",	"CT",	"CT",	"SY",	SpeciesTorsion::Cos3Form,	0,	0,	1.89117,	0 },	// 
		{ this, "HC",	"CT",	"CT",	"S",	SpeciesTorsion::Cos3Form,	0,	0,	1.89117,	0 },	// sulfide
		{ this, "CT",	"CT",	"CT",	"SH",	SpeciesTorsion::Cos3Form,	5.28021,	-0.828432,	1.94556,	0 },	// thiol
		{ this, "CT",	"CT",	"CT",	"SY",	SpeciesTorsion::Cos3Form,	5.28021,	-0.828432,	1.94556,	0 },	// (mod
		{ this, "HC",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// amine
		{ this, "HC",	"CT",	"CT",	"NT",	SpeciesTorsion::Cos3Form,	-4.23839,	-2.96646,	1.97903,	0 },	// amine
		{ this, "CT",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	-0.79496,	-1.74473,	1.74891,	0 },	// amine
		{ this, "CT",	"CT",	"N2",	"H",	SpeciesTorsion::Cos3Form,	-0.79496,	-1.74473,	1.74891,	0 },	// guanidinium
		{ this, "CT",	"CT",	"N2",	"H3",	SpeciesTorsion::Cos3Form,	-0.79496,	-1.74473,	1.74891,	0 },	// guanidinium
		{ this, "CT",	"CT",	"CT",	"NT",	SpeciesTorsion::Cos3Form,	10.0081,	-2.82002,	2.3012,	0 },	// amine
		{ this, "HO",	"OH",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	7.03749,	0,	0 },	// phenol
		{ this, "N",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	3.53548,	-4.02501,	2.98319,	0 },	// Chi-1
		{ this, "N",	"CT",	"CT",	"C*",	SpeciesTorsion::Cos3Form,	3.53548,	-4.02501,	2.98319,	0 },	// Chi-1
		{ this, "N",	"CT",	"CT",	"CA",	SpeciesTorsion::Cos3Form,	3.53548,	-4.02501,	2.98319,	0 },	// Chi-1
		{ this, "C",	"N",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-9.89516,	3.81581,	-3.5564,	0 },	// Phi
		{ this, "N",	"CT",	"C",	"N",	SpeciesTorsion::Cos3Form,	7.59814,	5.11285,	6.6149,	0 },	// Psi
		{ this, "C",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	1.93301,	0,	0 },	// Phi'
		{ this, "CT",	"CT",	"C",	"N",	SpeciesTorsion::Cos3Form,	4.90783,	0.790776,	-5.0208,	0 },	// Psi'
		{ this, "C",	"N",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// Phi"
		{ this, "HC",	"CT",	"C",	"N",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// Psi"
		{ this, "HC",	"CM",	"C",	"N",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "HC",	"CT",	"C",	"NM",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "HC",	"CM",	"C",	"NM",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "CQ",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "CQ",	"N",	"CT",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "H",	"N",	"CT",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "H",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "H",	"N",	"CT",	"C",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "*",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "N",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// peptides
		{ this, "CT",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// amides
		{ this, "CM",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// amides
		{ this, "HC",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// amides
		{ this, "CT",	"C",	"NM",	"CT",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// tert.
		{ this, "O",	"C",	"NM",	"CT",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// tert.
		{ this, "CT",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// amides-
		{ this, "CT",	"C",	"N",	"CA",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// 
		{ this, "HC",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// amides-
		{ this, "O",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// amides
		{ this, "O",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// amides
		{ this, "O",	"C",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// amides
		{ this, "O",	"C",	"N",	"CM",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// amides
		{ this, "N",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// imides
		{ this, "N",	"C",	"N",	"C",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// imides
		{ this, "CT",	"N",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tert.
		{ this, "CT",	"N2",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tert.
		{ this, "CA",	"N",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// "
		{ this, "CT",	"NM",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tert.
		{ this, "CA",	"NM",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// "
		{ this, "C*",	"CA",	"SH",	"HS",	SpeciesTorsion::Cos3Form,	0,	4.6024,	0,	0 },	// aromatic
		{ this, "CA",	"CA",	"SH",	"HS",	SpeciesTorsion::Cos3Form,	0,	4.6024,	0,	0 },	// aromatic
		{ this, "N*",	"CA",	"SH",	"HS",	SpeciesTorsion::Cos3Form,	-12.552,	0,	0,	0 },	// "
		{ this, "HC",	"CT",	"S",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	2.70705,	0 },	// sulfide
		{ this, "HC",	"CT",	"S",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	2.70705,	0 },	// sulfide
		{ this, "CT",	"CT",	"CT",	"S",	SpeciesTorsion::Cos3Form,	10.9579,	-2.59408,	1.07947,	0 },	// sulfide
		{ this, "CT",	"CT",	"S",	"CT",	SpeciesTorsion::Cos3Form,	3.8702,	-2.40998,	2.83257,	0 },	// sulfide
		{ this, "H",	"N2",	"CA",	"N2",	SpeciesTorsion::Cos3Form,	0,	16.3176,	0,	0 },	// guanidinium
		{ this, "H",	"N2",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	16.3176,	0,	0 },	// guanidinium
		{ this, "CT",	"N2",	"CA",	"N2",	SpeciesTorsion::Cos3Form,	0,	33.2042,	0,	0 },	// methylguanidinium
		{ this, "HC",	"CT",	"N2",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0.740568,	0 },	// methylguanidinium
		{ this, "HC",	"CT",	"N2",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// methylguanidinium
		{ this, "CT",	"CT",	"N2",	"CA",	SpeciesTorsion::Cos3Form,	7.65254,	1.01671,	-2.08363,	0 },	// ethylguanidinium
		{ this, "HC",	"CT",	"CT",	"N2",	SpeciesTorsion::Cos3Form,	0,	0,	-2.43509,	0 },	// ethylguanidinium
		{ this, "C",	"N",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	-0.581576,	0 },	// N-methylformamide
		{ this, "HC",	"CT",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// N-methylformamide
		{ this, "C",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-5.84086,	-1.78657,	0,	0 },	// N-ethylformamide
		{ this, "H",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// N-ethylformamide
		{ this, "N",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.94138,	0 },	// N-ethylformamide
		{ this, "N",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	8.21738,	0,	2.75726,	0 },	// N-propylformamide
		{ this, "C",	"NM",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	-0.581576,	0 },	// tertiary
		{ this, "C",	"NM",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-5.84086,	-1.78657,	0,	0 },	// tertiary
		{ this, "NM",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.94138,	0 },	// tertiary
		{ this, "NM",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	8.21738,	0,	2.75726,	0 },	// tertiary
		{ this, "CM",	"CM",	"C",	"NM",	SpeciesTorsion::Cos3Form,	8.368,	0,	0,	0 },	// tertiary
		{ this, "CM",	"CM",	"C",	"N",	SpeciesTorsion::Cos3Form,	8.368,	0,	0,	0 },	// vinyl
		{ this, "HC",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// all
		{ this, "HC",	"C",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "HC",	"CM",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 
		{ this, "CT",	"CT",	"C",	"N",	SpeciesTorsion::Cos3Form,	13.598,	-1.68197,	-0.569024,	0 },	// propanamide
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	4.87854,	0,	0 },	// propanamide
		{ this, "HC",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	0,	0,	-0.4184,	0 },	// all
		{ this, "C",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-8.61904,	-1.30959,	1.31796,	0 },	// butanamide
		{ this, "N",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	26.2755,	-6.13793,	8.49352,	0 },	// Chi
		{ this, "C",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	-25.8571,	0,	0,	0 },	// Chi
		{ this, "N",	"CT",	"CT",	"S*",	SpeciesTorsion::Cos3Form,	5.97475,	0.359824,	0.121336,	0 },	// Chi
		{ this, "C",	"CT",	"CT",	"S*",	SpeciesTorsion::Cos3Form,	-18.1753,	-7.17138,	0,	0 },	// Chi
		{ this, "HC",	"CT",	"C*",	"CW",	SpeciesTorsion::Cos3Form,	0,	0,	-2.00832,	0 },	// 3-methylindole
		{ this, "HC",	"CT",	"C*",	"CB",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 3-methylindole
		{ this, "CT",	"CT",	"C*",	"CW",	SpeciesTorsion::Cos3Form,	-2.98738,	0,	0,	0 },	// 3-ethylindole
		{ this, "CT",	"CT",	"C*",	"CB",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// 3-ethylindole
		{ this, "HC",	"CT",	"CC",	"N*",	SpeciesTorsion::Cos3Form,	0,	0,	1.7531,	0 },	// HID,
		{ this, "CT",	"CT",	"CC",	"N*",	SpeciesTorsion::Cos3Form,	9.89934,	-1.09621,	2.11292,	0 },	// "
		{ this, "HC",	"CT",	"N3",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	1.09202,	0 },	// ammonium
		{ this, "CT",	"CT",	"N3",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	1.45185,	0 },	// ammonium
		{ this, "HC",	"CT",	"CT",	"N3",	SpeciesTorsion::Cos3Form,	0,	0,	1.60666,	0 },	// ammonium
		{ this, "CT",	"CT",	"CT",	"N3",	SpeciesTorsion::Cos3Form,	11.4307,	-0.958136,	2.02924,	0 },	// ammonium
		{ this, "HC",	"CT",	"NO",	"ON",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// nitro
		{ this, "*T",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	3.43088,	0,	0 },	// carboxylate
		{ this, "*T",	"CT",	"C",	"O2",	SpeciesTorsion::Cos3Form,	0,	3.43088,	0,	0 },	// carboxylate
		{ this, "HC",	"CT",	"CT",	"NO",	SpeciesTorsion::Cos3Form,	0,	0,	-0.9414,	0 },	// nitroethane
		{ this, "CT",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-13.326,	-3.4518,	2.06271,	0 },	// carboxylate
		{ this, "HC",	"CT",	"C",	"O2",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// caboxylates
		{ this, "HC",	"CT",	"C",	"OH",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// acids
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	2.28446,	0,	0 },	// RCOOH
		{ this, "CA",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	2.28446,	0,	0 },	// RCOOH
		{ this, "CT",	"CT",	"C",	"OH",	SpeciesTorsion::Cos3Form,	4.184,	2.28446,	1.8828,	0 },	// RCOOH
		{ this, "NT",	"CT",	"C",	"OH",	SpeciesTorsion::Cos3Form,	22.0078,	3.43088,	0,	0 },	// neutral
		{ this, "HC",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-10.8324,	-4.69863,	1.12968,	0 },	// axial
		{ this, "C",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-2.3012,	0,	4.184,	0 },	// dicarboxylic
		{ this, "O",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	23.012,	0,	0 },	// carboxylic
		{ this, "CT",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	6.276,	23.012,	0,	0 },	// carboxylic
		{ this, "HC",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	6.276,	23.012,	0,	0 },	// carboxylic
		{ this, "C",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0.309616,	0 },	// dicarboxylic
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	-3.138,	-2.3012,	-1.046,	0 },	// dicarboxylic
		{ this, "CT",	"CT",	"C",	"OH",	SpeciesTorsion::Cos3Form,	0,	5.90781,	0,	0 },	// dicarboxylic
		{ this, "CT",	"N",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-7.26761,	5.23418,	-14.6482,	0 },	// Proline
		{ this, "CT",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	19.8866,	-3.07106,	0,	0 },	// "
		{ this, "CT",	"CT",	"N",	"CT",	SpeciesTorsion::Cos3Form,	11.9621,	8.61067,	-47.1369,	0 },	// "
		{ this, "CT",	"NM",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-7.26761,	5.23418,	-14.6482,	0 },	// 
		{ this, "CT",	"NM",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	19.8866,	-3.07106,	0,	0 },	// 
		{ this, "CT",	"CT",	"NM",	"CT",	SpeciesTorsion::Cos3Form,	11.9621,	8.61067,	-47.1369,	0 },	// 
		{ this, "CT",	"CT",	"C+",	"CT",	SpeciesTorsion::Cos3Form,	0,	-4.184,	0,	0 },	// carbocation
		{ this, "CT",	"CT",	"C+",	"HC",	SpeciesTorsion::Cos3Form,	0,	-4.184,	0,	0 },	// carbocation
		{ this, "HC",	"CT",	"C",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.1506,	0 },	// ketone
		{ this, "HC",	"CT",	"C",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.50624,	0 },	// aldehyde
		{ this, "HC",	"CT",	"C",	"F",	SpeciesTorsion::Cos3Form,	0,	0,	1.50624,	0 },	// acyl
		{ this, "HC",	"CT",	"C",	"Cl",	SpeciesTorsion::Cos3Form,	0,	0,	1.50624,	0 },	// acyl
		{ this, "HC",	"CT",	"C",	"Br",	SpeciesTorsion::Cos3Form,	0,	0,	1.50624,	0 },	// acyl
		{ this, "CT",	"CT",	"C",	"F",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// acyl
		{ this, "CT",	"CT",	"C",	"Cl",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// acyl
		{ this, "CT",	"CT",	"C",	"Br",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// acyl
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	-1.15897,	5.13795,	-2.9037,	0 },	// aldehyde
		{ this, "HC",	"CT",	"CT",	"C(O)",	SpeciesTorsion::Cos3Form,	0,	0,	-0.317984,	0 },	// aldehyde
		{ this, "CT",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-7.10025,	-1.9079,	2.44764,	0 },	// aldehyde
		{ this, "CA",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-7.10025,	-1.9079,	2.44764,	0 },	// aldehyde
		{ this, "C*",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	-7.10025,	-1.9079,	2.44764,	0 },	// 
		{ this, "CT",	"CT",	"C",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// aldehyde
		{ this, "CT",	"CT",	"C",	"CT",	SpeciesTorsion::Cos3Form,	6.08354,	-0.602496,	-3.2426,	0 },	// ketone
		{ this, "C",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	12.552,	23.012,	0,	0 },	// oxalic
		{ this, "CA",	"CT",	"P",	"OS",	SpeciesTorsion::Cos3Form,	9.414,	0,	0,	0 },	// phosphonates
		{ this, "CT",	"P",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	14.644,	-13.8072,	6.276,	0 },	// phosphonates
		{ this, "P",	"OS",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// phosphonates
		{ this, "O2",	"P",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "O",	"P",	"OS",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "O",	"P",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "OH",	"P",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "CA",	"CA",	"CT",	"P",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "CA",	"CT",	"P",	"O2",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// phosphonates
		{ this, "HC",	"CT",	"P",	"O2",	SpeciesTorsion::Cos3Form,	0,	0,	1.046,	0 },	// phosphonates
		{ this, "HC",	"CT",	"P",	"OS",	SpeciesTorsion::Cos3Form,	0,	0,	1.046,	0 },	// phosphonates
		{ this, "O2",	"P",	"OS",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	2.35141,	0 },	// MeOPO3
		{ this, "O2",	"P",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	3.7656,	-12.2591,	11.0458,	0 },	// dimethyl
		{ this, "CA",	"CA",	"OS",	"P",	SpeciesTorsion::Cos3Form,	0,	12.5102,	0,	0 },	// PhOPO3
		{ this, "NT",	"CT",	"CT",	"Cl",	SpeciesTorsion::Cos3Form,	8.368,	0,	0,	0 },	// 2-chloroethylamines
		{ this, "HC",	"CT",	"CT",	"Cl",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "CT",	"CT",	"CT",	"Cl",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "Cl",	"CT",	"CT",	"Cl",	SpeciesTorsion::Cos3Form,	-1.046,	0,	0,	0 },	// dichloride
		{ this, "HC",	"CT",	"CT",	"I",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "CT",	"CT",	"CT",	"I",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "HO",	"OH",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	18.736,	-9.09853,	0,	0 },	// trifluoroethanol
		{ this, "HC",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	1.99158,	0 },	// trifluoroethanol
		{ this, "F",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.31252,	0 },	// trifluoroethanol
		{ this, "F",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	0,	0,	2.25978,	0 },	// trifluoroethanol
		{ this, "CT",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	13.3888,	20.5016,	0,	0 },	// 1,2-diacid
		{ this, "CT",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	-4.184,	-7.9496,	-3.7656,	0 },	// 1,2-diacid
		{ this, "C",	"CT",	"CT",	"C",	SpeciesTorsion::Cos3Form,	3.3472,	0,	3.7656,	0 },	// 1,2-diacid
		{ this, "H",	"N",	"SY",	"CA",	SpeciesTorsion::Cos3Form,	6.99146,	-20.5058,	2.7991,	0 },	// sulfonamide
		{ this, "HC",	"CT",	"N",	"SY",	SpeciesTorsion::Cos3Form,	5.69861,	-6.09609,	0.623416,	0 },	// sulfonamide
		{ this, "CT",	"N",	"SY",	"CA",	SpeciesTorsion::Cos3Form,	8.67762,	-12.4097,	10.347,	0 },	// sulfonamide
		{ this, "CT",	"CT",	"N",	"SY",	SpeciesTorsion::Cos3Form,	12.2549,	-10.5981,	2.07945,	0 },	// sulfonamide
		{ this, "CA",	"CA",	"SY",	"N",	SpeciesTorsion::Cos3Form,	6.9287,	-3.21331,	-0.489528,	0 },	// sulfonamide
		{ this, "*",	"*",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfonamide
		{ this, "*",	"CA",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfonamide
		{ this, "*",	"N",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfonamide
		{ this, "*",	"CT",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfonamide
		{ this, "*",	"CT",	"SY",	"N",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfonamide
		{ this, "HC",	"CT",	"CT",	"F",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "CT",	"CT",	"CT",	"F",	SpeciesTorsion::Cos3Form,	1.2552,	-1.6736,	1.6736,	0 },	// alkyl
		{ this, "F",	"CT",	"CT",	"F",	SpeciesTorsion::Cos3Form,	-10.46,	0,	1.046,	0 },	// 1,2-difluoride
		{ this, "CT",	"CT",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	-5.58982,	0,	0,	0 },	// hexopyranoses
		{ this, "CT",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	11.188,	-12.0625,	4.29278,	0 },	// hexopyranoses
		{ this, "OH",	"CT",	"CT",	"OH",	SpeciesTorsion::Cos3Form,	37.9321,	0,	0,	0 },	// hexopyranoses
		{ this, "CT",	"OS",	"CO",	"OH",	SpeciesTorsion::Cos3Form,	-1.569,	-5.68187,	0.016736,	0 },	// hexopyranoses
		{ this, "CT",	"OS",	"CO",	"OS",	SpeciesTorsion::Cos3Form,	-1.569,	-5.68187,	0.016736,	0 },	// hexopyranoses
		{ this, "OS",	"CO",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-5.25929,	-7.5563,	0.012552,	0 },	// hexopyranoses
		{ this, "OH",	"CT",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	18.0707,	0,	0,	0 },	// hexopyranoses
		{ this, "HC",	"CT",	"CT",	"Br",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "CT",	"CT",	"CT",	"Br",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "CA",	"CA",	"CT",	"Br",	SpeciesTorsion::Cos3Form,	0,	0,	1.6736,	0 },	// alkyl
		{ this, "*",	"CA",	"CA",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// in
		{ this, "CA",	"CA",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"C!",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"C!",	"C!",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"C!",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"CA",	"CA",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"C!",	"C!",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"C!",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"C!",	"NC",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"C!",	"NC",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"C!",	"CA",	"NC",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"C!",	"CA",	"NC",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"CA",	"CA",	"NC",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"C!",	"NC",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"C!",	"N",	"C",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C!",	"C!",	"CM",	"C",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NC",	"C!",	"CA",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"F",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"Cl",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"Br",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"I",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"F",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"Cl",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"Br",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"I",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"Ceq",	"Ceq",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C",	"Ceq",	"Ceq",	"C",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"CT",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"CA",	"CA",	"CT",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"CA",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"CB",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"CB",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"O*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"O*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"CA",	"CA",	"O*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "O*",	"CA",	"CA",	"O*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"CA",	"CA",	"S*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"S*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"CA",	"CA",	"S*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "S*",	"CA",	"CA",	"S*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CM",	"C!",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CM",	"C!",	"N",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"CA",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "O*",	"CA",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "N*",	"CA",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CB",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"CB",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "N*",	"CB",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "N*",	"CB",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CB",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CS",	"C",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CR",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CS",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"CS",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"CS",	"CW",	"H*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"CS",	"CW",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CS",	"CW",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CS",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CS",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CS",	"CS",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CA",	"CU",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CW",	"OS",	"CW",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NC",	"CA",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CS",	"CW",	"OS",	"CW",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"NC",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NC",	"CA",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"NC",	"CA",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"NC",	"NC",	"CT",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CA",	"NC",	"NC",	"CA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CT",	"NC",	"NC",	"CT",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NC",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NC",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NC",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NA",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NB",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NB",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NB",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NA",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NA",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NB",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "N*",	"CR",	"S",	"CW",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CR",	"NB",	"CV",	"CW",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CR",	"NB",	"CV",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CU",	"CW",	"S",	"CR",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CR",	"S",	"CW",	"CV",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CW",	"S",	"CR",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CW",	"S",	"CR",	"NB",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "S",	"CR",	"NB",	"CU",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "S",	"CR",	"NB",	"CV",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CR",	"NB",	"CU",	"CW",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CU",	"CW",	"OS",	"CR",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "CW",	"OS",	"CR",	"NB",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "OS",	"CR",	"NB",	"CU",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CU",	"CW",	"OS",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CU",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CU",	"CW",	"S",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CV",	"CW",	"OS",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CV",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "NB",	"CV",	"CW",	"S",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"N",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CU",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "HA",	"CW",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NC",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"NC",	"CQ",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NC",	"CQ",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"NC",	"CQ",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"N",	"CQ",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"N",	"CQ",	"N*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "C*",	"N",	"CQ",	"HA",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "H",	"N",	"CQ",	"*",	SpeciesTorsion::Cos3Form,	0,	30.334,	0,	0 },	// 
		{ this, "*",	"CW",	"NA",	"*",	SpeciesTorsion::Cos3Form,	0,	11.7152,	0,	0 },	// 
		{ this, "*",	"NA",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	19.4556,	0,	0 },	// 
		{ this, "HA",	"CR",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "N*",	"CR",	"NB",	"C*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CR",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CW",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CR",	"NA",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "HA",	"CR",	"NA",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "N*",	"CR",	"NA",	"C*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CR",	"NC",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CK",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CK",	"NA",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"CK",	"NC",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"NA",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"NB",	"NB",	"*",	SpeciesTorsion::Cos3Form,	0,	41.84,	0,	0 },	// 
		{ this, "*",	"NB",	"CV",	"*",	SpeciesTorsion::Cos3Form,	0,	20.0832,	0,	0 },	// 
		{ this, "*",	"CW",	"CV",	"*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "C*",	"CW",	"CV",	"C*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "C*",	"CW",	"CV",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "HA",	"CW",	"CV",	"C*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "HA",	"CW",	"CV",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "*",	"CW",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "C*",	"CW",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "C*",	"CW",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "H*",	"CW",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "H*",	"CW",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "HA",	"CW",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "S",	"CW",	"CW",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "S",	"CW",	"CU",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "S",	"CW",	"CV",	"HA",	SpeciesTorsion::Cos3Form,	0,	44.978,	0,	0 },	// 
		{ this, "*",	"NA",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "C*",	"NA",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "N*",	"NA",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "C*",	"NA",	"CW",	"H*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "N*",	"NA",	"CW",	"H*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "H",	"NA",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "H",	"NA",	"CR",	"*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "H",	"NA",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// 
		{ this, "*",	"C*",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	54.6012,	0,	0 },	// 
		{ this, "*",	"C*",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	14.0164,	0,	0 },	// 
		{ this, "*",	"CA",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "C*",	"CA",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "N*",	"CA",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "C*",	"CA",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "N*",	"CA",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "*",	"C",	"CB",	"*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "O",	"C",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "N*",	"C",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "O",	"C",	"CB",	"N*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "N*",	"C",	"CB",	"C*",	SpeciesTorsion::Cos3Form,	0,	29.288,	0,	0 },	// 
		{ this, "*",	"CB",	"CN",	"*",	SpeciesTorsion::Cos3Form,	0,	25.104,	0,	0 },	// 
		{ this, "*",	"NA",	"CN",	"*",	SpeciesTorsion::Cos3Form,	0,	12.7612,	0,	0 },	// 
		{ this, "*",	"CW",	"NA",	"*",	SpeciesTorsion::Cos3Form,	0,	12.552,	0,	0 },	// 
		{ this, "CT",	"CT",	"OS",	"P",	SpeciesTorsion::Cos3Form,	-5.94128,	-2.59408,	0.4184,	0 },	// in
		{ this, "HC",	"CT",	"C",	"OS",	SpeciesTorsion::Cos3Form,	0,	0,	0.552288,	0 },	// esters
		{ this, "CT",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	19.5351,	21.4388,	0,	0 },	// esters
		{ this, "HC",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	19.5351,	21.4388,	0,	0 },	// esters
		{ this, "O",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	21.4388,	0,	0 },	// esters
		{ this, "C",	"OS",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0.828432,	0 },	// esters
		{ this, "CT",	"CT",	"C",	"OS",	SpeciesTorsion::Cos3Form,	0,	0,	-2.31375,	0 },	// esters
		{ this, "CT",	"CT",	"OS",	"C",	SpeciesTorsion::Cos3Form,	-5.10448,	-0.527184,	1.76565,	0 },	// esters
		{ this, "CA",	"CT",	"CT",	"N3",	SpeciesTorsion::Cos3Form,	4.184,	0,	0,	0 },	// phenethylammonium-
		{ this, "CA",	"CT",	"CT",	"NT",	SpeciesTorsion::Cos3Form,	-3.3472,	0,	0,	0 },	// phenethylamines-
		{ this, "CT",	"CT",	"NO",	"ON",	SpeciesTorsion::Cos3Form,	0,	1.6736,	0,	0 },	// nitroethane
		{ this, "CA",	"CA",	"NO",	"ON",	SpeciesTorsion::Cos3Form,	0,	4.8116,	0,	0 },	// nitrobenzene
		{ this, "CT",	"NY",	"CA",	"NZ",	SpeciesTorsion::Cos3Form,	0,	15.2758,	0,	0 },	// neutral
		{ this, "CT",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	16.736,	0,	0 },	// azetidine-
		{ this, "CT",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0.8368,	-1.74473,	1.74891,	0 },	// pyrrolidine
		{ this, "CT",	"NT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	1.74054,	-0.535552,	2.90788,	0 },	// amine
		{ this, "CT",	"NT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	6.42662,	-0.535552,	2.90788,	0 },	// exocyclic
		{ this, "CT",	"NT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	6.12538,	-0.535552,	2.90788,	0 },	// exocyclic
		{ this, "CT",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	3.4267,	-1.74473,	1.74891,	0 },	// cyclic
		{ this, "CT",	"CT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	6.36805,	-1.74473,	1.74891,	0 },	// cyclic
		{ this, "NT",	"CT",	"CT",	"NT",	SpeciesTorsion::Cos3Form,	46.1704,	-4.05011,	1.12968,	0 },	// amine
		{ this, "CT",	"NT",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	1.74054,	-0.535552,	2.90788,	0 },	// amine
		{ this, "HC",	"CT",	"NT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	2.34304,	0 },	// amine
		{ this, "CT",	"CT",	"CW",	"NA",	SpeciesTorsion::Cos3Form,	7.1128,	-2.5104,	0,	0 },	// 2-ethyl
		{ this, "CT",	"Ceq",	"Ceq",	"CM",	SpeciesTorsion::Cos3Form,	3.7656,	0.96232,	-2.11292,	0 },	// 2-Me-1,3-butadiene
		{ this, "CT",	"Ceq",	"C",	"O*",	SpeciesTorsion::Cos3Form,	3.7656,	0.96232,	-2.11292,	0 },	// 2-Me-1,3-butadiene-like
		{ this, "CT",	"CM",	"C",	"O*",	SpeciesTorsion::Cos3Form,	3.7656,	0.96232,	-2.11292,	0 },	// 2-Me-1,3-butadiene-like
		{ this, "CT",	"C",	"Ceq",	"CM",	SpeciesTorsion::Cos3Form,	3.3472,	-12.552,	0,	0 },	// methyl
		{ this, "CM",	"Ceq",	"C",	"OH",	SpeciesTorsion::Cos3Form,	13.3888,	-12.552,	0,	0 },	// acrylic
		{ this, "CM",	"CM",	"C",	"OH",	SpeciesTorsion::Cos3Form,	13.3888,	-12.552,	0,	0 },	// acrylic
		{ this, "CM",	"Ceq",	"C",	"O",	SpeciesTorsion::Cos3Form,	10.46,	25.104,	0,	0 },	// acrolein
		{ this, "CM",	"CM",	"C",	"O",	SpeciesTorsion::Cos3Form,	10.46,	25.104,	0,	0 },	// acrolein-like
		{ this, "CA",	"CA",	"C",	"CT",	SpeciesTorsion::Cos3Form,	0,	0.8368,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"C",	"HC",	SpeciesTorsion::Cos3Form,	0,	0.8368,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"N",	"*",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// N-phenylamide
		{ this, "CA",	"CA",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// N-phenylamide
		{ this, "CA",	"CA",	"N",	"C",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// N-phenylamide
		{ this, "CA",	"CA",	"N",	"CT",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// N-phenylamide
		{ this, "CA",	"CA",	"N",	"CR",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"N",	"CW",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"NT",	"CR",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"NT",	"CW",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CR",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CR",	"NT",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "NB",	"CR",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "NB",	"CR",	"NT",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CR",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CR",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "NB",	"CR",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "NB",	"CR",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "OS",	"CR",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "OS",	"CR",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "OS",	"CW",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "OS",	"CW",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CW",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "S",	"CW",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"N",	"CQ",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"NT",	"CQ",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// diarylamine
		{ this, "CA",	"CA",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"C",	"OS",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"C",	"OH",	SpeciesTorsion::Cos3Form,	0,	8.7864,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"OS",	"C",	SpeciesTorsion::Cos3Form,	0,	10.46,	0,	0 },	// phenyl
		{ this, "O",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// benzoic
		{ this, "O",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// benzoic
		{ this, "CA",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	16.736,	20.92,	0,	0 },	// benzoic
		{ this, "CA",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	16.736,	20.92,	0,	0 },	// benzoic
		{ this, "O",	"C",	"OS",	"CA",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 },	// phenyl
		{ this, "CT",	"C",	"OS",	"CA",	SpeciesTorsion::Cos3Form,	6.276,	20.92,	0,	0 },	// phenyl
		{ this, "CA",	"CA",	"C",	"N",	SpeciesTorsion::Cos3Form,	0,	4.6024,	0,	0 },	// aryl
		{ this, "CA",	"CA",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline
		{ this, "*",	"CA",	"N2",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "*",	"CQ",	"N2",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "N*",	"CA",	"N2",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "CB",	"CA",	"N2",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "S",	"CR",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "OS",	"CR",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	8.49352,	0,	0 },	// aniline-like
		{ this, "CA",	"CA",	"NT",	"CT",	SpeciesTorsion::Cos3Form,	-31.7231,	14.3553,	13.3804,	0 },	// substituted-aniline
		{ this, "CT",	"CM",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	11.7863,	-0.707096,	2.27191,	0 },	// alkenes
		{ this, "CM",	"CM",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	1.44766,	1.69452,	-3.78234,	0 },	// alkenes
		{ this, "Ceq",	"CM",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	1.44766,	1.69452,	-3.78234,	0 },	// alkenes
		{ this, "CM",	"CM",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	5.19234,	14.029,	-1.19662,	0 },	// styrene
		{ this, "CT",	"CM",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0.85772,	-2.2217,	0,	0 },	// 1-methylstyrene
		{ this, "Z",	"CM",	"X",	"Y",	SpeciesTorsion::Cos3Form,	0,	125.52,	0,	0 },	// improper
		{ this, "*",	"CM",	"CM",	"*",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "CT",	"CM",	"CM",	"CT",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "CT",	"CM",	"Ceq",	"CT",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "CT",	"CM",	"Ceq",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "HC",	"CM",	"Ceq",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "HC",	"CM",	"Ceq",	"CT",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "CT",	"CM",	"CM",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "C",	"CM",	"CM",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "HC",	"CM",	"CM",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "HC",	"CM",	"Ceq",	"Ceq",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// alkene
		{ this, "Cl",	"CM",	"CM",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// chloroalkene
		{ this, "HC",	"CM",	"CM",	"OS",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// vinyl
		{ this, "CT",	"CM",	"CM",	"OS",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// vinyl
		{ this, "HC",	"CM",	"CM",	"OH",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// vinyl
		{ this, "CT",	"CM",	"CM",	"OH",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// vinyl
		{ this, "C",	"CM",	"CM",	"N",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// conj.
		{ this, "CT",	"C",	"NC",	"CT",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// imine
		{ this, "HC",	"C",	"NC",	"HC",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// imine
		{ this, "CT",	"C",	"NC",	"O*",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// oxime
		{ this, "HC",	"C",	"NC",	"O*",	SpeciesTorsion::Cos3Form,	0,	58.576,	0,	0 },	// oxime
		{ this, "CA",	"C!",	"C!",	"CA",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl
		{ this, "CA",	"C!",	"C!",	"NC",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"C",	"CA",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl
		{ this, "CA",	"C!",	"NA",	"NB",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"NA",	"CW",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CW",	"NA",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CW",	"OS",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CM",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CU",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CU",	"NB",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CV",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CV",	"NB",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CR",	"OS",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CR",	"NA",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CR",	"NB",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CW",	"S",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CR",	"S",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"N",	"C",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"N",	"CM",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NC",	"C!",	"N",	"C",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NC",	"C!",	"N",	"CM",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CS",	"CS",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CS",	"CS",	"CS",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "OS",	"CS",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "OS",	"CS",	"CS",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NA",	"CS",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NA",	"CS",	"CS",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CW",	"CV",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CW",	"CV",	"CS",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NB",	"CV",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NB",	"CV",	"CS",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "N*",	"CR",	"CS",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "N*",	"CR",	"CW",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"N",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"N",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"N",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"CM",	"CU",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"CM",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "*",	"CM",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NC",	"C!",	"CU",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "NC",	"C!",	"CU",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CU",	"C*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "CA",	"C!",	"CU",	"N*",	SpeciesTorsion::Cos3Form,	0,	9.07928,	0,	0 },	// biphenyl-like
		{ this, "Cl",	"CM",	"CM",	"Cl",	SpeciesTorsion::Cos3Form,	-6.6944,	58.576,	0,	0 },	// chloroalkene
		{ this, "HC",	"CT",	"P+",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// phosphonium
		{ this, "HC",	"CT",	"CT",	"P+",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// "
		{ this, "CT",	"CT",	"P+",	"CT",	SpeciesTorsion::Cos3Form,	4.184,	-2.092,	2.092,	0 },	// "
		{ this, "CK",	"N*",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	13.1043,	-6.23834,	11.4809,	0 },	// Ping
		{ this, "CK",	"NA",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	11.5311,	-3.64845,	-15.3971,	0 },	// Ping
		{ this, "C",	"NC",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	12.552,	12.552,	0,	0 },	// oxime
		{ this, "C",	"NC",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	12.552,	12.552,	0,	0 },	// oxime
		{ this, "CM",	"CM",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	-14.644,	12.552,	0,	0 },	// vinyl
		{ this, "Ceq",	"CM",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	-14.644,	12.552,	0,	0 },	// vinyl
		{ this, "CM",	"CM",	"CT",	"F",	SpeciesTorsion::Cos3Form,	2.092,	0,	0,	0 },	// allyl
		{ this, "CM",	"CM",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	2.092,	0,	0,	0 },	// allyl
		{ this, "Ceq",	"CM",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	2.092,	0,	0,	0 },	// allyl
		{ this, "CM",	"Ceq",	"CT",	"O*",	SpeciesTorsion::Cos3Form,	2.092,	0,	0,	0 },	// allyl
		{ this, "CM",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-3.7656,	0,	0,	0 },	// allyl
		{ this, "CA",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-3.7656,	0,	0,	0 },	// benzyl
		{ this, "Ceq",	"CT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-3.7656,	0,	0,	0 },	// allyl
		{ this, "HC",	"CT",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	1.4644,	0 },	// sulfone
		{ this, "HC",	"CT",	"SY",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.4644,	0 },	// sulfone
		{ this, "HC",	"CT",	"SY",	"CA",	SpeciesTorsion::Cos3Form,	0,	0,	1.4644,	0 },	// sulfone
		{ this, "CT",	"CT",	"SY",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfone
		{ this, "CT",	"CT",	"SY",	"OY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// sulfone
		{ this, "HC",	"CT",	"CT",	"SY",	SpeciesTorsion::Cos3Form,	0,	0,	1.4644,	0 },	// sulfone
		{ this, "HC",	"CT",	"N3",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.26231,	0 },	// 2ary
		{ this, "CT",	"CT",	"N3",	"CT",	SpeciesTorsion::Cos3Form,	6.01617,	-0.517979,	1.10416,	0 },	// 2ary
		{ this, "*",	"CT",	"CZ",	"NZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// nitriles
		{ this, "*",	"CT",	"NZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// isonitriles
		{ this, "HC",	"CT",	"NZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// isonitriles
		{ this, "*",	"CA",	"NZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// isonitriles
		{ this, "CA",	"CA",	"NZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// isonitriles
		{ this, "*",	"CZ",	"CZ",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "*",	"CT",	"CZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "CT",	"CT",	"CZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "HC",	"CT",	"CZ",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "HC",	"CZ",	"CZ",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "CT",	"CZ",	"CZ",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// alkynes
		{ this, "CA",	"CA",	"SY",	"CT",	SpeciesTorsion::Cos3Form,	0,	-3.7656,	0,	0 },	// sulfone
		{ this, "CR",	"NA",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	-6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "CR",	"N*",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	-6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "CK",	"NA",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	-6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "CK",	"N*",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	-6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "C*",	"NA",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "C*",	"N*",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	6.276,	-6.276,	0,	0 },	// imidazoles,
		{ this, "CR",	"NA",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "CR",	"N*",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "CK",	"NA",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "CK",	"N*",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	-4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "C*",	"NA",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "C*",	"N*",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	4.184,	-1.4644,	0,	0 },	// imidazoles,
		{ this, "N*",	"CT",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	-2.092,	-6.276,	4.184,	0 },	// imidazoles,
		{ this, "C*",	"NA",	"CT",	"OS",	SpeciesTorsion::Cos3Form,	0,	-7.84918,	0,	0 },	// Ping
		{ this, "C*",	"NA",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	0,	-2.40998,	0,	0 },	// Ping
		{ this, "*",	"CY",	"CY",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "*",	"CY",	"Ndoll",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "*",	"CY",	"Cdoll",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"CY",	"Cdoll",	"Ndoll",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"CY",	"Cdoll",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "*",	"CY",	"S",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"CY",	"Ndoll",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"Ndoll",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"Cdoll",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"Cdoll",	"Ndoll",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "N",	"CY",	"CY",	"*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"CY",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "Cdoll",	"CY",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"N",	"C",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "C*",	"Ndoll",	"CT",	"C*",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "C*",	"Ndoll",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"Cdoll",	"Ndoll",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// small
		{ this, "CY",	"Cdoll",	"Ndoll",	"CY",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// small
		{ this, "CY",	"N",	"CT",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// small
		{ this, "CY",	"Cdoll",	"Ndoll",	"CA",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// small
		{ this, "O",	"Cdoll",	"Ndoll",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// small
		{ this, "CY",	"Cdoll",	"Ndoll",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// small
		{ this, "O",	"Cdoll",	"Ndoll",	"CT",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// small
		{ this, "O",	"Cdoll",	"Ndoll",	"CA",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// small
		{ this, "O",	"C",	"N",	"CY",	SpeciesTorsion::Cos3Form,	0,	25.4764,	0,	0 },	// small
		{ this, "C",	"N",	"CY",	"CY",	SpeciesTorsion::Cos3Form,	-5.84086,	-1.78657,	0,	0 },	// small
		{ this, "C",	"N",	"CY",	"Cdoll",	SpeciesTorsion::Cos3Form,	-5.84086,	-1.78657,	0,	0 },	// small
		{ this, "O",	"Cdoll",	"Ndoll",	"CY",	SpeciesTorsion::Cos3Form,	0,	83.68,	0,	0 },	// small
		{ this, "HC",	"CY",	"CY",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"CY",	"CY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CY",	"CY",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CT",	"CY",	"CY",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "CY",	"CY",	"CY",	"CY",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// small
		{ this, "HC",	"CT",	"CY",	"CY",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// small
		{ this, "HC",	"CT",	"CM",	"CM",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// alkenes
		{ this, "HC",	"CT",	"CM",	"Ceq",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// alkenes
		{ this, "HC",	"CT",	"Ceq",	"Ceq",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// alkenes
		{ this, "HC",	"CT",	"Ceq",	"CM",	SpeciesTorsion::Cos3Form,	0,	0,	-1.55645,	0 },	// alkenes
		{ this, "HC",	"CM",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	1.33051,	0 },	// alkene
		{ this, "HC",	"CT",	"CT",	"CM",	SpeciesTorsion::Cos3Form,	0,	0,	1.53134,	0 },	// alkene
		{ this, "HC",	"CT",	"CT",	"CZ",	SpeciesTorsion::Cos3Form,	0,	0,	1.53134,	0 },	// alkyne,
		{ this, "CT",	"CT",	"CT",	"CZ",	SpeciesTorsion::Cos3Form,	0,	-2.7196,	0,	0 },	// alkyne,
		{ this, "HC",	"CT",	"CM",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// alkenes
		{ this, "CT",	"NT",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "CT",	"NT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "H",	"NT",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "H",	"NT",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "CT",	"NT",	"NT",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "CT",	"NT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "H",	"NT",	"NT",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	1.2552,	0 },	// generic
		{ this, "OS",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// carbamates
		{ this, "OH",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	9.6232,	25.4764,	0,	0 },	// carbamates
		{ this, "OS",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// carbamates
		{ this, "OH",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// carbamates
		{ this, "N",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	-8.368,	20.92,	0,	0 },	// carbamates
		{ this, "N",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	-8.368,	20.92,	0,	0 },	// carbamates
		{ this, "HC",	"NC",	"NZ",	"NZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// azides
		{ this, "CT",	"NC",	"NZ",	"NZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// azides
		{ this, "CT",	"CT",	"NC",	"NZ",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// azides
		/* { this, "HC",	"CT",	"C",	"Seq",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 }, */	// thiocarbonyl
		/* { this, "Seq",	"C",	"N",	"H",	SpeciesTorsion::Cos3Form,	4.184,	20.92,	0,	0 }, */	// thioamides
		/* { this, "Seq",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */	// thioamides
		/* { this, "Seq",	"C",	"N",	"CA",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */	// thioamides
		/* { this, "Seq",	"C",	"N",	"CM",	SpeciesTorsion::Cos3Form,	0,	27.196,	0,	0 }, */	// thioamides
		/* { this, "Seq",	"C",	"OS",	"CT",	SpeciesTorsion::Cos3Form,	0,	20.92,	0,	0 }, */	// thioesters
		/* { this, "Seq",	"C",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	0,	23.012,	0,	0 }, */	// thioacids
		{ this, "CT",	"S",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	2.5104,	0,	0 },	// thioanisole
		{ this, "O",	"C",	"C",	"CT",	SpeciesTorsion::Cos3Form,	0,	2.092,	0,	0 },	// dicarbonyls
		{ this, "O",	"C",	"C",	"HC",	SpeciesTorsion::Cos3Form,	0,	0.8368,	0,	0 },	// dicarbonyls
		{ this, "HC",	"C",	"C",	"CT",	SpeciesTorsion::Cos3Form,	3.3472,	-3.17984,	0,	0 },	// dicarbonyls
		{ this, "HC",	"C",	"C",	"HC",	SpeciesTorsion::Cos3Form,	3.3472,	0,	0,	0 },	// dicarbonyls
		{ this, "CT",	"C",	"C",	"CT",	SpeciesTorsion::Cos3Form,	2.9288,	-6.276,	0,	0 },	// dicarbonyls
		{ this, "C",	"C",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0.35564,	0 },	// dicarbonyls
		{ this, "N",	"C",	"C",	"O",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// dicarbonyls
		{ this, "N",	"C",	"C",	"HC",	SpeciesTorsion::Cos3Form,	-3.7656,	1.2552,	0,	0 },	// dicarbonyls
		{ this, "N",	"C",	"C",	"CT",	SpeciesTorsion::Cos3Form,	-2.092,	0.8368,	0,	0 },	// dicarbonyls
		{ this, "C",	"C",	"N",	"CT",	SpeciesTorsion::Cos3Form,	1.6736,	20.5016,	0,	0 },	// dicarbonyls
		{ this, "H",	"N",	"C",	"C",	SpeciesTorsion::Cos3Form,	0,	20.5016,	0,	0 },	// dicarbonyls
		{ this, "O",	"C",	"C",	"O",	SpeciesTorsion::Cos3Form,	6.6944,	13.3888,	0,	0 },	// dicarbonyls
		{ this, "CT",	"C",	"N",	"OH",	SpeciesTorsion::Cos3Form,	19.0037,	27.627,	4.37228,	0 },	// hydroxamic
		{ this, "O",	"C",	"N",	"OH",	SpeciesTorsion::Cos3Form,	0,	27.627,	0,	0 },	// hydroxamic
		{ this, "C",	"N",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	23.0915,	-28.0328,	2.4309,	0 },	// hydroxamic
		{ this, "H",	"N",	"OH",	"HO",	SpeciesTorsion::Cos3Form,	11.3888,	-21.5643,	0,	0 },	// hydroxamic
		{ this, "F",	"CT",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	1.8828,	0,	0 },	// fluoromethyl
		{ this, "F",	"CT",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	1.8828,	0,	0 },	// fluoromethyl
		{ this, "F",	"CT",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	1.8828,	0,	0 },	// fluoromethyl
		{ this, "Cl",	"CT",	"CA",	"CA",	SpeciesTorsion::Cos3Form,	0,	-1.6736,	0,	0 },	// chloromethyl
		{ this, "Cl",	"CT",	"CW",	"*",	SpeciesTorsion::Cos3Form,	0,	-1.6736,	0,	0 },	// chloromethyl
		{ this, "Cl",	"CT",	"CS",	"*",	SpeciesTorsion::Cos3Form,	0,	-1.6736,	0,	0 },	// chloromethyl
		{ this, "Cl",	"CT",	"C",	"O",	SpeciesTorsion::Cos3Form,	-2.7196,	0,	0,	0 },	// 2-chloroamide
		{ this, "Cl",	"CT",	"C",	"N",	SpeciesTorsion::Cos3Form,	2.7196,	0,	0,	0 },	// 2-chloroamide
		{ this, "CT",	"CT",	"CT",	"Si",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// silane
		{ this, "CT",	"CT",	"Si",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// silane
		{ this, "CT",	"CT",	"Si",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0.6276,	0 },	// silane
		{ this, "HC",	"CT",	"Si",	"H",	SpeciesTorsion::Cos3Form,	0,	0,	0.75312,	0 },	// silane
		{ this, "Si",	"CT",	"CT",	"Si",	SpeciesTorsion::Cos3Form,	21.7568,	-2.092,	0,	0 },	// silane
		{ this, "Si",	"CT",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0.6276,	0 },	// silane
		{ this, "CT",	"Si",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0.75312,	0 },	// silane
		{ this, "OS",	"Si",	"CT",	"HC",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tentative
		{ this, "OS",	"Si",	"OS",	"Si",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tentative
		{ this, "Si",	"OS",	"Si",	"CT",	SpeciesTorsion::Cos3Form,	0,	0,	0,	0 },	// tentative
		{ this, "CT",	"CT",	"CT",	"CT",	SpeciesTorsion::Cos4Form,	27.7064,	3.96643,	-5.80739,	-8.86171 },	// perfluoroalkane
		{ this, "N2",	"CA",	"CA",	"CA",	SpeciesTorsion::Cos4Form,	0,	6.77808,	0,	-1.84096 }	// benzamidine
	};
}

OPLSAA2005BaseForcefield::~OPLSAA2005BaseForcefield()
{
}

/*
 * Definition
 */

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType OPLSAA2005BaseForcefield::shortRangeType() const
{
	return Forcefield::LennardJonesGeometricType;
}
