/*
	*** Configuration Widget
	*** src/gui/viewer/configurationwidget.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_CONFIGURATIONWIDGET_H
#define DISSOLVE_CONFIGURATIONWIDGET_H

#include "gui/viewer/ui_configurationwidget.h"

// Forward Declarations
/* none */

// Configuration Widget
class ConfigurationWidget : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ConfigurationWidget(QWidget* parent = 0);
	~ConfigurationWidget();

	private:
	// Main form declaration
	Ui::ConfigurationWidget ui_;

	public:
	// Return contained ConfigurationViewer
	ConfigurationViewer* configurationViewer();


	/*
	 * ToolBar
	 */
	private slots:
	/* Not Present */

	
	/*
	 * Signals / Slots
	 */
	public slots:
};

#endif
