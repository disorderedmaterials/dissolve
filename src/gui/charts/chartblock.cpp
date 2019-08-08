/*
	*** Chart Block - Functions
	*** src/gui/charts/chartblock.cpp
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

#include "gui/charts/chartblock.h"
#include <QPropertyAnimation>
#include <QWidget>

// Constructor
ChartBlock::ChartBlock() : ListItem<ChartBlock>()
{
}

ChartBlock::~ChartBlock()
{
}

/*
 * Widget
 */

// Set underlying widget position
void ChartBlock::setNewPosition(int left, int top)
{
	newGeometry_.setLeft(left);
	newGeometry_.setTop(top);
}

// Set underlying widget geometry
void ChartBlock::setNewSize(int width, int height)
{
	newGeometry_.setWidth(width);
	newGeometry_.setHeight(height);
}

// Commit new geometry for widget
void ChartBlock::setNewGeometry(bool animate)
{
	// Set the widget's geometry based on these coordinates and its SizeHint - we give it all the space it needs
	if (animate)
	{
		QPropertyAnimation* animation = new QPropertyAnimation(widget(), "geometry");
		animation->setDuration(100);
		animation->setEndValue(newGeometry_);
		animation->start();
	}
	else widget()->setGeometry(newGeometry_);
}
