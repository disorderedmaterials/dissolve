/*
	*** UChroma Base - Functions
	*** src/gui/uchroma/uchromabase_funcs.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/uchromabase.h"

// Static Members
CharString UChromaBase::viewerFontFileName_;

// Constructor
UChromaBase::UChromaBase(QWidget* parent) : parent_(parent), viewLayout_(*this),
	axesWindow_(*this, parent),
	dataWindow_(*this, parent),
	styleWindow_(*this, parent),
	transformWindow_(*this, parent),
	viewWindow_(*this, parent ),
	createCollectionDialog_(*this, parent),
	importDialog_(*this, parent),
	editFitKernelDialog_(*this, parent),
	saveImageDialog_(*this, parent)
{
	// Window
	refreshing_ = false;

	// Interaction
	viewer_ = NULL;
	interactionMode_ = UChromaBase::ViewInteraction;
	interactionAxis_ = -1;
	interactionStarted_ = false;
	clickedInteractionValue_ = 0.0;
	clickedObject_ = Viewer::NoObject;
	currentInteractionValue_ = 0.0;
// 	setInteractionMode(interactionMode_, -1);

	// Modification flag
	modified_ = false;

	// Image export and viewer font
	imageExportFileName_ = "image.png";
	imageExportWidth_ = 800;
	imageExportHeight_ = 600;
	imageExportMaintainAspect_ = true;
	imageExportFormat_ = UChromaBase::PNGFormat;

	// Collections
	currentCollection_ = NULL;

	// View layout
	currentViewPane_ = NULL;

	// Current EditStateGroup (for undo)
	currentEditStateGroup_ = NULL;
	undoEditStateGroup_ = NULL;
	currentEditState_ = NULL;
}

// Virtual Destructor
UChromaBase::~UChromaBase()
{
}

// Return pointer to parent (derived) widget
QWidget* UChromaBase::parent()
{
	return parent_;
}

/*
 * Window Functions
 */

// Return Viewer present in derived class
Viewer* UChromaBase::viewer()
{
	return viewer_;
}

/*
 * Input File
 */

// Current session file directory
void UChromaBase::setSessionFileDirectory(QDir dir)
{
	sessionFileDirectory_ = dir;
}

// Return current session file directory
QDir UChromaBase::sessionFileDirectory()
{
	return sessionFileDirectory_;
}

// Set name of input file
void UChromaBase::setInputFile(const char* fileName)
{
	inputFile_ = fileName;

	updateTitleBar();
}

// Return name of input file
const char* UChromaBase::inputFile()
{
	return inputFile_;
}

/*
 * Modification Flag
 */

// Set as modified
void UChromaBase::setAsModified()
{
	modified_ = true;

	updateTitleBar();
}

// Set as not modified
void UChromaBase::setAsNotModified()
{
	modified_ = false;

	updateTitleBar();
}

// Return status of flag
bool UChromaBase::isModified()
{
	return modified_;
}
