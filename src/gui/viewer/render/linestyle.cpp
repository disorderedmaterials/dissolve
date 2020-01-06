/*
	*** Line Style
	*** src/gui/viewer/render/linestyle.cpp
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

#include "gui/viewer/render/linestyle.h"
#include <stdio.h>

// Constructor
LineStyle::LineStyle(double width, LineStipple::StippleType stipple, double r, double g, double b, double a)
{
	width_ = width;
	stipple_ = stipple;
	setColour(r, g, b, a);
}

// Destructor
LineStyle::~LineStyle()
{
}

// Copy constructor
LineStyle::LineStyle(const LineStyle& source)
{
	(*this) = source;
}

// Assignment operator
void LineStyle::operator=(const LineStyle& source)
{
	width_ = source.width_;
	stipple_ = source.stipple_;
	colour_[0] = source.colour_[0];
	colour_[1] = source.colour_[1];
	colour_[2] = source.colour_[2];
	colour_[3] = source.colour_[3];
}

/*
 * Style
 */

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple)
{
	width_ = width;
	stipple_ = stipple;
}

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, QColor colour)
{
	width_ = width;
	stipple_ = stipple;
	setColour(colour);
}

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, double r, double g, double b, double a)
{
	width_ = width;
	stipple_ = stipple;
	setColour(r, g, b, a);
}

// Set line width
void LineStyle::setWidth(double width)
{
	width_ = width;
}

// Return line width
double LineStyle::width() const
{
	return width_;
}

// Set line stipple
void LineStyle::setStipple(LineStipple::StippleType stipple)
{
	stipple_ = stipple;
}

// Return line stipple
LineStipple::StippleType LineStyle::stipple() const
{
	return stipple_;
}

// Set line colour
void LineStyle::setColour(double r, double g, double b, double a)
{
	colour_[0] = r;
	colour_[1] = g;
	colour_[2] = b;
	colour_[3] = a;
}

// Set line colour
void LineStyle::setColour(QColor colour)
{
	colour_[0] = colour.redF();
	colour_[1] = colour.greenF();
	colour_[2] = colour.blueF();
	colour_[3] = colour.alphaF();
}

// Return line colour
const GLfloat* LineStyle::colour() const
{
	return colour_;
}

/*
 * GL
 */

// Send line styling to GL
void LineStyle::sendToGL(const double pixelScaling) const
{
	// Set line width, including any supplied scaling factor
	glLineWidth(width_ * pixelScaling);

	// Enable / disable stippling
	if (stipple_ == LineStipple::NoStipple) glDisable(GL_LINE_STIPPLE);
	else
	{
		glEnable(GL_LINE_STIPPLE);
		LineStipple::stipple[stipple_].apply();
	}

	// Apply colour
	glColor4fv(colour_);
}
