/*
	*** Colour Scale
	*** src/gui/viewer/render/colourscale.h
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

#ifndef DISSOLVE_COLOURSCALE_H
#define DISSOLVE_COLOURSCALE_H

#include "gui/viewer/render/colourscaledelta.h"
#include "gui/viewer/render/colourscalepoint.h"
#include "templates/array.h"
#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
/* none */

// Colour Scale
class ColourScale
{
	public:
	// Constructor
	ColourScale();
	// Copy constructor
	ColourScale(const ColourScale&);
	// Assignment operator
	void operator=(const ColourScale& source);
	// Clear ColourScale
	void clear();

	/*
	 * Points
	 */
	private:
	// List of points in the ColourScale
	Array<ColourScalePoint> points_;

	public:
	// Add new point to ColourScale
	void addPoint(double value, QColor colour);
	// Return number of points in ColourScale
	int nPoints() const;
	// Return points in ColourScale
	const Array<ColourScalePoint>& points() const;
	// Return first point in ColourScale
	const ColourScalePoint& firstPoint() const;
	// Return last point in ColourScale
	const ColourScalePoint& lastPoint() const;
	// Return specific point in ColourScale
	const ColourScalePoint& point(int id) const;
	// Set colour and value data for point
	void setPoint(int position, double value, QColor colour);
	// Set value for specified point
	void setValue(int position, double value);
	// Set colour for specified point
	void setColour(int position, QColor colour);
	// Remove point from ColourScale
	void removePoint(int position);
	// Get colour associated with value supplied
	QColor colour(double value) const;
	// Get colour associated with value supplied, setting as GLfloat[4]
	void colour(double value, GLfloat* rgba) const;
	// Set all alpha values to that specified
	void setAllAlpha(double alpha);


	/*
	 * Deltas
	 */
	private:
	// List of colour deltas between points in the ColourScale
	Array<ColourScaleDelta> deltas_;

	private:
	// Calculate colour deltas for current list of points
	void calculateDeltas();


	/*
	 * Style
	 */
	private:
	// Whether colours are interpolated between points in the scale
	bool interpolated_;
	// Whether interpolation should be performed with HSV instead of RGB
	bool useHSV_;

	public:
	// Set whether the ColourScale is interpolated
	void setInterpolated(bool b);
	// Return whether the ColourScale is interpolated
	bool interpolated() const;
	// Set whether or not to use HSV interpolation
	void setUseHSV(bool b);
	// Return whether or not to use HSV interpolation
	bool useHSV() const;
};

#endif
