/*
	*** Custom MDI Area
	*** src/gui/tmdiarea_funcs.cpp
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

#include "gui/tmdiarea.hui"
#include "gui/gui.h"
#include "main/duq.h"

// Constructor
TMdiArea::TMdiArea(DUQWindow* duqWindow, QWidget* parent) : QMdiArea(parent)
{
	duqWindow_ = duqWindow;

	setFont(duqWindow_->font());

	// Set up the area
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

/*
 * Signals / Slots
 */

// Custom context menu requested
void TMdiArea::showContextMenu(const QPoint& pos)
{
	QMenu menu;
	menu.setFont(font());
	QMenu* subMenu = menu.addMenu("Add &Widget");
	duqWindow_->createModuleMenu(subMenu);
	menu.exec(mapToGlobal(pos));
}
