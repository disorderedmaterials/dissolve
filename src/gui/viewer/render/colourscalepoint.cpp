/*
	*** Colour Scale Point
	*** src/gui/viewer/render/colourscalepoint.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/render/colourscalepoint.h"

// Constructor
ColourScalePoint::ColourScalePoint(double value, QColor colour)
{
	// Private variables
	value_ = value;
	setColour(colour_);
}
/*
 * Value and Colour
 */

// Set value and colour for point
void ColourScalePoint::set(double value, QColor colour)
{
	value_ = value;
	colour_ = colour;
}

// Set value of point
void ColourScalePoint::setValue(double d)
{
	value_ = d;
}

// Return value of point
double ColourScalePoint::value() const
{
	return value_;
}

// Set colour
void ColourScalePoint::setColour(QColor colour)
{
	colour_ = colour;
}

// Return colour
QColor ColourScalePoint::colour() const
{
	return colour_;
}

// Return colour as GLfloat*
void ColourScalePoint::colour(GLfloat* rgba) const
{
	rgba[0] = colour_.redF();
	rgba[1] = colour_.greenF();
	rgba[2] = colour_.blueF();
	rgba[3] = colour_.alphaF();
}

// Set alpha value
void ColourScalePoint::setAlpha(int alpha)
{
	colour_.setAlpha(alpha);
}
