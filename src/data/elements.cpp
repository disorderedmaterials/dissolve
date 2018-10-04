/*
	*** Element Data
	*** src/data/elements.cpp
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

#include "data/elements.h"
#include "base/charstring.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <ctype.h>

/*
 * Element
 */

// Constructor
Element::Element(int Z, const char* name, const char* symbol)
{
	Z_ = Z;
	name_ = name;
	symbol_ = symbol;
}

/*
 * Element Information
 */

// Return atomic number (Z)
int Element::Z() const
{
	return Z_;
}

// Return name of element
const char* Element::name() const
{
	return name_;
}

// Return symbol of element
const char* Element::symbol() const
{
	return symbol_;
}

/*
 * Elements Helper Class
 */

// Instantiate / return array of element data
Element* Elements::elements()
{
	// Basic element data
	static Element elementData[] = {
		{ 0,	"Unknown",	"XX" },
		{ 1,	"Hydrogen",	"H" },
		{ 2,	"Helium",	"He" },
		{ 3,	"Lithium",	"Li" },
		{ 4,	"Beryllium",	"Be" },
		{ 5,	"Boron",	"B" },
		{ 6,	"Carbon",	"C" },
		{ 7,	"Nitrogen",	"N" },
		{ 8,	"Oxygen",	"O" },
		{ 9,	"Fluorine",	"F" },
		{ 10,	"Neon",		"Ne" },
		{ 11,	"Sodium",	"Na" },
		{ 12,	"Magnesium",	"Mg" },
		{ 13,	"Aluminium",	"Al" },
		{ 14,	"Silicon",	"Si" },
		{ 15,	"Phosphorus",	"P" },
		{ 16,	"Sulfur",	"S" },
		{ 17,	"Chlorine",	"Cl" },
		{ 18,	"Argon",	"Ar" },
		{ 19,	"Potassium",	"K" },
		{ 20,	"Calcium",	"Ca" },
		{ 21,	"Scandium",	"Sc" },
		{ 22,	"Titanium",	"Ti" },
		{ 23,	"Vanadium",	"V" },
		{ 24,	"Chromium",	"Cr" },
		{ 25,	"Manganese",	"Mn" },
		{ 26,	"Iron",		"Fe" },
		{ 27,	"Cobalt",	"Co" },
		{ 28,	"Nickel",	"Ni" },
		{ 29,	"Copper",	"Cu" },
		{ 30,	"Zinc",		"Zn" },
		{ 31,	"Gallium",	"Ga" },
		{ 32,	"Germanium",	"Ge" },
		{ 33,	"Arsenic",	"As" },
		{ 34,	"Selenium",	"Se" },
		{ 35,	"Bromine",	"Br" },
		{ 36,	"Krypton",	"Kr" },
		{ 37,	"Rubidium",	"Rb" },
		{ 38,	"Strontium",	"Sr" },
		{ 39,	"Yttrium",	"Y" },
		{ 40,	"Zirconium",	"Zr" },
		{ 41,	"Niobium",	"Nb" },
		{ 42,	"Molybdenum",	"Mo" },
		{ 43,	"Technetium",	"Tc" },
		{ 44,	"Ruthenium",	"Ru" },
		{ 45,	"Rhodium",	"Rh" },
		{ 46,	"Palladium",	"Pd" },
		{ 47,	"Silver",	"Ag" },
		{ 48,	"Cadmium",	"Cd" },
		{ 49,	"Indium",	"In" },
		{ 50,	"Tin",		"Sn" },
		{ 51,	"Antimony",	"Sb" },
		{ 52,	"Tellurium",	"Te" },
		{ 53,	"Iodine",	"I" },
		{ 54,	"Xenon",	"Xe" },
		{ 55,	"Caesium",	"Cs" },
		{ 56,	"Barium",	"Ba" },
		{ 57,	"Lanthanum",	"La" },
		{ 58,	"Cerium",	"Ce" },
		{ 59,	"Praesodymium",	"Pr" },
		{ 60,	"Neodymium",	"Nd" },
		{ 61,	"Prometheum",	"Pm" },
		{ 62,	"Samarium",	"Sm" },
		{ 63,	"Europium",	"Eu" },
		{ 64,	"Gadolinium",	"Gd" },
		{ 65,	"Terbium",	"Tb" },
		{ 66,	"Dysprosium",	"Dy" },
		{ 67,	"Holmium",	"Ho" },
		{ 68,	"Erbium",	"Er" },
		{ 69,	"Thulium",	"Tm" },
		{ 70,	"Ytterbium",	"Yb" },
		{ 71,	"Lutetium",	"Lu" },
		{ 72,	"Hafnium",	"Hf" },
		{ 73,	"Tantalum",	"Ta" },
		{ 74,	"Tungsten",	"W" },
		{ 75,	"Rhenium",	"Re" },
		{ 76,	"Osmium",	"Os" },
		{ 77,	"Iridium",	"Ir" },
		{ 78,	"Platinum",	"Pt" },
		{ 79,	"Gold",		"Au" },
		{ 80,	"Mercury",	"Hg" },
		{ 81,	"Thallium",	"Tl" },
		{ 82,	"Lead",		"Pb" },
		{ 83,	"Bismuth",	"Bi" },
		{ 84,	"Polonium",	"Po" },
		{ 85,	"Astatine",	"At" },
		{ 86,	"Radon",	"Rn" },
		{ 87,	"Francium",	"Fr" },
		{ 88,	"Radium",	"Ra" },
		{ 89,	"Actinium",	"Ac" },
		{ 90,	"Thorium",	"Th" },
		{ 91,	"Protactinium",	"Pa" },
		{ 92,	"Uranium",	"U" },
		{ 93,	"Neptunium",	"Np" },
		{ 94,	"Plutonium",	"Pu" },
		{ 95,	"Americium",	"Am" },
		{ 96,	"Curium",	"Cm" },
		{ 97,	"Berkelium",	"Bk" },
		{ 98,	"Californium",	"Cf" },
		{ 99,	"Einsteinium",	"Es" },
		{ 100,	"Fermium",	"Fm" },
		{ 101,	"Mendelevium",	"Md" },
		{ 102,	"Nobelium",	"No" },
		{ 103,	"Lawrencium",	"Lr" },
		{ 104,	"Rutherfordium","Rf" },
		{ 105,	"Dubnium",	"Db" },
		{ 106,	"Seaborgium",	"Sg" },
		{ 107,	"Bohrium",	"Bh" },
		{ 108,	"Hassium",	"Hs" },
		{ 109,	"Meitnerium",	"Mt" },
		{ 110,	"Darmstadtium",	"Ds" },
		{ 111,	"Roentgenium",	"Rg" },
		{ 112,	"Copernicium",	"Cn" },
		{ 113,	"Nihonium",	"Nh" },
		{ 114,	"Flerovium",	"Fl" },
		{ 115,	"Moscovium",	"Mc" },
		{ 116,	"Livermorium",	"Lv" },
		{ 117,	"Tennessine",	"Ts" },
		{ 118,	"Oganesson",	"Og" }
	};

	return elementData;
}

// Return Element with corresponding Z 
Element& Elements::element(int Z)
{
	if ((Z < 0) || (Z > nElements()))
	{
		Messenger::error("Element with Z=%i is out of range!\n", Z);
		return elements()[0];
	}
	
	return elements()[Z];
}

// Return Element with corresponding symbol
Element& Elements::element(const char* symbol)
{
	static CharString cleaned;
	cleaned.clear();
	int n, nDigits = 0, count = 0;
	for (n=0; symbol[n] != '\0'; n++)
	{
		if (symbol[n] == ' ') continue;
		else if ((symbol[n] > 64) && (symbol[n] < 91)) cleaned += (count == 0 ? symbol[n] : tolower(symbol[n]));
		else if ((symbol[n] > 96) && (symbol[n] < 123)) cleaned += (count == 0 ? toupper(symbol[n]) : symbol[n]);
		else if ((symbol[n] > 47) && (symbol[n] < 58)) { cleaned += symbol[n]; ++nDigits; }
		else break;
		++count;
	}

	// Pure digit given?
	if (count == nDigits)
	{
		int Z = atoi(cleaned.get());
		if ((Z < 0) || (Z > nElements())) return elements()[0];
		else return elements()[Z];
	}
	else for (n=0; n<nElements(); n++) if (cleaned == elements()[n].symbol()) return elements()[n];

	return elements()[0];
}

// Return pointer to Element with corresponding Z
Element* Elements::elementPointer(int Z)
{
	return &element(Z);
}

// Return pointer to Element with corresponding symbol
Element* Elements::elementPointer(const char* symbol)
{
	return &element(symbol);
}

// Return total number of defined elements
int Elements::nElements()
{
	return 119;
}

// Return name of element with specified Z
const char* Elements::name(int Z)
{
	return element(Z).name();
}

// Return symbol of element with specified Z
const char* Elements::symbol(int Z)
{
	return element(Z).symbol();
}

/*
 * ElementReference
 */

// Constructor
ElementReference::ElementReference(int z, const char* symbol) : element_(Elements::element(z))
{
	// Check the basic element data and make sure that this is a valid Z/symbol combination
	if (!DissolveSys::sameString(symbol, element_.symbol())) Messenger::error("ElementReference::ElementReference() - Data is corrupted: element with Z = %i should have symbol '%s' but we were given the symbol '%s').\n", z, element_.symbol(), symbol);
}

// Destructor
ElementReference::~ElementReference()
{
}

// Return referenced element
const Element& ElementReference::element() const
{
	return element_;
}

// Return atomic number (Z)
int ElementReference::Z() const
{
	return element_.Z();
}

// Return name of element
const char* ElementReference::name() const
{
	return element_.name();
}

// Return symbol of element
const char* ElementReference::symbol() const
{
	return element_.symbol();
}

