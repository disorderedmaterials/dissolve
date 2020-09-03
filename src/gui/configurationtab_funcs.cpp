/*
    *** ConfigurationTab Functions
    *** src/gui/configurationtab_funcs.cpp
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

#include "base/lineparser.h"
#include "base/units.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "gui/configurationtab.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/getconfigurationnamedialog.h"
#include "gui/gui.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QMessageBox>

ConfigurationTab::ConfigurationTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent,
                                   const QString title, Configuration *cfg)
    : ListItem<ConfigurationTab>(), MainTab(dissolveWindow, dissolve, parent, QString("Configuration: %1").arg(title), this)
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    configuration_ = cfg;

    // Populate coordinates file format combo
    for (int n = 0; n < cfg->inputCoordinates().nFormats(); ++n)
        ui_.CoordinatesFileFormatCombo->addItem(QString::fromStdString(std::string(cfg->inputCoordinates().formatKeyword(n))));

    // Populate density units combo
    ComboEnumOptionsPopulator(ui_.DensityUnitsCombo, Units::densityUnits());

    // Set target for ConfigurationViewer
    ui_.ViewerWidget->setConfiguration(configuration_);

    // Set target for ProcedureEditor, and connect signals
    ui_.ProcedureWidget->setUp(&configuration_->generator(), dissolve.coreData());
    connect(ui_.ProcedureWidget, SIGNAL(dataModified()), dissolveWindow, SLOT(setModified()));

    // Set up the ModuleEditor
    ui_.LayerPanel->setUp(dissolveWindow, &cfg->moduleLayer(), configuration_);
}

ConfigurationTab::~ConfigurationTab()
{
    // Remove the Configuration represented in this tab
    dissolve_.removeConfiguration(configuration_);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ConfigurationTab::type() const { return MainTab::ConfigurationTabType; }

// Raise suitable dialog for entering / checking new tab name
QString ConfigurationTab::getNewTitle(bool &ok)
{
    // Get a new, valid name for the Configuration
    GetConfigurationNameDialog nameDialog(this, dissolve_.coreData());
    ok = nameDialog.get(configuration_, QString::fromStdString(std::string(configuration_->name())));

    if (ok)
    {
        // Rename our Configuration, and flag that our data has been modified
        configuration_->setName(qPrintable(nameDialog.newName()));

        dissolveWindow_->setModified();
    }

    return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool ConfigurationTab::canChangeTitle() const { return true; }

// Return whether the tab can be closed (after any necessary user querying, etc.)
bool ConfigurationTab::canClose() const
{
    // Check that we really want to delete this tab
    QMessageBox queryBox;
    queryBox.setText(QString("Really delete the configuration '%1'?\nThis cannot be undone!")
                         .arg(QString::fromStdString(std::string(configuration_->name()))));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret != QMessageBox::Yes)
        return false;

    return true;
}

/*
 * Configuration Target
 */

// Return displayed Configuration
Configuration *ConfigurationTab::configuration() const { return configuration_; }

/*
 * Update
 */

// Update density label
void ConfigurationTab::updateDensityLabel()
{
    if (!configuration_)
    {
        ui_.DensityUnitsLabel->setText("N/A");
        return;
    }

    if (ui_.DensityUnitsCombo->currentIndex() == 0)
        ui_.DensityUnitsLabel->setText(QString::number(configuration_->atomicDensity()));
    else
        ui_.DensityUnitsLabel->setText(QString::number(configuration_->chemicalDensity()));
}
// Update controls in tab
void ConfigurationTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Temperature
    ui_.TemperatureSpin->setValue(configuration_->temperature());

    // Current Box
    const Box *box = configuration_->box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    ui_.CurrentBoxALabel->setText(QString::number(box->axisLengths().x));
    ui_.CurrentBoxBLabel->setText(QString::number(box->axisLengths().y));
    ui_.CurrentBoxCLabel->setText(QString::number(box->axisLengths().z));
    ui_.CurrentBoxAlphaLabel->setText(QString::number(box->axisAngles().x));
    ui_.CurrentBoxBetaLabel->setText(QString::number(box->axisAngles().y));
    ui_.CurrentBoxGammaLabel->setText(QString::number(box->axisAngles().z));
    updateDensityLabel();

    // Input Coordinates
    ui_.CoordinatesFileEdit->setText(QString::fromStdString(std::string(configuration_->inputCoordinates().filename())));
    ui_.CoordinatesFileFormatCombo->setCurrentIndex(configuration_->inputCoordinates().formatIndex());

    // Size Factor
    ui_.RequestedSizeFactorSpin->setValue(configuration_->requestedSizeFactor());
    ui_.AppliedSizeFactorSpin->setValue(configuration_->appliedSizeFactor());

    // Generator
    ui_.ProcedureWidget->updateControls();

    // Viewer
    ui_.ViewerWidget->postRedisplay();
}

// Disable sensitive controls within tab
void ConfigurationTab::disableSensitiveControls()
{
    ui_.GeneratorGroup->setEnabled(false);
    ui_.TemperatureGroup->setEnabled(false);
    ui_.SizeFactorGroup->setEnabled(false);
}

// Enable sensitive controls within tab
void ConfigurationTab::enableSensitiveControls()
{
    ui_.GeneratorGroup->setEnabled(true);
    ui_.TemperatureGroup->setEnabled(true);
    ui_.SizeFactorGroup->setEnabled(true);
}

/*
 * Signals / Slots
 */

void ConfigurationTab::on_GeneratorRegenerateButton_clicked(bool checked)
{
    // Are we sure that's what we want to do?
    QMessageBox queryBox;
    queryBox.setText(QString("This will erase the current contents of the Configuration '%1'.")
                         .arg(QString::fromStdString(std::string(configuration_->name()))));
    queryBox.setInformativeText("Proceed?");
    queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    queryBox.setDefaultButton(QMessageBox::No);
    auto ret = queryBox.exec();

    if (ret == QMessageBox::Yes)
    {
        configuration_->initialiseContent(dissolve_.worldPool(), dissolve_.pairPotentialRange(), true);
        updateControls();
    }
}

void ConfigurationTab::on_TemperatureSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    configuration_->setTemperature(value);

    dissolveWindow_->setModified();
}

// Current Box
void ConfigurationTab::on_DensityUnitsCombo_currentIndexChanged(int index) { updateDensityLabel(); }

// Initial Coordinates
void ConfigurationTab::on_CoordinatesFileEdit_textChanged(QString text)
{
    if (refreshLock_.isLocked())
        return;
}

void ConfigurationTab::on_CoordinatesFileSelectButton_clicked(bool checked)
{
    if (refreshLock_.isLocked())
        return;
}

// Size Factor Scaling
void ConfigurationTab::on_RequestedSizeFactorSpin_valueChanged(double value)
{
    if (refreshLock_.isLocked())
        return;

    configuration_->setRequestedSizeFactor(value);
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ConfigurationTab::readState(LineParser &parser, const CoreData &coreData) { return true; }

// Write widget state through specified LineParser
bool ConfigurationTab::writeState(LineParser &parser) const { return true; }
