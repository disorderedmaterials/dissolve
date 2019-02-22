/*
	*** Species Viewer - Context Menu
	*** src/gui/viewer/speciesviewer_context.cpp
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

#include "gui/viewer/speciesviewer.hui"
#include "templates/variantpointer.h"
#include <QClipboard>

/// Initialise context menu
void SpeciesViewer::initialiseContextMenu()
{
// 	// Set font for menu, and create italic version
// 	QFont menuFont = font();
// 	menuFont.setPointSize(8);
// 	contextMenu_.setFont(menuFont);
// 	QFont italicFont = font();
// 	italicFont.setItalic(true);
// 
// 	// View-wide functions
// 	contextMenu_.addSeparator();
// 	QAction* action = contextMenu_.addAction("Copy image");
// 	connect(action, SIGNAL(triggered(bool)), this, SLOT(copyViewToClipboard(bool)));
}

// Update dynamic aspects of context menu before display
void SpeciesViewer::updateContextMenu()
{
}

