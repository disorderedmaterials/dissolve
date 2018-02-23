/*
	*** Gradient Bar
	*** src/gui/widgets/gradientbar.cpp
	Copyright T. Youngs 2013-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/widgets/gradientbar.hui"
#include <QPainter>

/*
 * Gradient Bar
 */

// Constructor
GradientBar::GradientBar(QWidget* parent) : QWidget(parent)
{
	// Private variables

}

// Set local colourscale
void GradientBar::setColourScale(const ColourScale& colourScale)
{
	// If the source colourscale uses deltaHSV_, we must construct an interpolated gradient manually since QGradient doesn't support HSV interpolation
	if (colourScale.useHSV())
	{
		colourScale_.clear();
		colourScale_.setUseHSV(false);
		if (colourScale.nPoints() > 0)
		{
			const int nPoints = 101;
			double value = colourScale.firstPoint()->value();
			double delta = (colourScale.lastPoint()->value() - value) / nPoints;
			for (int n=0; n<nPoints; ++n)
			{
				colourScale_.addPoint(value, colourScale.colour(value));
				value += delta;
			}
		}
	}
	else colourScale_ = colourScale;
	
	// Setup QGradient - in ObjectBoundingMode, 0.0 = top of rectangle, and 1.0 is bottom
	gradient_ = QLinearGradient(0.0, 1.0, 0.0, 0.0);
	gradient_.setCoordinateMode(QGradient::ObjectBoundingMode);

	// -- Loop backwards through points
	if (colourScale_.nPoints() == 0)
	{
		gradient_.setColorAt(0.0, QColor(0,0,0));
		gradient_.setColorAt(1.0, QColor(0,0,0));
	}
	else if (colourScale_.nPoints() == 1)
	{
		gradient_.setColorAt(0.0, colourScale_.firstPoint()->colour());
		gradient_.setColorAt(1.0, colourScale_.firstPoint()->colour());
	}
	else
	{
		double zero = colourScale_.firstPoint()->value();
		double span = colourScale_.lastPoint()->value() - zero;
		for (ColourScalePoint* csp = colourScale_.firstPoint(); csp != NULL; csp = csp->next) gradient_.setColorAt((csp->value() - zero) / span, csp->colour());
	}
	
	repaint();
}

// Paint event callback
void GradientBar::paintEvent(QPaintEvent *event)
{
	if (colourScale_.nPoints() == 0) return;

	QPainter painter(this);
	
	// Draw single rectangle and we're done
	QBrush brush(gradient_);
	QPen pen(Qt::NoPen);
	QRectF rect(0.0, 0.0, width()-1.0, height()-1.0);
	painter.setBrush(brush);
	painter.setPen(pen);
	painter.drawRect(rect);
	painter.end();
}
