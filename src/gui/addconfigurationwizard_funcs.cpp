/*
	*** Add Configuration Wizard Functions
	*** src/gui/addconfigurationwizard_funcs.cpp
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

#include "gui/addconfigurationwizard.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/box.h"
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

	// Set up our UI, and attach the wizard's widgets to placeholder widgets (if available)
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Register pages with the wizard
	registerChoicePage(AddConfigurationWizard::StartPage, "Create Configuration");
	registerPage(AddConfigurationWizard::MonoSpeciesPage, "Single-Species Configuration", AddConfigurationWizard::BoxTypePage);
	registerPage(AddConfigurationWizard::MultiSpeciesPage, "Mixture of Species", AddConfigurationWizard::BoxTypePage);
	registerPage(AddConfigurationWizard::BoxTypePage, "Basic Box Type");
	registerPage(AddConfigurationWizard::BoxGeometryPage, "Box Geometry", AddConfigurationWizard::MultiplierPage);
	registerPage(AddConfigurationWizard::MultiplierPage, "Box Multiplier", AddConfigurationWizard::FinishPage);
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

// Get Box details from controls and put into target Configuration
void AddConfigurationWizard::getBoxDetails()
{
	// Set Configuration multiplier
	importTarget_->setMultiplier(ui_.MultiplierSpin->value());

	// Set the Box angles and relative lengths in the Configuration
	importTarget_->setBoxAngles(Vec3<double>(ui_.BoxAbsoluteAngleAlphaSpin->value(), ui_.BoxAbsoluteAngleBetaSpin->value(), ui_.BoxAbsoluteAngleGammaSpin->value()));
	if (ui_.BoxNonPeriodicRadio->isChecked())
	{
		importTarget_->setRelativeBoxLengths(Vec3<double>(1.0,1.0,1.0));
		importTarget_->setNonPeriodic(true);
	}
	else if (ui_.BoxCubicRadio->isChecked()) importTarget_->setRelativeBoxLengths(Vec3<double>(1.0,1.0,1.0));
	else if (ui_.BoxOrthorhombicRadio->isChecked()) importTarget_->setRelativeBoxLengths(Vec3<double>(ui_.BoxRelativeLengthASpin->value(), ui_.BoxRelativeLengthBSpin->value(), ui_.BoxRelativeLengthCSpin->value()));
	else importTarget_->setRelativeBoxLengths(Vec3<double>(ui_.BoxRelativeLengthASpin->value(), ui_.BoxRelativeLengthBSpin->value(), ui_.BoxRelativeLengthCSpin->value()));
}

// Move constructed Configuration over to the specified Dissolve object, returning the new pointer to it
Configuration* AddConfigurationWizard::importConfiguration(Dissolve& dissolve)
{
	if (!importTarget_) return NULL;

	// Set the name of the Configuration from the name edit
	importTarget_->setName(qPrintable(ui_.FinishNameEdit->text()));

	// Set the Configuration's temperature
	importTarget_->setTemperature(ui_.FinishTemperatureSpin->value());

	getBoxDetails();

	importTarget_->initialise(dissolve.worldPool(), true, dissolve.pairPotentialRange(), dissolve.nBoxNormalisationPoints());

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

	// Update controls / check flow
	switch (index)
	{
		default:
			break;
	}

	// Page index is valid, so show it - no need to switch/case
	ui_.MainStack->setCurrentIndex(index);

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool AddConfigurationWizard::progressionAllowed(int index) const
{
	// Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
	switch (index)
	{
		case (AddConfigurationWizard::FinishPage):
			return (ui_.FinishNameIndicator->state() == CheckIndicator::OKState);
		default:
			break;
	}

	return true;
}

// Perform any necessary actions before moving to the next page
bool AddConfigurationWizard::prepareForNextPage(int currentIndex)
{
	Species* sp;

	switch (currentIndex)
	{
		case (AddConfigurationWizard::MonoSpeciesPage):
			// Clear Configuration, and add used Species
			importTarget_->clear();
			sp = VariantPointer<Species>(ui_.MonoSpeciesSpeciesCombo->currentData());
			importTarget_->addUsedSpecies(sp, 1.0);
			if (ui_.MonoSpeciesDensityUnitsCombo->currentIndex() == 0) importTarget_->setAtomicDensity(ui_.MonoSpeciesDensitySpin->value());
			else importTarget_->setChemicalDensity(ui_.MonoSpeciesDensitySpin->value());
			break;
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

			// Set box angles and lengths
			getBoxDetails();

			updateMultiplierPage();
			break;
		case (AddConfigurationWizard::BoxGeometryPage):
			// Set box angles and lengths
			getBoxDetails();

			updateMultiplierPage();
			break;
		default:
			break;
	}

	return true;
}

// Determine next page for the current page, based on current data
int AddConfigurationWizard::determineNextPage(int currentIndex)
{
	int result = -1;

	switch (currentIndex)
	{
		case (AddConfigurationWizard::BoxTypePage):
			if (ui_.BoxCubicRadio->isChecked() || ui_.BoxNonPeriodicRadio->isChecked()) result = AddConfigurationWizard::MultiplierPage;
			else result = AddConfigurationWizard::BoxGeometryPage;
			break;
		default:
			break;
	}

	return result;
}

// Perform any necessary actions before moving to the previous page
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
 * Multiplier Page
 */

// Species population row update function
void AddConfigurationWizard::updatePopulationTableRow(int row, Species* sp, int population, bool createItems)
{
	QTableWidgetItem* item;

	// Species Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setFlags(Qt::NoItemFlags);
		ui_.MultiplierPopulationsTable->setItem(row, 0, item);
	}
	else item = ui_.MultiplierPopulationsTable->item(row, 0);
	item->setText(sp->name());

	// Population
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setFlags(Qt::NoItemFlags);
		ui_.MultiplierPopulationsTable->setItem(row, 1, item);
	}
	else item = ui_.MultiplierPopulationsTable->item(row, 1);
	if (population == 0)
	{
		item->setIcon(QIcon(":/general/icons/general_warn.svg"));
		item->setText("Zero");
	}
	else item->setText(QString::number(population));
}

// Update controls on MultiplierPage
void AddConfigurationWizard::updateMultiplierPage()
{
	// Calculate expected number of atoms, and individual species populations
	RefList<Species,int> populations;
	int nExpectedAtoms = 0;
	ListIterator<SpeciesInfo> usedSpeciesIterator(importTarget_->usedSpecies());
	while (SpeciesInfo* spInfo = usedSpeciesIterator.iterate())
	{
		// Get Species pointer
		Species* sp = spInfo->species();

		// Determine the number of molecules of this component
		int count =  spInfo->population() * importTarget_->multiplier();

		populations.add(sp, count);

		nExpectedAtoms += count * sp->nAtoms();
	}

	// Determine required volume, and generate a temporary Box to get hte relevant info
	double volume = nExpectedAtoms / importTarget_->atomicDensity();
	Box* temporaryBox = Box::generate(importTarget_->relativeBoxLengths(), importTarget_->boxAngles(), volume);

	TableWidgetRefListUpdater<AddConfigurationWizard,Species,int> populationsUpdater(ui_.MultiplierPopulationsTable, populations, this, &AddConfigurationWizard::updatePopulationTableRow);
	ui_.MultiplierNAtomsLabel->setText(QString::number(nExpectedAtoms));
	ui_.MultiplierBoxALabel->setText(QString::number(temporaryBox->axisLength(0)));
	ui_.MultiplierBoxBLabel->setText(QString::number(temporaryBox->axisLength(1)));
	ui_.MultiplierBoxCLabel->setText(QString::number(temporaryBox->axisLength(2)));
	ui_.MultiplierPPRangeLabel->setText(QString::number(temporaryBox->inscribedSphereRadius()));
	QPalette palette = ui_.MultiplierBoxCLabel->palette();
	if (temporaryBox->inscribedSphereRadius() < dissolveReference_->pairPotentialRange()) palette.setColor(QPalette::WindowText, Qt::red);
	ui_.MultiplierPPRangeLabel->setPalette(palette);
}

void AddConfigurationWizard::on_MultiplierSpin_valueChanged(int value)
{
	getBoxDetails();

	updateMultiplierPage();
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

	ui_.FinishNameIndicator->setOK(nameIsValid);

	// Update state of progression controls
	updateProgressionControls();
}
