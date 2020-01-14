/*
	*** Module Widget
	*** src/modules/skeleton/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_SKELETON_H
#define DISSOLVE_MODULEWIDGET_SKELETON_H

#include "modules/skeleton/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class SkeletonModule;

// Module Widget
class SkeletonModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	SkeletonModule* module_;

	public:
	// Constructor
	SkeletonModuleWidget(QWidget* parent, SkeletonModule* module);


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::SkeletonModuleWidget ui_;
};

#endif
