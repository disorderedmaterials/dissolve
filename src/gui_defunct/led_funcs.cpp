/*
	*** LED Widget Functions
	*** src/gui/led_funcs.cpp
	Copyright T. Youngs 2012-2013

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

#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include "gui/led.uih"

/*!
 * \brief Constructor
 */
LED::LED(QWidget* parent) : QWidget(parent)
{
	resize(10,10);
}

/*!
 * \brief Destructor
 */
LED::~LED()
{
}

/*!
 * \brief Widget PaintEvent
 */
void LED::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QImage image(":/icons/style_sphere.svg");
	painter.drawImage(QPoint(10,10), image);
}
