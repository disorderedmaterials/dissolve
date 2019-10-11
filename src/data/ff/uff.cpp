/*
	*** Universal Forcefield
	*** src/data/ff/uff.cpp
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

#include "data/ff/uff.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "classes/species.h"

/*
 * Implements "UFF, a Full Periodic Table Force Field for Molecular Mechanics and Molecular Dynamics Simulations"
 * A. K. Rappe, C. J. Casewit, K. S. Colwell, W. A. Goddard III, and W. M. Skiff
 * J. Am. Chem. Soc. 114, 10024-10039 (1992)
 * 
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * Generator data 8 (THyb) are used to quickly determine the method of torsional parameter generation.
 * Torsional parameters U(i) are assigned to the second through sixth periods, following M. G. Martin's implementation in MCCCS Towhee.
 * Other modifications from the original paper are made following the MCCCS Towhee implementation.
 * All energy values are in kcal.
 */

// Constructor / Destructor
Forcefield_UFF::Forcefield_UFF() : Forcefield()
{
	// Create the atom type data - the ForcefieldAtomTypes will be registered with the parent Forcefield via their constructors
	static UFFAtomType uffData[] =
	{
		// Z	El	FFID	Name		Description
		// 					  r	theta     x       D     zeta      Z      chi	geom	V	U
		{ this,	1,	"H",	1,	"H_",		"Generic hydrogen",
								0.3540,	180.00,	2.8860,	0.0440,	12.000,	0.7120,	4.528,	0,	0.0,	0.0 },
		{ this,	1,	"H",	2,	"H_b",		"Bridging hydrogen in B-H-B",
								0.4600,	83.50,	2.8860,	0.0440,	12.000,	0.7125,	4.528,	8,	0.0,	0.0 },
		{ this,	2,	"He",	3,	"He4+4",	"Helium",
								0.8490,	90.00,	2.3620,	0.0560,	15.240,	0.0972,	9.66,	4,	0.0,	0.0 },
		{ this,	3,	"Li",	4,	"Li",		"Lithium",
								1.3360,	180.00,	2.4510,	0.0250,	12.000,	1.0255,	3.006,	0,	0.0,	2.0 },
		{ this,	4,	"Be",	5,	"Be3+2",	"Beryllium",
								1.0740,	109.47,	2.7450,	0.0850,	12.000,	1.5650,	4.877,	3,	0.0,	2.0 },
		{ this,	5,	"B",	6,	"B_3",		"Boron (tetrahedral)",
								0.8380,	109.47,	4.0830,	0.1800,	12.052,	1.7550,	5.11,	3,	0.0,	2.0 },
		{ this,	5,	"B",	7,	"B_2",		"Boron (trigonal)",
								0.8280,	120.00,	4.0830,	0.1800,	12.052,	1.7550,	5.11,	2,	0.0,	2.0 },
		{ this,	6,	"C",	8,	"C_3",		"Carbon (tetrahedral)",
								0.7570,	109.47,	3.8510,	0.1050,	12.730,	1.9120,	5.343,	3,	2.119,	2.0 },
		{ this,	6,	"C",	9,	"C_R",		"Carbon (resonant)",
								0.7290,	120.00,	3.8510,	0.1050,	12.730,	1.9120,	5.343,	9,	0.0,	2.0 },
		{ this,	6,	"C",	10,	"C_2",		"Carbon (trigonal)",
								0.7320,	120.00,	3.8510,	0.1050,	12.730,	1.9120,	5.343,	2,	0.0,	2.0 },
		{ this,	6,	"C",	11,	"C_1",		"Carbon (linear)",
								0.7060,	180.00,	3.8510,	0.1050,	12.730,	1.9120,	5.343,	1,	0.0,	2.0 },
		{ this,	7,	"N",	12,	"N_3",		"Nitrogen (tetrahedral)",
								0.7000,	106.70,	3.6600,	0.0690,	13.407,	2.5438,	6.899,	3,	0.450,	0.0 },
		{ this,	7,	"N",	13,	"N_R",		"Nitrogen (resonant)",
								0.6990,	120.00,	3.6600,	0.0690,	13.407,	2.5438,	6.899,	9,	0.0,	2.0 },
		{ this,	7,	"N",	14,	"N_2",		"Nitrogen (trigonal)",
								0.6850,	111.30,	3.6600,	0.0690,	13.407,	2.5438,	6.899,	2,	0.0,	2.0 },
		{ this,	7,	"N",	15,	"N_1",		"Nitrogen (linear)",
								0.6560,	180.00,	3.6600,	0.0690,	13.407,	2.5438,	6.899,	1,	0.0,	2.0 },
		{ this,	8,	"O",	16,	"O_3",		"Oxygen (tetrahedral)",
								0.6580,	104.51,	3.5000,	0.0600,	14.085,	2.2998,	8.741,	3,	0.018,	2.0 },
		{ this,	8,	"O",	17,	"O_3_z",	"Oxygen (in silicate)",
								0.5280,	145.50,	3.5000,	0.0600,	14.085,	2.2998,	8.741,	3,	0.018,	2.0 },
		{ this,	8,	"O",	18,	"O_R",		"Oxygen (resonant)",
								0.6800,	110.30,	3.5000,	0.0600,	14.085,	2.2998,	8.741,	9,	0.0,	2.0 },
		{ this,	8,	"O",	19,	"O_2",		"Oxygen (trigonal)",
								0.6340,	120.00,	3.5000,	0.0600,	14.085,	2.2998,	8.741,	2,	0.0,	2.0 },
		{ this,	8,	"O",	20,	"O_1",		"Oxygen (linear)",
								0.6390,	180.00,	3.5000,	0.0600,	14.085,	2.2998,	8.741,	1,	0.0,	2.0 },
		{ this,	9,	"F",	21,	"F_",		"Flourine",
								0.6680,	180.00,	3.3640,	0.0500,	14.762,	1.7350,	10.874,	0,	0.0,	2.0 },
		{ this,	10,	"Ne",	22,	"Ne4+4",	"Neon",
								0.9200,	90.00,	3.2430,	0.0420,	15.440,	0.1944,	11.04,	4,	0.0,	2.0 },
		{ this,	11,	"Na",	23,	"Na",		"Sodium",
								1.5390,	180.00,	2.9830,	0.0300,	12.000,	1.0809,	2.843,	0,	0.0,	1.25 },
		{ this,	12,	"Mg",	24,	"Mg3+2",	"Magnesium",
								1.4210,	109.47,	3.0210,	0.1110,	12.000,	1.7866,	3.951,	3,	0.0,	1.25 },
		{ this,	13,	"Al",	25,	"Al3",		"Aluminium",
								1.2440,	109.47,	4.4990,	0.5050,	11.278,	1.7924,	4.06,	3,	0,	1.25 },
		{ this,	14,	"Si",	26,	"Si3",		"Silicon",
								1.1170,	109.47,	4.2950,	0.4020,	12.175,	2.3232,	4.168,	3,	1.225,	1.25 },
		{ this,	15,	"P",	27,	"P_3+3",	"Phosphorus (tetrahedral, oxidation state +3)",
								1.1010,	93.80,	4.1470,	0.3050,	13.072,	2.8627,	5.463,	3,	2.4,	1.25 },
		{ this,	15,	"P",	28,	"P_3+5",	"Phosphorus (tetrahedral, oxidation state +5)",
								1.0560,	109.47,	4.1470,	0.3050,	13.072,	2.8627,	5.463,	3,	2.4,	1.25 },
		{ this,	15,	"P",	29,	"P_3+q",	"XXX",
								1.0560,	109.47,	4.1470,	0.3050,	13.072,	2.8627,	5.463,	3,	2.4,	1.25 },
		{ this,	16,	"S",	30,	"S_3+2",	"Sulfur (tetrahedral, oxidation state +2)",
								1.0640,	92.10,	4.0350,	0.2740,	13.969,	2.7032,	6.928,	3,	0.484,	1.25 },
		{ this,	16,	"S",	31,	"S_3+4",	"Sulfur (tetrahedral, oxidation state +4)",
								1.0490,	103.20,	4.0350,	0.2740,	13.969,	2.7032,	6.928,	3,	0.484,	1.25 },
		{ this,	16,	"S",	32,	"S_3+6",	"Sulfur (tetrahedral, oxidation state +6)",
								1.0270,	109.47,	4.0350,	0.2740,	13.969,	2.7032,	6.928,	3,	0.484,	1.25 },
		{ this,	16,	"S",	33,	"S_R",		"Sulfur (resonant)",
								1.0770,	92.20,	4.0350,	0.2740,	13.969,	2.7032,	6.928,	9,	0.0,	1.25 },
		{ this,	16,	"S",	34,	"S_2",		"Sulfur (trigonal)",
								0.8540,	120.00,	4.0350,	0.2740,	13.969,	2.7032,	6.928,	2,	0.0,	1.25 },
		{ this,	17,	"Cl",	35,	"Cl",		"Chlorine",
								1.0440,	180.00,	3.9470,	0.2270,	14.886,	2.3484,	8.564,	0,	0.0,	1.25 },
		{ this,	18,	"Ar",	36,	"Ar4+4",	"Argon",
								1.0320,	90.00,	3.8680,	0.1850,	15.763,	0.2994,	9.465,	4,	0.0,	1.25 },
		{ this,	19,	"K",	37,	"K_",		"Krypton",
								1.9530,	180.00,	3.8120,	0.0350,	12.000,	1.1645,	2.421,	0,	0.0,	0.7 },
		{ this,	20,	"Ca",	38,	"Ca6+2",	"Calcium (octahedral, oxidation state +2)",
								1.7610,	90.00,	3.3990,	0.2380,	12.000,	2.1414,	3.231,	6,	0.0,	0.7 },
		{ this,	21,	"Sc",	39,	"Sc3+3",	"Scandium (tetrahedral, oxidation state +3)",
								1.5130,	109.47,	3.2950,	0.0190,	12.000,	2.5924,	3.395,	3,	0.0,	0.7 },
		{ this,	22,	"Ti",	40,	"Ti3+4",	"Titanium (tetrahedral, oxidation state +4)",
								1.4120,	109.47,	3.1750,	0.0170,	12.000,	2.6595,	3.47,	3,	0.0,	0.7 },
		{ this,	22,	"Ti",	41,	"Ti6+4",	"Titanium (octahedral, oxidation state +4)",
								1.4120,	90.00,	3.1750,	0.0170,	12.000,	2.6595,	3.47,	6,	0.0,	0.7 },
		{ this,	23,	"V",	42,	"V_3+5",	"Vanadium",
								1.4020,	109.47,	3.1440,	0.0160,	12.000,	2.6789,	3.65,	3,	0.0,	0.7 },
		{ this,	24,	"Cr",	43,	"Cr6+3",	"Chromium",
								1.3450,	90.00,	3.0230,	0.0150,	12.000,	2.4631,	3.415,	6,	0.0,	0.7 },
		{ this,	25,	"Mn",	44,	"Mn6+2",	"Manganese",
								1.3820,	90.00,	2.9610,	0.0130,	12.000,	2.4301,	3.325,	6,	0.0,	0.7 },
		{ this,	26,	"Fe",	45,	"Fe3+2",	"Iron (tetrahedral, oxidation state +2)",
								1.4120,	109.47,	2.9120,	0.0130,	12.000,	2.4301,	3.76,	3,	0.0,	0.7 },
		{ this,	26,	"Fe",	46,	"Fe6+2",	"Iron (octahedral, oxidation state +2)",
								1.3350,	90.00,	2.9120,	0.0130,	12.000,	2.4301,	3.76,	6,	0.0,	0.7 },
		{ this,	27,	"Co",	47,	"Co6+3",	"Cobalt (octahedral, oxidation state +3)",
								1.2410,	90.00,	2.8720,	0.0140,	12.000,	2.4301,	4.105,	6,	0.0,	0.7 },
		{ this,	28,	"Ni",	48,	"Ni4+2",	"Nickel",
								1.1640,	90.00,	2.8340,	0.0150,	12.000,	2.4301,	4.465,	4,	0.0,	0.7 },
		{ this,	29,	"Cu",	49,	"Cu3+1",	"Copper",
								1.3020,	109.47,	3.4950,	0.0050,	12.000,	1.7565,	4.2,	3,	0.0,	0.7 },
		{ this,	30,	"Zn",	50,	"Zn3+2",	"Zinc",
								1.1930,	109.47,	2.7630,	0.1240,	12.000,	1.3084,	5.106,	3,	0.0,	0.7 },
		{ this,	31,	"Ga",	51,	"Ga3+3",	"Gallium (tetrahedral, oxidation state +3)",
								1.2600,	109.47,	4.3830,	0.4150,	11.000,	1.8206,	3.641,	3,	0.0,	0.7 },
		{ this,	32,	"Ge",	52,	"Ge3",		"Germanium",
								1.1970,	109.47,	4.2800,	0.3790,	12.000,	2.7888,	4.051,	3,	0.701,	0.0 },
		{ this,	33,	"As",	53,	"As3+3",	"Astatine (tetrahedral, oxidation state +3)",
								1.2110,	92.10,	4.2300,	0.3090,	13.000,	2.8640,	5.188,	3,	1.5,	0.0 },
		{ this,	34,	"Se",	54,	"Se3+2",	"Selenium",
								1.1900,	90.60,	4.2050,	0.2910,	14.000,	2.7645,	6.428,	3,	0.335,	0.0 },
		{ this,	35,	"Br",	55,	"Br",		"Bromine",
								1.1920,	180.00,	4.1890,	0.2510,	15.000,	2.5186,	7.79,	0,	0.0,	0.7 },
		{ this,	36,	"Kr",	56,	"Kr4+4",	"Krypton",
								1.1470,	90.00,	4.1410,	0.2200,	16.000,	0.4520,	8.505,	4,	0.0,	0.7 },
		{ this,	37,	"Rb",	57,	"Rb",		"Rubidium",
								2.2600,	180.00,	4.1140,	0.0400,	12.000,	1.5922,	2.331,	0,	0.0,	0.2 },
		{ this,	38,	"Sr",	58,	"Sr6+2",	"Strontium",
								2.0520,	90.00,	3.6410,	0.2350,	12.000,	2.4486,	3.024,	6,	0.0,	0.2 },
		{ this,	39,	"Y",	59,	"Y_3+3",	"Yttrium (tetrahedral, oxidation state +3)",
								1.6980,	109.47,	3.3450,	0.0720,	12.000,	3.2573,	3.83,	3,	0.0,	0.2 },
		{ this,	40,	"Zr",	60,	"Zr3+4",	"Zirconium",
								1.5640,	109.47,	3.1240,	0.0690,	12.000,	3.6675,	3.4,	3,	0.0,	0.2 },
		{ this,	41,	"Nb",	61,	"Nb3+5",	"Niobium",
								1.4730,	109.47,	3.1650,	0.0590,	12.000,	3.6179,	3.55,	3,	0.0,	0.2 },
		{ this,	42,	"Mo",	62,	"Mo6+6",	"Molybdenum (octahedral)",
								1.4670,	90.00,	3.0520,	0.0560,	12.000,	3.4021,	3.465,	6,	0.0,	0.2 },
		{ this,	42,	"Mo",	63,	"Mo3+6",	"Molybdenum (tetrahedral)",
								1.4840,	109.47,	3.0520,	0.0560,	12.000,	3.4021,	3.465,	3,	0.0,	0.2 },
		{ this,	43,	"Tc",	64,	"Tc6+5",	"Technecium",
								1.3220,	90.00,	2.9980,	0.0480,	12.000,	3.4021,	3.29,	6,	0.0,	0.2 },
		{ this,	44,	"Ru",	65,	"Ru6+2",	"Rubidium",
								1.4780,	90.00,	2.9630,	0.0560,	12.000,	3.4021,	3.575,	6,	0.0,	0.2 },
		{ this,	45,	"Rh",	66,	"Rh6+3",	"Rhodium (octahedral, oxidation state +3)",
								1.3320,	90.00,	2.9290,	0.0530,	12.000,	3.5081,	3.975,	6,	0.0,	0.2 },
		{ this,	46,	"Pd",	67,	"Pd4+2",	"Palladium",
								1.3380,	90.00,	2.8990,	0.0480,	12.000,	3.2077,	4.32,	4,	0.0,	0.2 },
		{ this,	47,	"Ag",	68,	"Ag1+1",	"Silver (linear, oxidation state +1)",
								1.3860,	180.00,	3.1480,	0.0360,	12.000,	1.9557,	4.436,	1,	0.0,	0.2 },
		{ this,	48,	"Cd",	69,	"Cd3+2",	"Cadmium",
								1.4030,	109.47,	2.8480,	0.2280,	12.000,	1.6525,	5.034,	3,	0.0,	0.2 },
		{ this,	49,	"In",	70,	"In3+3",	"Indium (tetrahedral, oxidation state +3)",
								1.4590,	109.47,	4.4630,	0.5990,	11.000,	2.0704,	3.506,	3,	0.0,	0.2 },
		{ this,	50,	"Sn",	71,	"Sn3",		"Tin",
								1.3980,	109.47,	4.3920,	0.5670,	12.000,	2.9608,	3.987,	3,	0.199,	0.2 },
		{ this,	51,	"Sb",	72,	"Sb3+3",	"Antimony (tetrahedral, oxidation state +3)",
								1.4070,	91.60,	4.4200,	0.4490,	13.000,	2.7042,	4.899,	3,	1.1,	0.2 },
		{ this,	52,	"Te",	73,	"Te3+2",	"Tellurium",
								1.3860,	90.25,	4.4700,	0.3980,	14.000,	2.8821,	5.816,	3,	0.3,	0.2 },
		{ this,	53,	"I",	74,	"I_",		"Iodine",
								1.3820,	180.00,	4.5000,	0.3390,	15.000,	2.6537,	6.822,	0,	0,	0.2 },
		{ this,	54,	"Xe",	75,	"Xe4+4",	"Xenon",
								1.2670,	90.00,	4.4040,	0.3320,	12.000,	0.5560,	7.595,	4,	0,	0.2 },
		{ this,	55,	"Cs",	76,	"Cs",		"Caesium",
								2.5700,	180.00,	4.5170,	0.0450,	12.000,	1.5728,	2.183,	0,	0.0,	0.1 },
		{ this,	56,	"Ba",	77,	"Ba6+2",	"Barium",
								2.2770,	90.00,	3.7030,	0.3640,	12.000,	2.7266,	2.814,	6,	0.0,	0.1 },
		{ this,	57,	"La",	78,	"La3+3",	"Lanthanum (tetrahedral, oxidation state +3)",
								1.9430,	109.47,	3.5220,	0.0170,	12.000,	3.3049,	2.8355,	3,	0.0,	0.1 },
		{ this,	58,	"Ce",	79,	"Ce6+3",	"Cerium (octahedral, oxidation state +3)",
								1.8410,	90.00,	3.5560,	0.0130,	12.000,	3.3049,	2.774,	6,	0.0,	0.1 },
		{ this,	59,	"Pr",	80,	"Pr6+3",	"Praseodymium (octahedral, oxidation state +3)",
								1.8230,	90.00,	3.6060,	0.0100,	12.000,	3.3049,	2.858,	6,	0.0,	0.1 },
		{ this,	60,	"Nd",	81,	"Nd6+3",	"Neodymium (octahedral, oxidation state +3)",
								1.8160,	90.00,	3.5750,	0.0100,	12.000,	3.3049,	2.8685,	6,	0.0,	0.1 },
		{ this,	61,	"Pm",	82,	"Pm6+3",	"Prometheum (octahedral, oxidation state +3)",
								1.8010,	90.00,	3.5470,	0.0090,	12.000,	3.3049,	2.881,	6,	0.0,	0.1 },
		{ this,	62,	"Sm",	83,	"Sm6+3",	"Samarium (octahedral, oxidation state +3)",
								1.7800,	90.00,	3.5200,	0.0080,	12.000,	3.3049,	2.9115,	6,	0.0,	0.1 },
		{ this,	63,	"Eu",	84,	"Eu6+3",	"Europium (octahedral, oxidation state +3)",
								1.7710,	90.00,	3.4930,	0.0080,	12.000,	3.3049,	2.8785,	6,	0.0,	0.1 },
		{ this,	64,	"Gd",	85,	"Gd6+3",	"Gadolinium (octahedral, oxidation state +3)",
								1.7350,	90.00,	3.3680,	0.0090,	12.000,	3.3049,	3.1665,	6,	0.0,	0.1 },
		{ this,	65,	"Tb",	86,	"Tb6+3",	"Terbium (octahedral, oxidation state +3)",
								1.7320,	90.00,	3.4510,	0.0070,	12.000,	3.3049,	3.018,	6,	0.0,	0.1 },
		{ this,	66,	"Dy",	87,	"Dy6+3",	"Dysprosium (octahedral, oxidation state +3)",
								1.7100,	90.00,	3.4280,	0.0070,	12.000,	3.3049,	3.0555,	6,	0.0,	0.1 },
		{ this,	67,	"Ho",	88,	"Ho6+3",	"Holmium (octahedral, oxidation state +3)",
								1.6960,	90.00,	3.4090,	0.0070,	12.000,	3.4157,	3.127,	6,	0.0,	0.1 },
		{ this,	68,	"Er",	89,	"Er6+3",	"Erbium (octahedral, oxidation state +3)",
								1.6730,	90.00,	3.3910,	0.0070,	12.000,	3.3049,	3.1865,	6,	0.0,	0.1 },
		{ this,	69,	"Tm",	90,	"Tm6+3",	"Thulium (octahedral, oxidation state +3)",
								1.6600,	90.00,	3.3740,	0.0060,	12.000,	3.3049,	3.2514,	6,	0.0,	0.1 },
		{ this,	70,	"Yb",	91,	"Yb6+3",	"Ytterbium (octahedral, oxidation state +3)",
								1.6370,	90.00,	3.3550,	0.2280,	12.000,	2.6177,	3.2889,	6,	0.0,	0.1 },
		{ this,	71,	"Lu",	92,	"Lu6+3",	"Lutetium (octahedral, oxidation state +3)",
								1.6710,	90.00,	3.6400,	0.0410,	12.000,	3.2709,	2.9629,	6,	0.0,	0.1 },
		{ this,	72,	"Hf",	93,	"Hf3+4",	"Hafnium (tetrahedral, oxidation state +4)",
								1.6110,	109.47,	3.1410,	0.0720,	12.000,	3.9212,	3.7,	3,	0.0,	0.1 },
		{ this,	73,	"Ta",	94,	"Ta3+5",	"Tantalum (tetrahedral, oxidation state +5)",
								1.5110,	109.47,	3.1700,	0.0810,	12.000,	4.0748,	5.1,	3,	0.0,	0.1 },
		{ this,	74,	"W",	95,	"W_6+6",	"Tungsten (octahedral, oxidation state +6)",
								1.3920,	90.00,	3.0690,	0.0670,	12.000,	3.6937,	4.63,	6,	0.0,	0.1 },
		{ this,	74,	"W",	96,	"W_3+4",	"Tungsten (tetrahedral, oxidation state +4)",
								1.5260,	109.47,	3.0690,	0.0670,	12.000,	3.6937,	4.63,	3,	0.0,	0.1 },
		{ this,	74,	"W",	97,	"W_3+6",	"Tungsten (tetrahedral, oxidation state +6)",
								1.3800,	109.47,	3.0690,	0.0670,	12.000,	3.6937,	4.63,	3,	0.0,	0.1 },
		{ this,	75,	"Re",	98,	"Re6+5",	"Rhenium (octahedral, oxidation state +5)",
								1.3720,	90.00,	2.9540,	0.0660,	12.000,	3.6937,	3.96,	6,	0.0,	0.1 },
		{ this,	75,	"Re",	99,	"Re3+7",	"Rhenium (tetrahedral, oxidation state +7)",
								1.3140,	109.47,	2.9540,	0.0660,	12.000,	3.6937,	3.96,	3,	0.0,	0.1 },
		{ this,	76,	"Os",	100,	"Os6+6",	"Osmium (octahedral, oxidation state +6)",
								1.3720,	90.00,	3.1200,	0.0370,	12.000,	3.6937,	5.14,	6,	0.0,	0.1 },
		{ this,	77,	"Ir",	101,	"Ir6+3",	"Iridium (octahedral, oxidation state +3)",
								1.3710,	90.00,	2.8400,	0.0730,	12.000,	3.7307,	5.0,	6,	0.0,	0.1 },
		{ this,	78,	"Pt",	102,	"Pt4+2",	"Platinum",
								1.3640,	90.00,	2.7540,	0.0800,	12.000,	3.3817,	4.79,	4,	0.0,	0.1 },
		{ this,	79,	"Au",	103,	"Au4+3",	"Gold",
								1.2620,	90.00,	3.2930,	0.0390,	12.000,	2.6255,	4.894,	4,	0.0,	0.1 },
		{ this,	80,	"Hg",	104,	"Hg1+2",	"Mercury",
								1.3400,	180.00,	2.7050,	0.3850,	12.000,	1.7497,	6.27,	1,	0.0,	0.1 },
		{ this,	81,	"Tl",	105,	"Tl3+3",	"Thallium (tetrahedral, oxidation state +3)",
								1.5180,	120.00,	4.3470,	0.6800,	11.000,	2.0685,	3.2,	3,	0.0,	0.1 },
		{ this,	82,	"Pb",	106,	"Pb3",		"Lead",
								1.4590,	109.47,	4.2970,	0.6630,	12.000,	2.8461,	3.9,	3,	0.1,	0.1 },
		{ this,	83,	"Bi",	107,	"Bi3+3",	"Bismuth (tetrahedral, oxidation state +3)",
								1.5120,	90.00,	4.3700,	0.5180,	13.000,	2.4700,	4.69,	3,	1.0,	0.1 },
		{ this,	84,	"Po",	108,	"Po3+2",	"Polonium (tetrahedral, oxidation state +2)",
								1.5000,	90.00,	4.7090,	0.3250,	14.000,	2.3329,	4.21,	3,	0.3,	0.1 },
		{ this,	85,	"At",	109,	"At",		"Astatine",
								1.5450,	180.00,	4.7500,	0.2840,	15.000,	2.2357,	4.75,	0,	0.0,	0.1 },
		{ this,	86,	"Rn",	110,	"Rn4+4",	"Radon",
								1.4200,	90.00,	4.7650,	0.2480,	16.000,	0.5832,	5.37,	4,	0.0,	0.1 },
		{ this,	87,	"Fr",	111,	"Fr",		"Francium",
								2.8800,	180.00,	4.9000,	0.0500,	12.000,	1.8469,	2.0,	0,	0.0,	0.0 },
		{ this,	88,	"Ra",	112,	"Ra6+2",	"Radium (octahedral, oxidation state +2)",
								2.5120,	90.00,	3.6770,	0.4040,	12.000,	2.9161,	2.843,	6,	0.0,	0.0 },
		{ this,	89,	"Ac",	113,	"Ac6+3",	"Actinium (octahedral, oxidation state +3)",
								1.9830,	90.00,	3.4780,	0.0330,	12.000,	3.8882,	2.835,	6,	0.0,	0.0 },
		{ this,	90,	"Th",	114,	"Th6+4",	"Thorium (octahedral, oxidation state +4)",
								1.7210,	90.00,	3.3960,	0.0260,	12.000,	4.2021,	3.175,	6,	0.0,	0.0 },
		{ this,	91,	"Pa",	115,	"Pa6+4",	"Protactinium (octahedral, oxidation state +4)",
								1.7110,	90.00,	3.4240,	0.0220,	12.000,	3.8882,	2.985,	6,	0.0,	0.0 },
		{ this,	92,	"U",	116,	"U_6+4",	"Uranium (octahedral, oxidation state +4)",
								1.6840,	90.00,	3.3950,	0.0220,	12.000,	3.8882,	3.341,	6,	0.0,	0.0 },
		{ this,	93,	"Np",	117,	"Np6+4",	"Neptunium (octahedral, oxidation state +4)",
								1.6660,	90.00,	3.4240,	0.0190,	12.000,	3.8882,	3.549,	6,	0.0,	0.0 },
		{ this,	94,	"Pu",	118,	"Pu6+4",	"Plutonium (octahedral, oxidation state +4)",
								1.6570,	90.00,	3.4240,	0.0160,	12.000,	3.8882,	3.243,	6,	0.0,	0.0 },
		{ this,	95,	"Am",	119,	"Am6+4",	"Americium (octahedral, oxidation state +4)",
								1.6600,	90.00,	3.3810,	0.0140,	12.000,	3.8882,	2.9895,	6,	0.0,	0.0 },
		{ this,	96,	"Cm",	120,	"Cm6+3",	"Curium (octahedral, oxidation state +3)",
								1.8010,	90.00,	3.3260,	0.0130,	12.000,	3.8882,	2.8315,	6,	0.0,	0.0 },
		{ this,	97,	"Bk",	121,	"Bk6+3",	"Berkelium (octahedral, oxidation state +3)",
								1.7610,	90.00,	3.3390,	0.0130,	12.000,	3.8882,	3.1935,	6,	0.0,	0.0 },
		{ this,	98,	"Cf",	122,	"Cf6+3",	"Californium (octahedral, oxidation state +3)",
								1.7500,	90.00,	3.3130,	0.0130,	12.000,	3.8882,	3.197,	6,	0.0,	0.0 },
		{ this,	99,	"Es",	123,	"Es6+3",	"Einsteinium (octahedral, oxidation state +3)",
								1.7240,	90.00,	3.2990,	0.0120,	12.000,	3.8882,	3.333,	6,	0.0,	0.0 },
		{ this,	100,	"Fm",	124,	"Fm6+3",	"Fermium (octahedral, oxidation state +3)",
								1.7120,	90.00,	3.2860,	0.0120,	12.000,	3.8882,	3.4,	6,	0.0,	0.0 },
		{ this,	101,	"Md",	125,	"Md6+3",	"Mendelevium (octahedral, oxidation state +3)",
								1.6890,	90.00,	3.2740,	0.0110,	12.000,	3.8882,	3.47,	6,	0.0,	0.0 },
		{ this,	102,	"No",	126,	"No6+3",	"Nobelium (octahedral, oxidation state +3)",
								1.6790,	90.00,	3.2480,	0.0110,	12.000,	3.8882,	3.475,	6,	0.0,	0.0 },
		{ this,	103,	"Lr",	127,	"Lr6+3",	"Lawrencium (octahedral, oxidation state +3)",
								1.6980,	90.00,	3.2360,	0.0110,	12.000,	3.8882,	3.5,	6,	1.0,	1.0 }
	};
}

Forcefield_UFF::~Forcefield_UFF()
{
}

/*
 * Definition
 */

// Return name of Forcefield
const char* Forcefield_UFF::name() const
{
	return "UFF";
}

// Return description for Forcefield
const char* Forcefield_UFF::description() const
{
	return "Implements 'UFF, a Full Periodic Table Force Field for Molecular Mechanics and Molecular Dynamics Simulations', A. K. Rappe, C. J. Casewit, K. S. Colwell, W. A. Goddard III, and W. M. Skiff, <i>J. Am. Chem. Soc.</i> <b>114</b>, 10024-10039 (1992).<br/>Notes:<ul><li>Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY</li><li>Generator data 8 (THyb) are used to quickly determine the method of torsional parameter generation.</li><li>Torsional parameters U(i) are assigned to the second through sixth periods, following M. G. Martin's implementation in MCCCS Towhee.</li><li>Other modifications from the original paper are made following the MCCCS Towhee implementation.</li></ul>";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_UFF::shortRangeType() const
{
	return Forcefield::LennardJonesType;
}

/*
 * Atom Type Data
 */

// Determine and return atom type for specified SpeciesAtom
ForcefieldAtomType* Forcefield_UFF::determineAtomType(SpeciesAtom* i) const
{
	switch (i->element()->Z())
	{
		// H
		case (ELEMENT_H): 
				if (isBoundTo(i, &Elements::element(5), 2)) return atomTypeByName("H_b", i->element());
				else return atomTypeByName("H_", i->element());
			break;
		// Boron
		case (ELEMENT_B):
				if (isAtomGeometry(i, Forcefield::TetrahedralGeometry)) return atomTypeByName("B_3", i->element());
				else return atomTypeByName("B_2", i->element());
			break;
		// Carbon
		case (ELEMENT_C):
				if (isBondPattern(i, 0, 0, 0, 0, -1)) return atomTypeByName("C_R", i->element());
				else if (isBondPattern(i, 2, 1)) return atomTypeByName("C_2", i->element());
				else if (isBondPattern(i, 1, 0, 1) || isAtomGeometry(i, Forcefield::LinearGeometry)) return atomTypeByName("C_1", i->element());
				else return atomTypeByName("C_3", i->element());
			break;
		// Nitrogen
		case (ELEMENT_N):
				if (isBondPattern(i, 0, 0, 0, 0, -1)) return atomTypeByName("N_R", i->element());
				else if (isBondPattern(i, 1, 1)) return atomTypeByName("N_2", i->element());
				else if (isBondPattern(i, 0, 0, 1) || isAtomGeometry(i, Forcefield::LinearGeometry)) return atomTypeByName("N_1", i->element());
				else return atomTypeByName("N_3", i->element());
			break;
		// Oxygen
		case (ELEMENT_O):
				if (isBondPattern(i, 0, 0, 0, 0, -1)) return atomTypeByName("O_R", i->element());
				else if (isBondPattern(i, 0, 1)) return atomTypeByName("O_2", i->element());
				else if (isBondPattern(i, 0, 0, 1) || isAtomGeometry(i, Forcefield::LinearGeometry)) return atomTypeByName("O_1", i->element());
				else if (isBoundTo(i, &Elements::element(ELEMENT_SI), 2)) return atomTypeByName("O_3_z", i->element());
				else return atomTypeByName("O_3", i->element());
			break;
		// Phosphorus
		case (ELEMENT_P):
				if (guessOxidationState(i) == 5) return atomTypeByName("P_3+5", i->element());
				else if (guessOxidationState(i) == 3) return atomTypeByName("P_3+3", i->element());
				else if ((i->nBonds() == 4) && (isAtomGeometry(i, Forcefield::TetrahedralGeometry))) return atomTypeByName("P_3+q", i->element());
			break;
		// Sulphur
		case (ELEMENT_S):
				if (guessOxidationState(i) == 2) return atomTypeByName("S_3+2", i->element());
				else if (guessOxidationState(i) == 4) return atomTypeByName("S_3+4", i->element());
				else if (guessOxidationState(i) == 6) return atomTypeByName("S_3+6", i->element());
				else if (isBondPattern(i, 0, 0, 0, 0, -1)) return atomTypeByName("S_R", i->element());
				else if (isAtomGeometry(i, Forcefield::TrigonalPlanarGeometry)) return atomTypeByName("S_2", i->element());
			break;
		// Titanium
		case (ELEMENT_TI):
				if (isAtomGeometry(i, Forcefield::OctahedralGeometry)) return atomTypeByName("Ti6+4", i->element());
				else return atomTypeByName("Ti3+4", i->element());
			break;
		// Iron
		case (ELEMENT_FE):
				if (isAtomGeometry(i, Forcefield::OctahedralGeometry)) return atomTypeByName("Fe6+2", i->element());
				else return atomTypeByName("Fe3+2", i->element());
				
			break;
		// Molybdenum
		case (ELEMENT_MO):
				if (isAtomGeometry(i, Forcefield::OctahedralGeometry)) return atomTypeByName("Mo6+6", i->element());
				else return atomTypeByName("Mo3+6", i->element());
			break;
		// Tungsten
		case (ELEMENT_W):
				if (isAtomGeometry(i, Forcefield::OctahedralGeometry)) return atomTypeByName("W_6+6", i->element());
				else if (guessOxidationState(i) == 4) return atomTypeByName("W_3+4", i->element());
				else return atomTypeByName("W_3+6", i->element());
			break;
		// Rhenium
		case (ELEMENT_RE):
				if (isAtomGeometry(i, Forcefield::OctahedralGeometry)) return atomTypeByName("Re6+5", i->element());
				else return atomTypeByName("Re3+7", i->element());
			break;
		// Default - all elements with only one type
		default:
			return atomTypesByElementPrivate_.constAt(i->element()->Z()).firstItem();
			break;
	}

	return NULL;
}

/*
 * Term Assignment
 */

// Generate bond parameters for the supplied UFF atom types
bool Forcefield_UFF::generateBondTerm(const Species* sp, SpeciesBond* bondTerm, UFFAtomType* i, UFFAtomType* j) const
{
	// Check type pointers
	if ((!i) || (!j)) return Messenger::error("One or more NULL type pointers passed (%p-%p).\n", i, j);

	// Calculate rBO : Bond-order correction = -0.1332 * (ri + rj) * ln(n)  (eq 3)
	const double sumr = i->r() + j->r();
	const double rBO = -0.1332 * sumr * log(bondTerm->bondOrder());

	// Calculate rEN : Electronegativity correction : ri*rj * (sqrt(Xi)-sqrt(Xj))**2 / (Xi*ri + Xj*rj)    (eq 4)
	const double chi = sqrt(i->chi()) - sqrt(j->chi());
	const double rEN = i->r() * j->r() * chi * chi / (i->chi()*i->r() + j->chi()*j->r());

	// rij : Equilibrium distance : = ri + rj + rBO - rEN  (eq 2)
	// Note: In the original paper  rij = ri + rj + rBO + rEN, but Marcus Martin (MCCCS Towhee) notes that the last term should be subtracted
	const double rij = sumr + rBO - rEN;

	// k : Force constant : = 664.12 * (Zi * Zj) / rij**3     (note 664.12 in kcal)
	const double k = 664.12 * 4.184 * (i->Z() * j->Z()) / (rij * rij * rij);

	// Set the parameters and form of the new bond term
	// Functional form is Harmonic : U = 0.5 * k * (r - eq)**2
	bondTerm->setForm(SpeciesBond::HarmonicForm);
	bondTerm->setParameters(k, rij);

	return true;
}

// Generate angle parameters for the supplied UFF atom types
bool Forcefield_UFF::generateAngleTerm(const Species* sp, SpeciesAngle* angleTerm, UFFAtomType* i, UFFAtomType* j, UFFAtomType* k) const
{
	// Check type pointers
	if ((!i) || (!j) || (!k)) return Messenger::error("One or more NULL type pointers passed (%p-%p-%p).\n", i, j, k);

	// rBO : Bond-order correction = -0.1332 * (ri + rj) * ln(n)  (eq 3)
	// We need the bond orders of the involved bonds...
	SpeciesBond* ij = sp->hasBond(angleTerm->i(), angleTerm->j());
	if (!ij) return Messenger::error("Can't locate bond i-j for bond order retrieval.\n");
	SpeciesBond* jk = sp->hasBond(angleTerm->j(), angleTerm->k());
	if (!jk) return Messenger::error("Can't locate bond j-k for bond order retrieval.\n");

	const double sumrij = i->r() + j->r();
	const double sumrjk = j->r() + k->r();
	const double rBOij = -0.1332 * sumrij * log(ij->bondOrder());
	const double rBOjk = -0.1332 * sumrjk * log(jk->bondOrder());

	// rEN : Electronegativity correction : ri*rj * (sqrt(Xi)-sqrt(Xj))**2 / (Xi*ri + Xj*rj)    (eq 4)
	const double chiij = sqrt(i->chi()) - sqrt(j->chi());
	const double rENij = i->r() * j->r() * chiij * chiij / (i->chi()*i->r() + j->chi()*j->r());
	const double chijk = sqrt(j->chi()) - sqrt(k->chi());
	const double rENjk = j->r() * k->r() * chijk * chijk / (j->chi()*j->r() + k->chi()*k->r());

	// rij : Equilibrium distance : = ri + rj + rBO - rEN  (eq 2)
	// Note: In the original paper  rij = ri + rj + rBO + rEN, but Marcus Martin (MCCCS Towhee) notes that the last term should be subtracted
	double rij = sumrij + rBOij - rENij;
	double rjk = sumrjk + rBOjk - rENjk;

	// Get theta for the central atom
	const double theta = j->theta();
	const double cosTheta = cos(theta);

	// Determine rik2 and rik5 values
	// rik2 = rij**2 + rjk**2 - 2 * rij * rjk * cos(theta)
	const double rik2 = rij * rij + rjk * rjk - 2.0 * rij * rjk * cosTheta;
	const double rik5 = rik2 * rik2 * sqrt(rik2);
	const double forcek = 664.12 * 4.184 * (i->Z() * k->Z() / rik5) * ( 3.0 * rij * rjk * (1.0 - cosTheta*cosTheta) - rik2*cosTheta);

	// To determine angle form and necessary coefficients, use 'geom' integer data (which represents the third letter of the atom name.
	// This idea is shamelessly stolen from MCCCS Towhee!
	int n = 0;
	const int geom = j->geom();

	if (geom == 0) Messenger::error("Unable to generate angle function around central atom '%s'.\n", j->typeName());
	else if (geom == 1) n = 1;
	else if (geom == 2) n = 3;
	else if ((geom == 3) && (j->theta() < 90.1)) n = 2;
	else if ((geom == 4) || (geom == 6)) n = 4;
	else
	{
		// General nonlinear case:  U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
		const double c2 = 1.0 / (4.0 * sin(theta)*sin(theta));
		const double c1 = -4.0 * c2 * cosTheta;
		const double c0 = c2 * (2.0 * cosTheta * cosTheta + 1.0);

		angleTerm->setForm(SpeciesAngle::Cos2Form);
		angleTerm->setParameters(forcek, c0, c1, c2);

		return true;
	}

	// Setup terms for the specific case (n != 0)
	angleTerm->setForm(SpeciesAngle::CosineForm);
	angleTerm->setParameters(forcek/(n*n), n, 0.0, -1.0);

	return true;
}

// Generate torsion parameters for the supplied UFF atom types
bool Forcefield_UFF::generateTorsionTerm(const Species* sp, SpeciesTorsion* torsionTerm, UFFAtomType* i, UFFAtomType* j, UFFAtomType* k, UFFAtomType* l) const
{
	/*
	 * There are seven cases to consider, listed in decreasing complexity:
	 *  a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres
	 *  b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
	 *  c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
	 *  d) j and k are both sp3 centres
	 *  e) j and k are both sp2 centres
	 *  f) j is sp2 and k is sp3 (or vice versa)
	 *  g) everything else (no torsional barrier)
	 */

// 	# The original formula in the paper is given as a sum over cosine terms, but this reduces to: 0.5 * V * (1 - cos(n*eq) * cos(n*phi))
// 	# Aten's single cosine potential has the form: forcek * (1 + s*cos(period*phi - eq))
// 	# Therefore: forcek = 0.5 * V
// 	#		  s = -cos(n*eq)
// 	#	     period = n
// 	#		 eq = 0.0

	const int groupJ = group(j->Z());
	const int groupK = group(k->Z());

	const int geomI = i->geom() == 9 ? 2 : i->geom();
	const int geomJ = j->geom() == 9 ? 2 : j->geom();
	const int geomK = k->geom() == 9 ? 2 : k->geom();
	const int geomL = l->geom() == 9 ? 2 : l->geom();
	double V, n, phi0;

	// Selection begins
	if ((groupJ == 16) && (groupK == 16) && (geomJ == 3) && (geomK == 3))
	{
		// Case a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres

		// V value is 2.0 kcal for oxygen, 6.8 kcal otherwise
		double vj = j->Z() == ELEMENT_O ? 2.0 : 6.8;
		double vk = k->Z() == ELEMENT_O ? 2.0 : 6.8;
		V = sqrt(vj*vk);
		n = 2.0;
		phi0 = 90.0;
	}
	else if ( ((groupJ == 16) && (geomJ == 3) && (geomK == 2)) || ((groupK == 16) && (geomK == 3) && (geomJ == 2)) )
	{
		// Case b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
		// Use eq 17, but since the bond order is 1 (single bond) ln term in eq 17 is zero...
		V = 5.0 * sqrt(j->U()*k->U());
		n = 2.0;
		phi0 = 90.0;
	}
	else if ((geomJ == 3) && (geomK == 3))
	{
		// Case d) j and k are both sp3 centres
		V = sqrt(j->V()*k->V());
		n = 3.0;
		phi0 = 180.0;
	}
	else if ((geomJ == 2) && (geomK == 2))
	{
		// Case e) j and k are both sp2 centres
		// Force constant is adjusted based on current bond order
		SpeciesBond* jk = sp->hasBond(torsionTerm->j(), torsionTerm->k());
		if (jk) V = 5.0 * sqrt(j->U()*k->U()) * (1.0 + 4.18*log(jk->bondOrder()));
		else
		{
			Messenger::error("Can't generate correct force constant for torsion, since the SpeciesBond jk is NULL.\n");
			V = 5.0 * sqrt(j->U()*k->U());
		}
		n = 2.0;
		phi0 = 180.0;
	}
	else if ( ((geomJ == 3) && (geomK == 2)) || ((geomK == 3) && (geomJ == 2)) )
	{
		// Case f) j is sp2 and k is sp3 (or vice versa)
		V = 1.0;
		n = 6.0;
		phi0 = 0.0;
	}
	else if ( ((geomJ == 3) && (geomK == 2) && (geomL == 2)) || ((geomK == 3) && (geomJ == 2) && (geomI == 2)) )
	{
		// Case c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
		V = 2.0;
		n = 3.0;
		phi0 = 180.0;
	}
	else
	{
		// Case g) everything else
		// Everything else....
		V = 0.0;
		n = 1.0;
		phi0 = 0.0;
	}

	// Convert V from kcal to kJ
	V *= 4.184;

	// Store the generated parameters
	torsionTerm->setForm(SpeciesTorsion::UFFCosineForm);
	torsionTerm->setParameters(V, n, phi0);

	return true;
}

// Assign suitable AtomTypes to the supplied Species
bool Forcefield_UFF::assignAtomTypes(Species* sp, CoreData& coreData, bool keepExisting) const
{
	// Loop over Species atoms
	for (SpeciesAtom* i = sp->atoms().first(); i != NULL; i = i->next())
	{
		// If keepExisting == true, don't reassign a type to this atom if one already exists
		if (keepExisting && i->atomType()) continue;

		UFFAtomType* uffType = dynamic_cast<UFFAtomType*>(determineAtomType(i));
		if (!uffType) Messenger::print("No UFF type available for Atom %i of Species (%s).\n", i->index()+1, i->element()->symbol());
		else
		{
			// Check if an AtomType of the same name already exists - if it does, just use that one
			AtomType* at = coreData.findAtomType(uffType->typeName());
			if (!at)
			{
				at = coreData.addAtomType(i->element());
				at->setName(uffType->typeName());

				/*
				 * Determine suitable LJ parameters.
				 *     UFF functional form  : U = Dij * ( (x/rij)**12 - 2(x/rij)**6 )
				 * Our functional form (LJ) : U = 4 * epsilon * [ (s/rij)**12 - (s/rij)**6 ]
				 * 
				 * So:    sigma = x / (2.0^(1/6))
				 * 	epsilon = Dij * 4.184       (UFF energy parameters in kcal)
				 */
				at->parameters().setParameter(0, uffType->D() * 4.184);
				at->parameters().setParameter(1, uffType->x() / (pow(2.0, 1.0/6.0)));
			}

			i->setAtomType(at);
		}
	}

	return true;
}

// Assign intramolecular parameters to the supplied Species
bool Forcefield_UFF::assignIntramolecular(Species* sp, bool useExistingTypes, bool assignBonds, bool assignAngles, bool assignTorsions) const
{
	// Create an array of the UFFAtomTypes for the atoms in the Species for speed
	Array<UFFAtomType*> atomTypes;
	if (useExistingTypes)
	{
		// For each SpeciesAtom, search for the AtomType by name...
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			if (!i->atomType()) return Messenger::error("No AtomType assigned to SpeciesAtom %i, so can't generate intramolecular terms based on existing types.\n", i->userIndex());
			UFFAtomType* at = dynamic_cast<UFFAtomType*>(atomTypeByName(i->atomType()->name(), i->element()));
			if (!at) return Messenger::error("Existing AtomType name '%s' does not correspond to a type in this forcefield.\n", i->atomType()->name());
			atomTypes.add(at);
		}
	}
	else
	{
		// Use on-the-fly generated types for all atoms
		ListIterator<SpeciesAtom> atomIterator(sp->atoms());
		while (SpeciesAtom* i = atomIterator.iterate())
		{
			UFFAtomType* at = dynamic_cast<UFFAtomType*>(determineAtomType(i));
			if (!at) return Messenger::error("Couldn't determine a suitable AtomType for atom %i.\n", i->userIndex());
			atomTypes.add(at);
		}
	}

	// Generate bond terms
	if (assignBonds)
	{
		// Generate bond parameters
		ListIterator<SpeciesBond> bondIterator(sp->bonds());
		while (SpeciesBond* bond = bondIterator.iterate())
		{
			UFFAtomType* i = atomTypes[bond->indexI()];
			UFFAtomType* j = atomTypes[bond->indexJ()];
			if (!generateBondTerm(sp, bond, i, j)) return Messenger::error("Failed to create parameters for bond %i-%i.\n", bond->indexI()+1, bond->indexJ()+1);
		}
	}

	// Generate angle terms
	if (assignTorsions)
	{
		// Generate angle parameters
		ListIterator<SpeciesAngle> angleIterator(sp->angles());
		while (SpeciesAngle* angle = angleIterator.iterate())
		{
			UFFAtomType* i = atomTypes[angle->indexI()];
			UFFAtomType* j = atomTypes[angle->indexJ()];
			UFFAtomType* k = atomTypes[angle->indexK()];
			if (!generateAngleTerm(sp, angle, i, j, k)) return Messenger::error("Failed to create parameters for angle %i-%i-%i.\n", angle->indexI()+1, angle->indexJ()+1, angle->indexK()+1);
		}
	}

	// Generate torsion terms
	if (assignTorsions)
	{
		// Generate torsion parameters
		ListIterator<SpeciesTorsion> torsionIterator(sp->torsions());
		while (SpeciesTorsion* torsion = torsionIterator.iterate())
		{
			UFFAtomType* i = atomTypes[torsion->indexI()];
			UFFAtomType* j = atomTypes[torsion->indexJ()];
			UFFAtomType* k = atomTypes[torsion->indexK()];
			UFFAtomType* l = atomTypes[torsion->indexL()];
			if (!generateTorsionTerm(sp, torsion, i, j, k, l)) return Messenger::error("Failed to create parameters for torsion %i-%i-%i-%i.\n", torsion->indexI()+1, torsion->indexJ()+1, torsion->indexK()+1, torsion->indexL()+1);
		}
	}

	return true;
}

// 
// int torsiongenerator(ffbound newterm, atom i, atom j, atom k, atom l)
// {
// 	# There are seven cases to consider, listed in decreasing complexity:
// 	#   a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres
// 	#   b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
// 	#   c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
// 	#   d) j and k are both sp3 centres
// 	#   e) j and k are both sp2 centres
// 	#   f) j is sp2 and k is sp3 (or vice versa)
// 	#   g) everything else (no torsional barrier)
// 
// 	# The original formula in the paper is given as a sum over cosine terms, but this reduces to: 0.5 * V * (1 - cos(n*eq) * cos(n*phi))
// 	# Aten's single cosine potential has the form: forcek * (1 + s*cos(period*phi - eq))
// 	# Therefore: forcek = 0.5 * V
// 	#		  s = -cos(n*eq)
// 	#	     period = n
// 	#		 eq = 0.0
// 
// 	ffatom fi = i.type;
// 	ffatom fj = j.type;
// 	ffatom fk = k.type;
// 	ffatom fl = l.type;
// 	int groupj = aten.elements[j.z].group;
// 	int groupk = aten.elements[k.z].group;
// 	int geomi = fi.dataI("geom");
// 	if (geomi == 9) geomi = 2;
// 	int geomj = fj.dataI("geom");
// 	if (geomj == 9) geomj = 2;
// 	int geomk = fk.dataI("geom");
// 	if (geomk == 9) geomk = 2;
// 	int geoml = fl.dataI("geom");
// 	if (geoml == 9) geoml = 2;
// 	double n, forcek, s;
// 
// 	# Here we go...
// 	if ((groupj == 16) && (groupk == 16) && (geomj == 3) && (geomk == 3))
// 	{
// 		# Case a) j and k are both group 16 (old group 6) atoms, and both are sp3 centres
// 		double vj = 6.8, vk = 6.8;
// 		# V value is 2.0 for oxygen, 6.8 otherwise
// 		if (j.z == 8) vj = 2.0;
// 		if (k.z == 8) vk = 2.0;
// 		vj = aten.convertEnergy(vj, "kcal");
// 		vk = aten.convertEnergy(vk, "kcal");
// 		forcek = sqrt(vj*vk);
// 		n = 2.0;
// 		s = -cos(n*180.0);
// 	}
// 	else if ( ((groupj == 16) && (geomj == 3) && (geomk == 2)) || ((groupk == 16) && (geomk == 3) && (geomj == 2)) )
// 	{
// 		# Case b) j or k is a group 16 atom, while the other is an sp2 or resonant centre
// 		# Since bond order is 1.0, ln term in eq 17 is zero...
// 		forcek = 5.0 * sqrt(fj.dataD("U")*fk.dataD("U"));
// 		n = 2.0;
// 		s = -cos(n*90.0);
// 	}
// 	else if ((geomj == 3) && (geomk == 3))
// 	{
// 		# Case d) j and k are both sp3 centres
// 		forcek = sqrt(j->V()*k->V());
// 		n = 3.0;
// 		s = -cos(n*180.0);
// 	}
// 	else if ((geomj == 2) && (geomk == 2))
// 	{
// 		# Case e) j and k are both sp2 centres
// 		# Force constant is adjusted based on current bond order
// 		forcek = 5.0 * sqrt(fj.dataD("U")*fk.dataD("U")) * (1.0 + 4.18*ln( j.findBond(k).order() ));
// 		#printf("sp2-sp2  Vi=%f Vj=%f lnbond=%f\n", fj.dataD("U"),fk.dataD("U"),ln( j.findBond(k).order() ));
// 		n = 2.0;
// 		s = -cos(n*180.0);
// 	}
// 	else if ( ((geomj == 3) && (geomk == 2)) || ((geomk == 3) && (geomj == 2)) )
// 	{
// 		# Case f) j is sp2 and k is sp3 (or vice versa)
// 		n = 6.0;
// 		s = -cos(n*0.0);
// 		forcek = aten.convertEnergy(1.0, "kcal");
// 	}
// 	else
// 	{
// 		# Case c) j or k is an sp3 atom, while the other is an sp2/resonant centre bound to another sp2/resonant centre
// 		# Case g) everything else
// 		if ( ((geomj == 3) && (geomk == 2) && (geoml == 2)) || ((geomk == 3) && (geomj == 2) && (geomi == 2)) )
// 		{
// 			forcek = aten.convertEnergy(2.0, "kcal");
// 			n = 3.0;
// 			s = -cos(n*180.0);
// 		}
// 		else
// 		{
// 			# Everything else....
// 			forcek = 0.0;
// 			n = 1.0;
// 			s = 1.0;
// 		}
// 	}
// 
// 	# Store the generated parameters
// 	# Parameter order is: forcek, n, eq, s
// 	newterm.form = "cos";
// 	newterm.data = { 0.5*forcek, n, 0.0, s };
// 	verbose("Generated cos torsion information for %s-%s-%s-%s : k=%f, n=%f, eq=0.0, s=%f\n", fi.equivalent,fj.equivalent,fk.equivalent,fj.equivalent,forcek,n,s);
// 	return 0;
// }

// Perform some test calculations
void Forcefield_UFF::test() const
{
	
}
