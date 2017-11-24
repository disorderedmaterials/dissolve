/*
	*** Module Widget
	*** src/modules/skeleton/gui/modulewidget.h
	Copyright T. Youngs 2007-2017

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

#ifndef DUQ_SKELETONMODULEWIDGET_H
#define DUQ_SKELETONMODULEWIDGET_H

#include "modules/skeleton/gui/ui_modulewidget.h"

// Forward Declarations
class Module;

// Module Widget
class SkeletonModuleWidget : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	Module* module_;
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Constructor
	SkeletonModuleWidget(QWidget* parent, Module* module);
	// Main form declaration
	Ui::SkeletonModuleWidget ui;
	// Update controls within widget
	void updateControls();


	/*
	 * Widgets / Functions
	 */
	private:

	private slots:
};

#endif
