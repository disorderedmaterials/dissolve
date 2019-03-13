/*
	*** Element Colours
	*** src/data/elementcolours.h
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

#ifndef DISSOLVE_ELEMENTCOLOURS_H
#define DISSOLVE_ELEMENTCOLOURS_H

#include "data/elements.h"
#include "templates/array.h"
#include "templates/list.h"

// Isotopic Neutron Scattering Data
class ElementColour : public ElementReference, public ListItem<ElementColour>
{
	public:
	// Constructor
	ElementColour(int z = 0, const char* symbol = NULL, float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0);
	// Assignment Operator
	ElementColour& operator=(const ElementColour& source);

	/*
	 * Element Colour Data
	 */
	private:
	// Colour
	float colour_[4];

	public:
	// Return colour
	const float* colour() const;
};

// Element Colours
class ElementColours : public Elements
{
	private:
	// ElementColour data, grouped by element
	static Array< List<ElementColour> > coloursByElementPrivate_;

	private:
	// Return colour data for specified Element
	static List<ElementColour>& coloursByElement(int Z);

	public:
	// Register specified ElementColour to given Element
	static void registerElementColour(ElementColour* ec, int Z);
	// Return colour for specified Z
	static const float* colour(int Z);
	// Return colour for specified Element
	static const float* colour(Element* el);
};

#endif
