/*
	*** Colour Button
	*** src/gui/widgets/colourbutton_funcs.cpp
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

#include "gui/widgets/colourbutton.hui"
#include <QPainter>
#include <QColorDialog>

/*
 * ColourButton
 */

// Constructor
ColourButton::ColourButton(QWidget* parent) : QPushButton(parent)
{
}

// Set displayed colour
void ColourButton::setColour(QColor colour)
{
	colour_ = colour;
	repaint();
}

// Return displayed colour
QColor ColourButton::colour()
{
	return colour_;
}

// Select new colour
bool ColourButton::selectColour()
{
	// Request a colour dialog
	QColor newcol = QColorDialog::getColor(colour_.rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
	if (!newcol.isValid()) return false;
	
	colour_ = newcol;
	repaint();
	return true;
}

// Paint event callback
void ColourButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	// Draw single rectangle and we're done
	QBrush brush(colour_);
	QRectF rect(3.0, 3.0, width()-3.0, height()-3.0);
	painter.setBrush(brush);
	painter.drawRect(rect);
	painter.end();
}
