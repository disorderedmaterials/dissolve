/*
	*** Add Configuration Wizard Functions
	*** src/gui/addconfigurationwizard_funcs.cpp
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

#include "gui/addconfigurationwizard.h"
#include "gui/helpers/combopopulator.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

// Constructor / Destructor
AddConfigurationWizard::AddConfigurationWizard(QWidget* parent)
{
	dissolveReference_ = NULL;
	importTarget_ = NULL;

	// Set up our UI, and attach the wizard's widgets to our vacant QFrame
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Register pages with the wizard
	registerChoicePage(AddConfigurationWizard::StartPage, "Create Configuration");
	registerPage(AddConfigurationWizard::MonoSpeciesPage, "Single-Species Configuration", AddConfigurationWizard::BoxTypePage);
	registerPage(AddConfigurationWizard::MultiSpeciesPage, "Mixture of Species", AddConfigurationWizard::BoxTypePage);
	registerPage(AddConfigurationWizard::BoxTypePage, "Basic Box Type", AddConfigurationWizard::BoxGeometryPage);
	registerPage(AddConfigurationWizard::BoxGeometryPage, "Box Geometry", AddConfigurationWizard::FinishPage);
	registerFinishPage(AddConfigurationWizard::FinishPage, "Name and Temperature");

	// Connect signals / slots
// 	connect(ui_.AtomTypesList->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(atomTypesListEdited(QWidget*)));
// 	connect(ui_.CreateAtomicElementSelector, SIGNAL(elementSelectionChanged()), this, SLOT(createAtomicElementChanged()));

	lockedForRefresh_ = 0;
}

AddConfigurationWizard::~AddConfigurationWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void AddConfigurationWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Move constructed Configuration over to the specified Dissolve object, returning the new pointer to it
Configuration* AddConfigurationWizard::importConfiguration(Dissolve& dissolve)
{
	if (!importTarget_) return NULL;

	// Set the name of the Configuration from the name edit
	importTarget_->setName(qPrintable(ui_.FinishNameEdit->text()));

	dissolve.ownConfiguration(importTarget_);

	Configuration* newCfg = importTarget_;

	importTarget_ = NULL;

	return newCfg;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddConfigurationWizard::displayControlPage(int index)
{
	// Check page index given
	if ((index < 0) || (index >= AddConfigurationWizard::nPages)) return Messenger::error("Page index %i not recognised.\n", index);

	// Page index is valid, so show it - no need to switch/case
	ui_.MainStack->setCurrentIndex(index);

	// Update controls in the target page if necessary
	switch (index)
	{
		default:
			break;
	}

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool AddConfigurationWizard::progressionAllowed(int index) const
{
	// Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
	switch (index)
	{
		case (AddConfigurationWizard::FinishPage):
			return (ui_.FinishNameIndicator->state());
		default:
			break;
	}

	return true;
}

// Perform any necssary actions before moving to the next page
bool AddConfigurationWizard::prepareForNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddConfigurationWizard::BoxTypePage):
			// Set relevant controls on the Box geometry page
			ui_.BoxRelativeLengthASpin->setVisible(true);
			ui_.BoxRelativeLengthASpin->setEnabled(true);
			ui_.BoxRelativeLengthBSpin->setVisible(true);
			ui_.BoxRelativeLengthCSpin->setVisible(true);
			if (ui_.BoxNonPeriodicRadio->isChecked())
			{
				ui_.BoxAbsoluteAngleAlphaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleBetaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleGammaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleAlphaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleBetaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleGammaSpin->setValue(90.0);
				ui_.BoxRelativeLengthASpin->setVisible(false);
				ui_.BoxRelativeLengthBSpin->setVisible(false);
				ui_.BoxRelativeLengthCSpin->setVisible(false);
			}
			else if (ui_.BoxCubicRadio->isChecked())
			{
				ui_.BoxAbsoluteAngleAlphaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleBetaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleGammaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleAlphaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleBetaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleGammaSpin->setValue(90.0);
				ui_.BoxRelativeLengthASpin->setEnabled(false);
				ui_.BoxRelativeLengthBSpin->setVisible(false);
				ui_.BoxRelativeLengthCSpin->setVisible(false);
			}
			else if (ui_.BoxOrthorhombicRadio->isChecked())
			{
				ui_.BoxAbsoluteAngleAlphaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleBetaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleGammaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleAlphaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleBetaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleGammaSpin->setValue(90.0);
			}
			else if (ui_.BoxMonoclinicRadio->isChecked())
			{
				ui_.BoxAbsoluteAngleAlphaSpin->setEnabled(true);
				ui_.BoxAbsoluteAngleBetaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleGammaSpin->setEnabled(false);
				ui_.BoxAbsoluteAngleBetaSpin->setValue(90.0);
				ui_.BoxAbsoluteAngleGammaSpin->setValue(90.0);
			}
			else if (ui_.BoxTriclinicRadio->isChecked())
			{
				ui_.BoxAbsoluteAngleAlphaSpin->setEnabled(true);
				ui_.BoxAbsoluteAngleBetaSpin->setEnabled(true);
				ui_.BoxAbsoluteAngleGammaSpin->setEnabled(true);
			}
		default:
			break;
	}

	return true;
}

// Perform any necssary actions before moving to the previous page
bool AddConfigurationWizard::prepareForPreviousPage(int currentIndex)
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
void AddConfigurationWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(AddConfigurationWizard::StartPage);

	// Update available Species where appropriate
	ComboNameListPopulator<Species> monoSpeciesComboPopulator(ui_.MonoSpeciesSpeciesCombo, dissolveReference_->coreData().constSpecies());

	// Restrict start page options based on number of Species available
	ui_.StartMonoSpeciesButton->setEnabled(dissolveReference_->nSpecies() != 0);
	ui_.StartMultiSpeciesButton->setEnabled(dissolveReference_->nSpecies() > 1);

	// Create a new Configuration to tinker with
	if (importTarget_) delete importTarget_;
	importTarget_ = new Configuration;

	// Set a new, unique name ready on the final page
	ui_.FinishNameEdit->setText(dissolveReference_->coreData().uniqueConfigurationName("NewConfiguration"));
}

/*
 * Start Page
 */

void AddConfigurationWizard::on_StartCreateEmptyButton_clicked(bool checked)
{
	goToPage(AddConfigurationWizard::FinishPage);
}

void AddConfigurationWizard::on_StartMonoSpeciesButton_clicked(bool checked)
{
	goToPage(AddConfigurationWizard::MonoSpeciesPage);
}

void AddConfigurationWizard::on_StartMultiSpeciesButton_clicked(bool checked)
{
	goToPage(AddConfigurationWizard::MultiSpeciesPage);
}

/*
 * Configuration Name Page
 */

// Configuration name edited
void AddConfigurationWizard::on_FinishNameEdit_textChanged(const QString text)
{
	if (!dissolveReference_)
	{
		printf("Internal Error: Dissolve pointer has not been set in AddConfigurationWizard.\n");
		return;
	}

	// Name of the prospective Configuration has been changed, so make sure it is valid
	bool nameIsValid;
	if (text.isEmpty()) nameIsValid = false;
	else nameIsValid = dissolveReference_->findConfiguration(qPrintable(text)) == NULL;

	ui_.FinishNameIndicator->setState(nameIsValid);

	// Update state of progression controls
	updateProgressionControls();
}
