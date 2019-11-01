/*
	*** Element Colours
	*** src/data/elementcolours.cpp
	Copyright T. Youngs 2012-2019

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

#include "data/elementcolours.h"
#include "base/messenger.h"

// Static Singletons
Array< List<ElementColour> > ElementColours::coloursByElementPrivate_;

/*
 * Isotopic Neutron Scattering Data
 */

// Constructor
ElementColour::ElementColour(int z, const char* symbol, float r, float g, float b, float a) : ElementReference(z, symbol), ListItem<ElementColour>()
{
	// Set the colour data
	colour_[0] = r;
	colour_[1] = g;
	colour_[2] = b;
	colour_[3] = a;

	// Add this isotope to its parent element's list
	ElementColours::registerElementColour(this, z);
}

// Assignment Operator
ElementColour& ElementColour::operator=(const ElementColour& source)
{
	colour_[0] = source.colour_[0];
	colour_[1] = source.colour_[1];
	colour_[2] = source.colour_[2];
	colour_[3] = source.colour_[3];

	return (*this);
}

// Return colour
const float* ElementColour::colour() const
{
	return colour_;
}

/*
 * Element Colours
 */

// Return colour data, grouped by element
List<ElementColour>& ElementColours::coloursByElement(int Z)
{
	// Has the master array been initialised yet? If not, do it now, before the Sears data is constructed
	if (coloursByElementPrivate_.nItems() == 0)
	{
		/*
		 * Create the array, and set all Lists to only disown their items on destruction, rather than deleting them.
		 * Need to do this otherwise each Isotope will be destructed twice - once from the List<T> destructor, and once
		 * again from the destruction of the static array colourData.
		 */
		coloursByElementPrivate_.initialise(Elements::nElements());
		for (int n=0; n<Elements::nElements(); ++n) coloursByElementPrivate_[n].setDisownOnDestruction(true);
	}

	/* Copied from Aten */

	static ElementColour colourData[] = {
	//	Z	El	R	G	B	A
		{ 0,	"XX",	0.5,	0.5,	0.5,	1.0 },
		{ 1,	"H",	0.87,	0.87,	0.87,	1.0 },
		{ 2,	"He",	1.0,	0.784,	0.784,	1.0 },
		{ 3,	"Li",	0.647,	0.165,	0.165,	1.0 },
		{ 4,	"Be",	0.0,	0.0,	0.0,	1.0 },
		{ 5,	"B",	0.0,	1.0,	0.0,	1.0 },
		{ 6,	"C",	0.0,	1.0,	0.2,	1.0 },
		{ 7,	"N",	0.561,	0.561,	1.0,	1.0 },
		{ 8,	"O",	1.0,	0.0,	0.0,	1.0 },
		{ 9,	"F",	0.784,	0.647,	0.094,	1.0 },
		{ 10,	"Ne",	0.0,	0.0,	0.0,	1.0 },
		{ 11,	"Na",	0.0,	0.0,	1.0,	1.0 },
		{ 12,	"Mg",	0.0,	0.0,	0.0,	1.0 },
		{ 13,	"Al",	0.561,	0.561,	1.0,	1.0 },
		{ 14,	"Si",	0.784,	0.647,	0.094,	1.0 },
		{ 15,	"P",	1.0,	0.647,	0.0,	1.0 },
		{ 16,	"S",	1.0,	0.784,	0.196,	1.0 },
		{ 17,	"Cl",	0.0,	1.0,	0.0,	1.0 },
		{ 18,	"Ar",	0.0,	1.0,	0.0,	1.0 },
		{ 19,	"K",	0.0,	1.0,	0.0,	1.0 },
		{ 20,	"Ca",	1.0,	1.0,	1.0,	1.0 },
		{ 21,	"Sc",	1.0,	0.86,	0.66,	1.0 },
		{ 22,	"Ti",	0.7,	0.7,	0.7,	1.0 },
		{ 23,	"V",	0.7,	0.7,	0.7,	1.0 },
		{ 24,	"Cr",	0.7,	0.7,	0.7,	1.0 },
		{ 25,	"Mn",	0.7,	0.7,	0.7,	1.0 },
		{ 26,	"Fe",	1.0,	0.647,	0.0,	1.0 },
		{ 27,	"Co",	0.647,	0.165,	0.165,	1.0 },
		{ 28,	"Ni",	0.647,	0.165,	0.165,	1.0 },
		{ 29,	"Cu",	0.647,	0.165,	0.165,	1.0 },
		{ 30,	"Zn",	0.647,	0.165,	0.165,	1.0 },
		{ 31,	"Ga",	0.647,	0.165,	0.165,	1.0 },
		{ 32,	"Ge",	0.647,	0.165,	0.165,	1.0 },
		{ 33,	"As",	0.647,	0.165,	0.165,	1.0 },
		{ 34,	"Se",	0.647,	0.165,	0.165,	1.0 },
		{ 35,	"Br",	0.647,	0.165,	0.165,	1.0 },
		{ 36,	"Kr",	0.6,	0.6,	0.6,	1.0 },
		{ 37,	"Rb",	0.0,	1.0,	0.0,	1.0 },
		{ 38,	"Sr",	0.0,	1.0,	0.0,	1.0 },
		{ 39,	"Y",	0.647,	0.165,	0.165,	1.0 },
		{ 40,	"Zr",	0.784,	0.647,	0.094,	1.0 },
		{ 41,	"Nb",	1.0,	1.0,	1.0,	1.0 },
		{ 42,	"Mo",	1.0,	1.0,	1.0,	1.0 },
		{ 43,	"Tc",	1.0,	1.0,	1.0,	1.0 },
		{ 44,	"Ru",	1.0,	1.0,	1.0,	1.0 },
		{ 45,	"Rh",	1.0,	1.0,	1.0,	1.0 },
		{ 46,	"Pd",	1.0,	1.0,	1.0,	1.0 },
		{ 47,	"Ag",	0.6,	0.6,	0.6,	1.0 },
		{ 48,	"Cd",	0.0,	1.0,	0.0,	1.0 },
		{ 49,	"In",	1.0,	1.0,	1.0,	1.0 },
		{ 50,	"Sn",	1.0,	1.0,	1.0,	1.0 },
		{ 51,	"Sb",	1.0,	1.0,	1.0,	1.0 },
		{ 52,	"Te",	1.0,	1.0,	1.0,	1.0 },
		{ 53,	"I",	1.0,	1.0,	1.0,	1.0 },
		{ 54,	"Xe",	1.0,	1.0,	1.0,	1.0 },
		{ 55,	"Cs",	0.647,	0.165,	0.165,	1.0 },
		{ 56,	"Ba",	1.0,	1.0,	1.0,	1.0 },
		{ 57,	"La",	1.0,	1.0,	1.0,	1.0 },
		{ 58,	"Ce",	1.0,	1.0,	1.0,	1.0 },
		{ 59,	"Pr",	1.0,	1.0,	1.0,	1.0 },
		{ 60,	"Nd",	1.0,	1.0,	1.0,	1.0 },
		{ 61,	"Pm",	1.0,	1.0,	1.0,	1.0 },
		{ 62,	"Sm",	1.0,	1.0,	1.0,	1.0 },
		{ 63,	"Eu",	1.0,	1.0,	1.0,	1.0 },
		{ 64,	"Gd",	1.0,	1.0,	1.0,	1.0 },
		{ 65,	"Tb",	1.0,	1.0,	1.0,	1.0 },
		{ 66,	"Dy",	1.0,	1.0,	1.0,	1.0 },
		{ 67,	"Ho",	1.0,	1.0,	1.0,	1.0 },
		{ 68,	"Er",	1.0,	1.0,	1.0,	1.0 },
		{ 69,	"Tm",	1.0,	1.0,	1.0,	1.0 },
		{ 70,	"Yb",	1.0,	1.0,	1.0,	1.0 },
		{ 71,	"Lu",	1.0,	1.0,	1.0,	1.0 },
		{ 72,	"Hf",	1.0,	1.0,	1.0,	1.0 },
		{ 73,	"Ta",	1.0,	1.0,	1.0,	1.0 },
		{ 74,	"W",	1.0,	1.0,	1.0,	1.0 },
		{ 75,	"Re",	1.0,	1.0,	1.0,	1.0 },
		{ 76,	"Os",	1.0,	1.0,	1.0,	1.0 },
		{ 77,	"Ir",	1.0,	1.0,	1.0,	1.0 },
		{ 78,	"Pt",	1.0,	1.0,	1.0,	1.0 },
		{ 79,	"Au",	0.784,	0.647,	0.094,	1.0 },
		{ 80,	"Hg",	1.0,	1.0,	1.0,	1.0 },
		{ 81,	"Tl",	1.0,	1.0,	1.0,	1.0 },
		{ 82,	"Pb",	1.0,	1.0,	1.0,	1.0 },
		{ 83,	"Bi",	1.0,	1.0,	1.0,	1.0 },
		{ 84,	"Po",	1.0,	1.0,	1.0,	1.0 },
		{ 85,	"At",	1.0,	1.0,	1.0,	1.0 },
		{ 86,	"Rn",	1.0,	1.0,	1.0,	1.0 },
		{ 87,	"Fr",	1.0,	1.0,	1.0,	1.0 },
		{ 88,	"Ra",	1.0,	1.0,	1.0,	1.0 },
		{ 89,	"Ac",	1.0,	1.0,	1.0,	1.0 },
		{ 90,	"Th",	1.0,	1.0,	1.0,	1.0 },
		{ 91,	"Pa",	1.0,	1.0,	1.0,	1.0 },
		{ 92,	"U",	1.0,	1.0,	1.0,	1.0 },
		{ 93,	"Np",	1.0,	1.0,	1.0,	1.0 },
		{ 94,	"Pu",	1.0,	1.0,	1.0,	1.0 },
		{ 95,	"Am",	1.0,	1.0,	1.0,	1.0 },
		{ 96,	"Cm",	1.0,	1.0,	1.0,	1.0 },
		{ 97,	"Bk",	1.0,	1.0,	1.0,	1.0 },
		{ 98,	"Cf",	1.0,	1.0,	1.0,	1.0 },
		{ 99,	"Es",	1.0,	1.0,	1.0,	1.0 },
		{ 100,	"Fm",	1.0,	1.0,	1.0,	1.0 },
		{ 101,	"Md",	1.0,	1.0,	1.0,	1.0 },
		{ 102,	"No",	1.0,	1.0,	1.0,	1.0 },
		{ 103,	"Lr",	1.0,	1.0,	1.0,	1.0 },
		{ 104,	"Rf",	1.0,	1.0,	1.0,	1.0 },
		{ 105,	"Db",	1.0,	1.0,	1.0,	1.0 },
		{ 106,	"Sg",	1.0,	1.0,	1.0,	1.0 },
		{ 107,	"Bh",	1.0,	1.0,	1.0,	1.0 },
		{ 108,	"Hs",	1.0,	1.0,	1.0,	1.0 },
		{ 109,	"Mt",	1.0,	1.0,	1.0,	1.0 },
		{ 110,	"Ds",	1.0,	1.0,	1.0,	1.0 },
		{ 111,	"Rg",	1.0,	1.0,	1.0,	1.0 },
		{ 112,	"Cn",	1.0,	1.0,	1.0,	1.0 },
		{ 113,	"Nh",	1.0,	1.0,	1.0,	1.0 },
		{ 114,	"Fl",	1.0,	1.0,	1.0,	1.0 },
		{ 115,	"Mc",	1.0,	1.0,	1.0,	1.0 },
		{ 116,	"Lv",	1.0,	1.0,	1.0,	1.0 },
		{ 117,	"Ts",	1.0,	1.0,	1.0,	1.0 },
		{ 118,	"Og",	1.0,	1.0,	1.0,	1.0 }
	};

	if ((Z < 0) || (Z > nElements()))
	{
		Messenger::error("ElementColours::coloursByElement() - Element with Z=%i is out of range!\n", Z);
		return coloursByElementPrivate_[0];
	}

	return coloursByElementPrivate_[Z];
}

// Register specified Isotope to given Element
void ElementColours::registerElementColour(ElementColour* ec, int Z)
{
	coloursByElementPrivate_[Z].own(ec);
}

// Return colour for specified Element
const float* ElementColours::colour(int Z)
{
	return coloursByElement(Z).first()->colour();
}

// Return colour for specified Element
const float* ElementColours::colour(Element* el)
{
	return coloursByElement(el->Z()).first()->colour();
}
