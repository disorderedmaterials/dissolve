/*
	*** Atomic Mass Data
	*** src/data/atomicmass.cpp
	Copyright T. Youngs 2012-2018

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

#include "data/atomicmass.h"
#include "base/messenger.h"
#include <stddef.h>

/*
 * Atomic Mass Data
 */

// Constructor
AtomicMassData::AtomicMassData(int z, const char* symbol, double massMin, double massMax) : ElementReference(z, symbol)
{
	if (massMax < 0.0) mass_ = massMin;
	else mass_ = (massMax + massMin) * 0.5;
}

// Return mass of element
double AtomicMassData::mass() const
{
	return mass_;
}

/*
 * Atomic Mass Helper Class
 */

// Return mass data for specified Z
const AtomicMassData& AtomicMass::massData(int Z)
{
	/*
	 * Atomic Mass Data
	 * 
	 * Source: CIAAW. Atomic weights of the elements 2017. Available online at www.ciaaw.org (http://www.ciaaw.org/atomic-weights.htm)
	 */

	static AtomicMassData massData[] = {
		{ 0,	"XX",	0.0			},
		{ 1,	"H",	1.00798175		},	// Uncertainty = (1), VSMOW
		{ 2,	"He",	4.002602		},	// Uncertainty = (2)
		{ 3,	"Li",	6.938,		6.997 },
		{ 4,	"Be",	9.0121831		},	// Uncertainty = (5)
		{ 5,	"B",	10.806,		10.821 },
		{ 6,	"C",	12.0096,	12.0116 },
		{ 7,	"N",	14.00643,	14.00728 },
		{ 8,	"O",	15.99903,	15.99977 },
		{ 9,	"F",	18.998403163		},	// Uncertainty = (6)
		{ 10,	"Ne",	20.1797			},	// Uncertainty = (6)
		{ 11,	"Na",	22.98976928		},	// Uncertainty = (2)
		{ 12,	"Mg",	24.304,		24.307 },
		{ 13,	"Al",	26.9815384		},	// Uncertainty = (3)
		{ 14,	"Si",	28.084,		28.086 },
		{ 15,	"P",	30.973761998		},	// Uncertainty = (5)
		{ 16,	"S",	32.059,		32.076 },
		{ 17,	"Cl",	35.446,		35.457 },
		{ 18,	"Ar",	39.792,		39.963 },
		{ 19,	"K",	39.0983			},	// Uncertainty = (1)
		{ 20,	"Ca",	40.078			},	// Uncertainty = (4) 
		{ 21,	"Sc",	44.955908		},	// Uncertainty = (5)
		{ 22,	"Ti",	47.867			},	// Uncertainty = (1)
		{ 23,	"V",	50.9415			},	// Uncertainty = (1)
		{ 24,	"Cr",	51.9961			},	// Uncertainty = (6)
		{ 25,	"Mn",	54.938043		},	// Uncertainty = (2)
		{ 26,	"Fe",	55.845			},	// Uncertainty = (2)
		{ 27,	"Co",	58.933194		},	// Uncertainty = (3)
		{ 28,	"Ni",	58.6934			},	// Uncertainty = (4)
		{ 29,	"Cu",	63.546			},	// Uncertainty = (3)
		{ 30,	"Zn",	65.38			},	// Uncertainty = (2)
		{ 31,	"Ga",	69.723			},	// Uncertainty = (1)
		{ 32,	"Ge",	72.630			},	// Uncertainty = (8)
		{ 33,	"As",	74.921595		},	// Uncertainty = (6)
		{ 34,	"Se",	78.971			},	// Uncertainty = (8)
		{ 35,	"Br",	79.901,		79.907 },
		{ 36,	"Kr",	83.798			},	// Uncertainty = (2)
		{ 37,	"Rb",	85.4678			},	// Uncertainty = (3) 
		{ 38,	"Sr",	87.62			},	// Uncertainty = (1)
		{ 39,	"Y",	88.90584		},	// Uncertainty = (1)
		{ 40,	"Zr",	91.224			},	// Uncertainty = (2) 
		{ 41,	"Nb",	92.90637		},	// Uncertainty = (1)
		{ 42,	"Mo",	95.95			},	// Uncertainty = (1) 
		{ 43,	"Tc",	98.0			},      // Assumed mass (http://www.rsc.org/periodic-table)
		{ 44,	"Ru",	101.07			},	// Uncertainty = (2) 
		{ 45,	"Rh",	102.90549		},	// Uncertainty = (2)
		{ 46,	"Pd",	106.42			},	// Uncertainty = (1) 
		{ 47,	"Ag",	107.8682		},	// Uncertainty = (2) 
		{ 48,	"Cd",	112.414			},	// Uncertainty = (4) 
		{ 49,	"In",	114.818			},	// Uncertainty = (1)
		{ 50,	"Sn",	118.710			},	// Uncertainty = (7) 
		{ 51,	"Sb",	121.760			},	// Uncertainty = (1) 
		{ 52,	"Te",	127.60			},	// Uncertainty = (3) 
		{ 53,	"I",	126.90447		},	// Uncertainty = (3)
		{ 54,	"Xe",	131.293			},	// Uncertainty = (6)
		{ 55,	"Cs",	132.90545196		},	// Uncertainty = (6)
		{ 56,	"Ba",	137.327			},	// Uncertainty = (7)
		{ 57,	"La",	138.90547		},	// Uncertainty = (7) 
		{ 58,	"Ce",	140.116			},	// Uncertainty = (1) 
		{ 59,	"Pr",	140.90766		},	// Uncertainty = (1)
		{ 60,	"Nd",	144.242			},	// Uncertainty = (3) 
		{ 61,	"Pm",	145.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 62,	"Sm",	150.36			},	// Uncertainty = (2) 
		{ 63,	"Eu",	151.964			},	// Uncertainty = (1) 
		{ 64,	"Gd",	157.25			},	// Uncertainty = (3) 
		{ 65,	"Tb",	158.925354		},	// Uncertainty = (8)
		{ 66,	"Dy",	162.500			},	// Uncertainty = (1) 
		{ 67,	"Ho",	164.930328		},	// Uncertainty = (7)
		{ 68,	"Er",	167.259			},	// Uncertainty = (3) 
		{ 69,	"Tm",	168.934218		},	// Uncertainty = (6)
		{ 70,	"Yb",	173.045			},	// Uncertainty = (10) 
		{ 71,	"Lu",	174.9668		},	// Uncertainty = (1) 
		{ 72,	"Hf",	178.49			},	// Uncertainty = (2)
		{ 73,	"Ta",	180.94788		},	// Uncertainty = (2)
		{ 74,	"W",	183.84			},	// Uncertainty = (1)
		{ 75,	"Re",	186.207			},	// Uncertainty = (1)
		{ 76,	"Os",	190.23			},	// Uncertainty = (3) 
		{ 77,	"Ir",	192.217			},	// Uncertainty = (2)
		{ 78,	"Pt",	195.084			},	// Uncertainty = (9)
		{ 79,	"Au",	196.966570		},	// Uncertainty = (4)
		{ 80,	"Hg",	200.592			},	// Uncertainty = (3)
		{ 81,	"Tl",	204.382,	204.385 },
		{ 82,	"Pb",	207.2			},	// Uncertainty = (1)
		{ 83,	"Bi",	208.98040		},	// Uncertainty = (1)
		{ 84,	"Po",	209.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 85,	"At",	210.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 86,	"Rn",	222.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 87,	"Fr",	223.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 88,	"Ra",	226.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 89,	"Ac",	227.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 90,	"Th",	232.0377		},	// Uncertainty = (4) 
		{ 91,	"Pa",	231.03588		},	// Uncertainty = (1)
		{ 92,	"U",	238.02891		},	// Uncertainty = (3)
		{ 93,	"Np",	237.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 94,	"Pu",	244.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 95,	"Am",	243.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 96,	"Cm",	247.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 97,	"Bk",	247.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 98,	"Cf",	251.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 99,	"Es",	252.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 100,	"Fm",	257.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 101,	"Md",	258.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 102,	"No",	259.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 103,	"Lr",	262.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 104,	"Rf",	267.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 105,	"Db",	268.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 106,	"Sg",	269.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 107,	"Bh",	270.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 108,	"Hs",	269.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 109,	"Mt",	278.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 110,	"Ds",	281.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 111,	"Rg",	280.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 112,	"Cn",	285.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 113,	"Nh",	286.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 114,	"Fl",	289.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 115,	"Mc",	289.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 116,	"Lv",	293.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 117,	"Ts",	294.0			},	// Assumed mass (http://www.rsc.org/periodic-table)
		{ 118,	"Og",	294.0			}	// Assumed mass (http://www.rsc.org/periodic-table)
	};

	if ((Z < 0) || (Z > nElements()))
	{
		Messenger::error("AtomicMass::mass() - Element with Z=%i is out of range!\n", Z);
		return massData[0];
	}

	return massData[Z];
}

// Return atomic mass for specified Element
double AtomicMass::mass(Element* element)
{
#ifdef CHECKS
	if (element == NULL)
	{
		Messenger::error("AtomicMass::mass() - Element is NULL.\n");
		return 0.0;
	}
#endif

	return massData(element->Z()).mass();
}

// Return atomic mass for specified Z
double AtomicMass::mass(int Z)
{
	return massData(Z).mass();
}
