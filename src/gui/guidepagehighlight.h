/*
	*** Guide Page Highlight
	*** src/gui/guidepagehighlight.h
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

#ifndef DISSOLVE_GUIDEPAGEHIGHLIGHT_H
#define DISSOLVE_GUIDEPAGEHIGHLIGHT_H

#include "base/charstring.h"
#include "templates/listitem.h"
#include <QPalette>

// Forward Declarations
/* none */

// Guide Page Highlight Definition
class GuidePageHighlight : public ListItem<GuidePageHighlight>
{
	public:
	// Constructor / Destructor
	GuidePageHighlight();
	~GuidePageHighlight();


	/*
	 * Data
	 */
	private:
	// Target widget name
	CharString widgetName_;
	// Highlight colour
	QColor colour_;

	public:
	// Set target widget name
	void setWidgetName(const char* name);
	// Return target widget name
	const char* widgetName() const;
	// Set highlight colour
	void setColour(int r, int g, int b, int a = 255);
	// Return highlight colour
	QColor colour() const;
};

#endif
