/*
	*** Module Palette
	*** src/gui/modulepalette_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/modulepalette.hui"
#include "gui/paletteblock.h"
#include "gui/gui.h"
#include "module/list.h"
#include "module/module.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

// Constructor
ModulePalette::ModulePalette(DissolveWindow* dissolveWindow, QWidget* parent) : QWidget(parent)
{
	dissolveWindow_ = dissolveWindow;

	// Drag / Drop
	draggedModule_ = NULL;

	// Create a layout for our widget
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);

	// Add available Modules to our widget
	for (ModuleReference* modRef = ModuleList::masterInstances().first(); modRef != NULL; modRef = modRef->next)
	{
		PaletteBlock* widget = new PaletteBlock(this, dissolveWindow_, modRef);
		displayedWidgets_.add(widget);

		layout->addWidget(widget);
	}
}

ModulePalette::~ModulePalette()
{
}

/*
 * QWidget Reimplementations
 */

// Mouse press event
void ModulePalette::mousePressEvent(QMouseEvent* event)
{
	// If the left-button was pressed *on a PaletteBlock HeaderFrame*, store the current click position. Otherwise do nothing
	if (event->button() == Qt::LeftButton)
	{
		// Check object under mouse
		draggedModule_ = paletteBlockHeaderAt(mapToGlobal(event->pos()));
// 		if (draggedModule_) printf("BLOCK = %s\n", draggedModule_->moduleReference()->module()->uniqueName());
		dragStartPosition_ = event->pos();
	}
}

// Mouse move event
void ModulePalette::mouseMoveEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// If we have no draggedModule_, then nothing to do
	if (!draggedModule_) return;

	// Check to see if we should begin a drag event based on the length of the click-drag so far
	if ((event->pos() - dragStartPosition_).manhattanLength() < QApplication::startDragDistance()) return;

	// Generate mime data for the event
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << draggedModule_;
	QMimeData* mimeData = new QMimeData;
	mimeData->setData("image/x-dissolve-paletteblock", itemData);

	// Construct the drag object
	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
// 	drag->setHotSpot(dragStartPosition_ -
	drag->setPixmap(draggedModule_->grab());

	// Begin the drag event
	//Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
// 	if (dropAction 
// 	...
}

// Mouse release event
void ModulePalette::mouseReleaseEvent(QMouseEvent* event)
{

}

// Mouse double-click event
void ModulePalette::mouseDoubleClickEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Was a PaletteBlock's header was under the mouse?
	PaletteBlock* block = paletteBlockHeaderAt(mapToGlobal(event->pos()));
	if (!block) return;

	// Attempt to open the Module in a ModuleTab
	Module* module = block->moduleReference()->module();
	if (!module) return;
	dissolveWindow_->addModuleTab(module);
}

// Drag enter event
void ModulePalette::dragEnterEvent(QDragEnterEvent* event)
{
	event->ignore();
}

// Drag leave event
void ModulePalette::dragLeaveEvent(QDragLeaveEvent* event)
{
	// Object has been dragged outside the widget
	update();
	event->accept();
}

// Draw move event
void ModulePalette::dragMoveEvent(QDragMoveEvent* event)
{
	repaint();
}

// Drop event
void ModulePalette::dropEvent(QDropEvent* event)
{
	event->ignore();
}

/*
 * Display Widgets
 */

// Find PaletteBlock displaying specified ModuleReference
RefListItem<PaletteBlock,bool>* ModulePalette::paletteBlockReference(ModuleReference* modRef)
{
	RefListIterator<PaletteBlock,bool> flowBlockIterator(displayedWidgets_);
	while (PaletteBlock* block = flowBlockIterator.iterate()) if (block->moduleReference() == modRef) return flowBlockIterator.currentItem();

	return NULL;
}

// Return the PaletteBlock clicked on its header at the specified position (if any)
PaletteBlock* ModulePalette::paletteBlockHeaderAt(QPoint globalPos)
{
	RefListIterator<PaletteBlock,bool> flowBlockIterator(displayedWidgets_);
	while (PaletteBlock* block = flowBlockIterator.iterate()) if (block->ui.HeaderFrame->geometry().contains(block->mapFromGlobal(globalPos))) return block;

	return NULL;
}

// Update controls within widget
void ModulePalette::updateControls()
{
}

// Disable sensitive controls within widget, ready for main code to run
void ModulePalette::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void ModulePalette::enableSensitiveControls()
{
}

