/*
	*** ColourScale
	*** src/gui/uchroma/classes/colourscale.h
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMACOLOURSCALE_H
#define DUQ_UCHROMACOLOURSCALE_H

#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
/* none */

// Colour scale point
class ColourScalePoint : public ListItem<ColourScalePoint>
{
	public:
	// Constructor
	ColourScalePoint();
	// Friend class
	friend class ColourScaleDelta;

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
	// Return colour as GLfloat*
	void colour(Vec4<GLfloat>& target) const;
};

// Colour Scale Delta
class ColourScaleDelta : public ListItem<ColourScaleDelta>
{
	public:
	// Constructor
	ColourScaleDelta();

	private:
	// Value at which the delta starts
	double start_;
	// Range of the data from the startValue
	double delta_;
	// Colour of this starting point
	QColor startColour_;
	// Delta between the starting point and the final point
	float deltaColourF_[4];

	public:
	// Check whether the delta 'contains' the supplied value
	bool containsValue(double d) const;
	// Create delta from two existing colours
	void set(ColourScalePoint* point1, ColourScalePoint* point2, bool useHSV);
	// Get colour for value, assuming that v is within the range 0 -> value_
	QColor colour(double v, bool useHSV) const;
	// Get colour for value as GLfloat*, assuming that v is within the range 0 -> value_
	void colour(double v, bool useHSV, Vec4<GLfloat>& target) const;
	// Return the starting value of the range
	double start() const;
	// Return the range of the delta
	double delta() const;
};

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


	/*
	 * Style
	 */
	private:
	// Whether colours are interpolated between points in the scale
	bool interpolated_;
	// Whether interpolation should be performed with HSV instead of RGB
	bool useHSV_;

	public:
	// Set whether the colourscale is interpolated
	void setInterpolated(bool b);
	// Return whether the colourscale is interpolated
	bool interpolated() const;
	// Set whether or not to use HSV interpolation
	void setUseHSV(bool b);
	// Return whether or not to use HSV interpolation
	bool useHSV() const;


	/*
	 * Colour Points
	 */
	private:
	// List of points in the colourscale
	List<ColourScalePoint> points_;
	// List of colour deltas between points in the colourscale
	List<ColourScaleDelta> deltas_;

	public:
	// Return number of points in colourscale
	int nPoints() const;
	// Return first point in colourscale
	ColourScalePoint* firstPoint() const;
	// Return last point in colourscale
	ColourScalePoint* lastPoint() const;
	// Return specific point in colourscale
	ColourScalePoint* point(int id);
	// Return first delta in colourscale
	ColourScaleDelta* firstDelta() const;
	// Clear all points in colourscale
	void clear();
	// Calculate colour deltas for current list of points
	void calculateDeltas();
	// Add new point to colourscale
	ColourScalePoint* addPoint(double value, QColor colour);
	// Set colour and value data for point
	void setPoint(int position, double value, QColor colour, bool setval = true, bool setcol = true);
	// Set only value for point
	void setPointValue(int position, double value);
	// Set only colour for point
	void setPointColour(int position, QColor colour);
	// Remove old point from colourscale
	void removePoint(int position);
	// Remove specified point from colourscale
	void removePoint(ColourScalePoint* point);
	// Get colour associated with value supplied
	QColor colour(double value) const;
	// Get colour associated with value supplied (as Vec4<GLfloat>)
	void colour(double value, Vec4<GLfloat>& target) const;
	// Apply colour associated with value
	void applyColour(double value) const;
	// Set all alpha values to that specified
	void setAllAlpha(double alpha);
};

#endif
