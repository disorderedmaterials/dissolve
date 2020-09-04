/*
    *** Add Configuration Wizard Functions
    *** src/gui/addconfigurationwizard_funcs.cpp
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

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "gui/addconfigurationwizard.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

AddConfigurationWizard::AddConfigurationWizard(QWidget *parent)
{
    dissolveReference_ = nullptr;
    importTarget_ = nullptr;

    // Set up our UI, and attach the wizard's widgets to placeholder widgets (if available)
    ui_.setupUi(this);
    setUpHeaderAndFooter(this);

    // Register pages with the wizard
    registerChoicePage(AddConfigurationWizard::StartPage, "Create Configuration");
    registerPage(AddConfigurationWizard::SelectTemplatePage, "Select Generator Template",
                 AddConfigurationWizard::NameAndTemperaturePage);
    registerPage(AddConfigurationWizard::NameAndTemperaturePage, "Name and Temperature", WizardWidgetPageInfo::FinishHereFlag);

    // Connect signals / slots
    // 	connect(ui_.AtomTypesList->itemDelegate(), SIGNAL(commitData(QWidget*)), this,
    // SLOT(atomTypesListEdited(QWidget*))); 	connect(ui_.CreateAtomicElementSelector,
    // SIGNAL(elementSelectionChanged()), this, SLOT(createAtomicElementChanged()));

    lockedForRefresh_ = 0;
}

AddConfigurationWizard::~AddConfigurationWizard() {}

/*
 * Data
 */

// Set Dissolve reference
void AddConfigurationWizard::setMainDissolveReference(const Dissolve *dissolveReference)
{
    dissolveReference_ = dissolveReference;
}

// Move constructed Configuration over to the specified Dissolve object, returning the new pointer to it
Configuration *AddConfigurationWizard::importConfiguration(Dissolve &dissolve)
{
    if (!importTarget_)
        return nullptr;

    // Set the name of the Configuration from the name edit
    importTarget_->setName(qPrintable(ui_.FinishNameEdit->text()));

    // Set the Configuration's temperature
    importTarget_->setTemperature(ui_.FinishTemperatureSpin->value());

    dissolve.ownConfiguration(importTarget_);

    Configuration *newCfg = importTarget_;

    importTarget_ = nullptr;

    return newCfg;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddConfigurationWizard::displayControlPage(int index)
{
    // Check page index given
    if ((index < 0) || (index >= AddConfigurationWizard::nPages))
        return Messenger::error("Page index {} not recognised.\n", index);

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
        case (AddConfigurationWizard::NameAndTemperaturePage):
            return (ui_.FinishNameIndicator->state() == CheckIndicator::OKState);
        default:
            break;
    }

    return true;
}

// Perform any necessary actions before moving to the next page
bool AddConfigurationWizard::prepareForNextPage(int currentIndex)
{
    Species *sp;

    switch (currentIndex)
    {
        case (AddConfigurationWizard::SelectTemplatePage):
            // Clear Configuration, and add used Species
            importTarget_->clear();
            break;
        default:
            break;
    }

    return true;
}

// Determine next page for the current page, based on current data
int AddConfigurationWizard::determineNextPage(int currentIndex)
{
    auto result = -1;

    switch (currentIndex)
    {
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

    // Create a new Configuration to tinker with
    if (importTarget_)
        delete importTarget_;
    importTarget_ = new Configuration;

    // Set a new, unique name ready on the final page
    ui_.FinishNameEdit->setText(
        QString::fromStdString(std::string(dissolveReference_->constCoreData().uniqueConfigurationName("NewConfiguration"))));
}

/*
 * Start Page
 */

void AddConfigurationWizard::on_StartCreateEmptyButton_clicked(bool checked)
{
    goToPage(AddConfigurationWizard::NameAndTemperaturePage);
}

void AddConfigurationWizard::on_StartCreateTemplateButton_clicked(bool checked)
{
    goToPage(AddConfigurationWizard::SelectTemplatePage);
}

/*
 * Select Template Page
 */

// TODO

/*
 * Configuration Name Page
 */

// Configuration name edited
void AddConfigurationWizard::on_FinishNameEdit_textChanged(const QString text)
{
    if (!dissolveReference_)
        return;

    // Name of the prospective Configuration has been changed, so make sure it is valid
    bool nameIsValid;
    if (text.isEmpty())
        nameIsValid = false;
    else
        nameIsValid = dissolveReference_->findConfiguration(qPrintable(text)) == nullptr;

    ui_.FinishNameIndicator->setOK(nameIsValid);

    // Update state of progression controls
    updateProgressionControls();
}
