/*
	*** Add Processing Layer Wizard Functions
	*** src/gui/addprocessinglayerwizard_funcs.cpp
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

#include "gui/addprocessinglayerwizard.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

// Constructor / Destructor
AddProcessingLayerWizard::AddProcessingLayerWizard(QWidget* parent) : temporaryDissolve_(temporaryCoreData_)
{
	dissolveReference_ = NULL;
	importTarget_ = NULL;

	// Set up our UI, and attach the wizard's widgets to placeholder widgets (if available)
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Register pages with the wizard
	registerChoicePage(AddProcessingLayerWizard::StartPage, "Create Processing Layer");
	registerFinishPage(AddProcessingLayerWizard::FinishPage, "Layer Name");

	// Connect signals / slots
	/* none */

	lockedForRefresh_ = 0;
}

AddProcessingLayerWizard::~AddProcessingLayerWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void AddProcessingLayerWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Move created ModuleLayer to the specified Dissolve object, returning the new pointer to it
ModuleLayer* AddProcessingLayerWizard::importModuleLayer(Dissolve& dissolve)
{
	// If there is no import target set, return now
	if (!importTarget_) return NULL;

	/*
	 * We will surgically detach the Modules in the importTarget_ from the temporary Dissolve object,
	 * adding them to the main Dissolve reference (they should already have been renamed to avoid clashes).
	 * Similarly, Configuration targets will have been remapped / removed by the time we get here.
	 */
	// TODO

	// Now, re-map any Configuration targets as specified, 

	// Set the name of the layer from the name edit
	importTarget_->setName(qPrintable(ui_.FinishNameEdit->text()));

	// Finally, remove the importTarget_ from its list, and re-own it elsewhere
	temporaryDissolve_.processingLayers().disown(importTarget_);
	dissolve.ownProcessingLayer(importTarget_);

	ModuleLayer* newLayer = importTarget_;

	importTarget_ = NULL;

	return newLayer;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddProcessingLayerWizard::displayControlPage(int index)
{
	// Check page index given
	if ((index < 0) || (index >= AddProcessingLayerWizard::nPages)) return Messenger::error("Page index %i not recognised.\n", index);

	// Page index is valid, so show it - no need to switch/case
	ui_.MainStack->setCurrentIndex(index);

	// Update controls in the target page if necessary
	switch (index)
	{
		case (AddProcessingLayerWizard::StartPage):
			// Clear our temporary Dissolve instance
			temporaryDissolve_.clear();
			break;
		default:
			break;
	}

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool AddProcessingLayerWizard::progressionAllowed(int index) const
{
	// Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
	switch (index)
	{
		case (AddProcessingLayerWizard::FinishPage):
			return (ui_.FinishNameIndicator->state());
		default:
			break;
	}

	return true;
}

// Perform any necessary actions before moving to the next page
bool AddProcessingLayerWizard::prepareForNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		default:
			break;
	}

	return true;
}

// Determine next page for the current page, based on current data
int AddProcessingLayerWizard::determineNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		default:
			break;
	}

	return -1;
}

// Perform any necessary actions before moving to the previous page
bool AddProcessingLayerWizard::prepareForPreviousPage(int currentIndex)
{
	switch (currentIndex)
	{
		default:
			break;
	}

	return true;
}

/*
 * Controls
 */

// Reset widget, ready for adding a new Configuration
void AddProcessingLayerWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(AddProcessingLayerWizard::StartPage);

	// Set a new, unique name ready on the final page
	ui_.FinishNameEdit->setText(dissolveReference_->uniqueProcessingLayerName("Main Processing"));
}

/*
 * Start Page
 */

void AddProcessingLayerWizard::on_StartCreateEmptyButton_clicked(bool checked)
{
	// Create an empty processing layer in our temporary data
	importTarget_ = temporaryDissolve_.addProcessingLayer();

	goToPage(AddProcessingLayerWizard::FinishPage);
}

void AddProcessingLayerWizard::on_StartAddPredefinedButton_clicked(bool checked)
{
// 	goToPage(AddProcessingLayerWizard::Page);
}

void AddProcessingLayerWizard::on_StartImportButton_clicked(bool checked)
{
// 	goToPage(AddProcessingLayerWizard::Page);
}

/*
 * Final Page
 */

// Configuration name edited
void AddProcessingLayerWizard::on_FinishNameEdit_textChanged(const QString text)
{
	if (!dissolveReference_)
	{
		printf("Internal Error: Dissolve pointer has not been set in AddProcessingLayerWizard.\n");
		return;
	}

	// Name of the prospective Configuration has been changed, so make sure it is valid
	bool nameIsValid;
	if (text.isEmpty()) nameIsValid = false;
	else nameIsValid = dissolveReference_->findProcessingLayer(qPrintable(text)) == NULL;

	ui_.FinishNameIndicator->setState(nameIsValid);

	// Update state of progression controls
	updateProgressionControls();
}
