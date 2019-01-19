/*
	*** Wizard Widget Highlight
	*** src/gui/wizardwidgethighlight.h
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

#ifndef DISSOLVE_WIZARDWIDGETHIGHLIGHT_H
#define DISSOLVE_WIZARDWIDGETHIGHLIGHT_H

#include "templates/listitem.h"
// #include <QColor>
#include <QPalette>

// Forward Declarations
class QWidget;

// Wizard Widget Highlight Definition
class WizardWidgetHighlight : public ListItem<WizardWidgetHighlight>
{
	public:
	// Constructor / Destructor
	WizardWidgetHighlight();
	~WizardWidgetHighlight();


	/*
	 * Data
	 */
	private:
	// Target widget
	QWidget* widget_;
	// Original palette
	QPalette originalPalette_;
	// Highlight colour
	QColor colour_;

	public:
	// Set target widget
	void setWidget(QWidget* widget);
	// Set highlight colour
	void setColour(int r, int g, int b, int a = 255);


	/*
	 * Highlighting
	 */
	public:
	// Highlight the target widget
	void highlight();
	// Remove highlight from the target widget
	void unHighlight();
};

#endif
