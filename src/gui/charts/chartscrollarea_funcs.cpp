/*
	*** Chart Scroll Area
	*** src/gui/charts/chartscrollarea_funcs.cpp
	Copyright T. Youngs 2012-2020

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

#include "gui/charts/chartscrollarea.hui"
#include <QScrollBar>

// Constructor
ChartScrollArea::ChartScrollArea(QWidget* parent) : QScrollArea(parent)
{
}

ChartScrollArea::~ChartScrollArea()
{
}

/*
 * QScrollArea Reimplementations
 */

// Return size hint
QSize ChartScrollArea::sizeHint() const
{
	QSize s = viewportSizeHint();
	if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) s.setHeight(s.height() + horizontalScrollBar()->sizeHint().height());
	if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) s.setWidth(s.width() + verticalScrollBar()->sizeHint().width());
	return s;
}

// Return viewport size hint
QSize ChartScrollArea::viewportSizeHint() const
{
	return (widget() ? widget()->sizeHint() : QSize(50,50));
}
