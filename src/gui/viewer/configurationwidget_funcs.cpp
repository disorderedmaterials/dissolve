/*
	*** Configuration Widget - Functions 
	*** src/gui/viewer/configurationwidget_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/configurationwidget.h"
#include "gui/widgets/elementselector.hui"
#include "classes/coredata.h"
#include "classes/empiricalformula.h"
#include "classes/configuration.h"
#include "main/dissolve.h"
#include <QButtonGroup>

// Constructor
ConfigurationWidget::ConfigurationWidget(QWidget* parent) : QWidget(parent)
{
	// Set up our UI
	ui_.setupUi(this);
}

// Destructor
ConfigurationWidget::~ConfigurationWidget()
{
}

// Return contained ConfigurationViewer
ConfigurationViewer* ConfigurationWidget::configurationViewer()
{
	return ui_.ConfigurationView;
}

/*
 * Toolbar
 */

/*
 * Signals / Slots
 */
