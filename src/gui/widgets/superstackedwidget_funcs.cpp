/*
	*** Super Stacked Widget
	*** src/gui/widgets/superstackedwidget_funcs.cpp
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

#include "gui/widgets/superstackedwidget.hui"
#include <QPainter>

// Constructor
SuperStackedWidget::SuperStackedWidget(QWidget* parent) : QStackedWidget(parent)
{
}

// Paint event
void SuperStackedWidget::paintEvent(QPaintEvent* event)
{
	if (currentIndex() != 0) return;

	QPainter painter(this);

	// Draw top-left hexagons
	painter.drawPixmap(0, 0, QPixmap(":/images/images/hexagons1.png"));

	// Draw the background before we do anything else
	painter.drawPixmap(width()-512, height()-527, QPixmap(":/images/images/big.png"));
}
