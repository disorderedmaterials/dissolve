/*
	*** Element Colours
	*** src/data/elementcolours.cpp
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

#include "data/elementcolours.h"
#include "base/messenger.h"

// Static Singletons
Array< List<ElementColour> > ElementColours::coloursByElementPrivate_;

/*
 * Isotopic Neutron Scattering Data
 */

// Constructor
ElementColour::ElementColour(int z, float r, float g, float b, float a) : ElementReference(z), ListItem<ElementColour>()
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
		{ ELEMENT_XX,	0.5,	0.5,	0.5,	1.0 },
		{ ELEMENT_H,	0.87,	0.87,	0.87,	1.0 },
		{ ELEMENT_HE,	1.0,	0.784,	0.784,	1.0 },
		{ ELEMENT_LI,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_BE,	0.0,	0.0,	0.0,	1.0 },
		{ ELEMENT_B,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_C,	0.0,	1.0,	0.2,	1.0 },
		{ ELEMENT_N,	0.561,	0.561,	1.0,	1.0 },
		{ ELEMENT_O,	1.0,	0.0,	0.0,	1.0 },
		{ ELEMENT_F,	0.784,	0.647,	0.094,	1.0 },
		{ ELEMENT_NE,	0.0,	0.0,	0.0,	1.0 },
		{ ELEMENT_NA,	0.0,	0.0,	1.0,	1.0 },
		{ ELEMENT_MG,	0.0,	0.0,	0.0,	1.0 },
		{ ELEMENT_AL,	0.561,	0.561,	1.0,	1.0 },
		{ ELEMENT_SI,	0.784,	0.647,	0.094,	1.0 },
		{ ELEMENT_P,	1.0,	0.647,	0.0,	1.0 },
		{ ELEMENT_S,	1.0,	0.784,	0.196,	1.0 },
		{ ELEMENT_CL,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_AR,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_K,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_CA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_SC,	1.0,	0.86,	0.66,	1.0 },
		{ ELEMENT_TI,	0.7,	0.7,	0.7,	1.0 },
		{ ELEMENT_V,	0.7,	0.7,	0.7,	1.0 },
		{ ELEMENT_CR,	0.7,	0.7,	0.7,	1.0 },
		{ ELEMENT_MN,	0.7,	0.7,	0.7,	1.0 },
		{ ELEMENT_FE,	1.0,	0.647,	0.0,	1.0 },
		{ ELEMENT_CO,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_NI,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_CU,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_ZN,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_GA,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_GE,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_AS,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_SE,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_BR,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_KR,	0.6,	0.6,	0.6,	1.0 },
		{ ELEMENT_RB,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_SR,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_Y,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_ZR,	0.784,	0.647,	0.094,	1.0 },
		{ ELEMENT_NB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_MO,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TC,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RU,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RH,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PD,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_AG,	0.6,	0.6,	0.6,	1.0 },
		{ ELEMENT_CD,	0.0,	1.0,	0.0,	1.0 },
		{ ELEMENT_IN,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_SN,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_SB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TE,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_I,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_XE,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_CS,	0.647,	0.165,	0.165,	1.0 },
		{ ELEMENT_BA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_LA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_CE,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PR,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_ND,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_SM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_EU,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_GD,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_DY,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_HO,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_ER,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_YB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_LU,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_HF,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_W,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RE,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_OS,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_IR,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PT,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_AU,	0.784,	0.647,	0.094,	1.0 },
		{ ELEMENT_HG,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TL,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_BI,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PO,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_AT,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RN,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_FR,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_AC,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TH,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PA,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_U,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_NP,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_PU,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_AM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_CM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_BK,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_CF,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_ES,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_FM,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_MD,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_NO,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_LR,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RF,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_DB,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_SG,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_BH,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_HS,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_MT,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_DS,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_RG,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_CN,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_NH,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_FL,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_MC,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_LV,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_TS,	1.0,	1.0,	1.0,	1.0 },
		{ ELEMENT_OG,	1.0,	1.0,	1.0,	1.0 }
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
