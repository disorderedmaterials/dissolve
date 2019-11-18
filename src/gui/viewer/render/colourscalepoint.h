/*
	*** Colour Scale Point
	*** src/gui/viewer/render/colourscalepoint.h
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

#ifndef DISSOLVE_COLOURSCALEPOINT_H
#define DISSOLVE_COLOURSCALEPOINT_H

#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
/* none */

// Colour Scale Point
class ColourScalePoint
{
	public:
	// Constructor
	ColourScalePoint(double value = 0.0, QColor colour = QColor());
	// Destructor
	~ColourScalePoint();
	// Copy Constructor
	ColourScalePoint(const ColourScalePoint& source);
	// Assignment operator
	void operator=(const ColourScalePoint& source);	


	/*
	 * Value and Colour
	 */
	private:
	// Value at which this point occurs
	double value_;
	// Colour of this point
	QColor colour_;

	public:
	// Set value and colour for point
	void set(double value, QColor colour);
	// Set value for point
	void setValue(double value);
	// Return value for point
	double value() const;
	// Set colour
	void setColour(QColor colour);
	// Return colour
	QColor colour() const;
	// Return colour as GLfloat[4]
	void colour(GLfloat* rgba) const;
	// Set alpha value
	void setAlpha(int alpha);
};

#endif
