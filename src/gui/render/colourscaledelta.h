/*
	*** Colour Scale Delta
	*** src/gui/render/colourscaledelta.h
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

#ifndef DISSOLVE_RENDER_COLOURSCALEDELTA_H
#define DISSOLVE_RENDER_COLOURSCALEDELTA_H

#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
class ColourScalePoint;

// Colour Scale Delta
class ColourScaleDelta
{
	public:
	// Constructors
	ColourScaleDelta();
	ColourScaleDelta(const ColourScalePoint& start, const ColourScalePoint& end, bool useHSV);

	private:
	// Value at which the delta starts
	double start_;
	// Range of the data from the start value
	double delta_;
	// Colour of the delta's starting point
	QColor startColour_;
	// Delta between the starting point and the final point
	float deltaColourF_[4];
	// Whether to use HSV, rather than RGB, when generating colours
	bool useHSV_;

	public:
	// Check whether the delta 'contains' the supplied value
	bool containsValue(double d) const;
	// Create delta from ColourScalePoints
	void set(const ColourScalePoint& start, const ColourScalePoint& end, bool useHSV);
	// Return colour of the delta's starting point
	QColor startColour() const;
	// Return colour of the delta's starting point as GLfloat[4]
	void startColour(GLfloat* rgba) const;
	// Get colour for value, assuming that v is within the range 0 -> value_
	QColor colour(double v) const;
	// Get colour for value as GLfloat[4], assuming that v is within the range 0 -> value_
	void colour(double v, GLfloat* rgba) const;
	// Return the starting value of the range
	double start() const;
	// Return the range of the delta
	double delta() const;
};

#endif
