/*
	*** Line Style
	*** src/gui/render/linestyle.h
	Copyright T. Youngs 2013-2020

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

#ifndef DISSOLVE_RENDER_LINESTYLE_H
#define DISSOLVE_RENDER_LINESTYLE_H

#include "gui/render/linestipple.h"
#include <QColor>

// Forward Declarations
/* none */

// Line Style
class LineStyle
{
	public:
	// Constructor / Destructor
	LineStyle(double width = 1.0, LineStipple::StippleType stipple = LineStipple::NoStipple, double r = 0.0, double g = 0.0, double b = 0.0, double a = 1.0);
	~LineStyle();
	// Copy constructor
	LineStyle(const LineStyle& source);
	// Assignment operator
	void operator=(const LineStyle& source);


	/*
	 * Style
	 */
	private:
	// Line width
	double width_;
	// Line stipple
	LineStipple::StippleType stipple_;
	// Line colour
	GLfloat colour_[4];

	public:
	// Set line style
	void set(double width, LineStipple::StippleType stipple);
	// Set line style and colour
	void set(double width, LineStipple::StippleType stipple, double r, double g, double b, double a = 1.0);
	// Set line style and colour
	void set(double width, LineStipple::StippleType stipple, QColor colour);
	// Set line width
	void setWidth(double width);
	// Return line width
	double width() const;
	// Set line stipple
	void setStipple(LineStipple::StippleType stipple);
	// Return line stipple
	LineStipple::StippleType stipple() const;
	// Set line colour
	void setColour(double r, double g, double b, double a = 1.0);
	// Set line colour
	void setColour(QColor colour);
	// Return line colour
	const GLfloat* colour() const;


	/*
	 * GL
	 */
	public:
	// Send line styling to GL
	void sendToGL(const double pixelScaling = 1.0) const;
};

#endif
