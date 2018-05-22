/*
	*** Module Keywords Widget
	*** src/gui/modulekeywordswidget.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_MODULEKEYWORDSWIDGET_H
#define DUQ_MODULEKEYWORDSWIDGET_H

#include "gui/keywordwidgets/base.h"
#include "templates/reflist.h"
#include <QFrame>

// Forward Declarations
class DUQ;
class DUQWindow;
class Module;
class ModuleReference;

// Module Keywords Widget
class ModuleKeywordsWidget : public QFrame
{
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;
	// Module whose keywords we are displaying
	Module* module_;
	// Pointer to DUQWindow
	DUQWindow* duqWindow_;

	public:
	// Constructor / Destructor
	ModuleKeywordsWidget(QWidget* parent);
	~ModuleKeywordsWidget();


	/*
	 * Keywords
	 */
	private:
	// List of keyword widgets displayed
	RefList<KeywordWidgetBase,bool> keywordWidgets_;

	public:
	// Set up keyword controls for specified Module
	void setUp(DUQWindow* duqWindow, Module* module);
	// Update controls within widget
	void updateControls();
};

#endif
