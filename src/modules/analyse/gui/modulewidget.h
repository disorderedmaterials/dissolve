/*
	*** Module Widget
	*** src/modules/analyse/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_ANALYSE_H
#define DISSOLVE_MODULEWIDGET_ANALYSE_H

#include "modules/analyse/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class AnalyseModule;
class CoreData;
class Module;

// Module Widget
class AnalyseModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor
	AnalyseModuleWidget(QWidget* parent, AnalyseModule* module, const CoreData& coreData);

	private:
	// Associated Module
	AnalyseModule* module_;


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::AnalyseModuleWidget ui_;
};

#endif
