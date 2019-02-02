/*
	*** Guide Page Highlight Functions
	*** src/gui/guidepagehighlight.cpp
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

#include "gui/guidepagehighlight.h"
#include <QWidget>

// Constructor
GuidePageHighlight::GuidePageHighlight() : ListItem<GuidePageHighlight>()
{
}

// Destructor
GuidePageHighlight::~GuidePageHighlight()
{
}

/*
 * Data
 */

// Set target widget name
void GuidePageHighlight::setWidgetName(const char* name)
{
	widgetName_ = name;
}

// Return target widget name
const char* GuidePageHighlight::widgetName() const
{
	return widgetName_.get();
}

// Set highlight colour
void GuidePageHighlight::setColour(int r, int g, int b, int a)
{
	colour_.setRgb(r, g, b, a);
}

// Return highlight colour
QColor GuidePageHighlight::colour() const
{
	return colour_;
}
