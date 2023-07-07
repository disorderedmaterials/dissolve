// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/units.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "gui/configurationTab.h"
#include "gui/getConfigurationNameDialog.h"
#include "gui/gui.h"
#include "gui/helpers/comboPopulator.h"
#include "gui/keywordWidgets/producers.h"
#include "main/dissolve.h"
#include <QMessageBox>

ConfigurationTab::ConfigurationTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent,
                                   const QString title, Configuration *cfg)
    : MainTab(dissolveWindow, dissolve, parent, QString("Configuration: %1").arg(title), this),
      procedureModel_(cfg->generator())
{
    ui_.setupUi(this);

    Locker refreshLocker(refreshLock_);

    configuration_ = cfg;

    // Populate density units combo
    ComboEnumOptionsPopulator(ui_.DensityUnitsCombo, Units::densityUnits());

    // Set target for ConfigurationViewer
    ui_.ViewerWidget->setConfiguration(configuration_);

    // Set-up the generator procedure editor
    ui_.GeneratorWidget->setUp(dissolveWindow_, configuration_->generator());
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ConfigurationTab::type() const { return MainTab::TabType::Configuration; }

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
        ui_.DensityUnitsLabel->setText("N/A");
    else
    {
        auto rho =
            ui_.DensityUnitsCombo->currentIndex() == 0 ? configuration_->atomicDensity() : configuration_->chemicalDensity();
        ui_.DensityUnitsLabel->setText(rho ? QString::number(*rho) : "--");
    }
}

// Update controls in tab
void ConfigurationTab::updateControls()
{
    Locker refreshLocker(refreshLock_);

    // Temperature
    ui_.TemperatureLabel->setText(QString::number(configuration_->temperature()).append(QString(" K")));

    // Current Box
    const auto *box = configuration_->box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    QString boxInfo =
        QString("A  %1      %2  %3\n").arg(box->axisLengths().x).arg(QString::fromUtf8("\u03B1")).arg(box->axisAngles().x);
    boxInfo +=
        QString("B  %1      %2  %3\n").arg(box->axisLengths().y).arg(QString::fromUtf8("\u03B2")).arg(box->axisAngles().y);
    boxInfo +=
        QString("C  %1      %2  %3\n").arg(box->axisLengths().z).arg(QString::fromUtf8("\u03B3")).arg(box->axisAngles().z);
    ui_.CurrentBoxTypeLabel->setToolTip(boxInfo);
    updateDensityLabel();

    // Populations
    ui_.AtomPopulationLabel->setText(QString::number(configuration_->nAtoms()));
    ui_.MoleculePopulationLabel->setText(QString::number(configuration_->nMolecules()));

    // Viewer
    ui_.ViewerWidget->postRedisplay();
}

// Prevent editing of the generator
void ConfigurationTab::preventEditing() { ui_.GeneratorWidget->setEnabled(false); }

// Allow editing of the generator
void ConfigurationTab::allowEditing() { ui_.GeneratorWidget->setEnabled(true); }

/*
 * Signals / Slots
 */

// Generate
void ConfigurationTab::on_GenerateButton_clicked(bool checked)
{
    auto proceed = true;

    // If the configuration already contains something, check first
    if (configuration_->nAtoms() > 0)
    {
        QMessageBox queryBox;
        queryBox.setText(QString("This will erase the current contents of the Configuration '%1'.")
                             .arg(QString::fromStdString(std::string(configuration_->name()))));
        queryBox.setInformativeText("Proceed?");
        queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        queryBox.setDefaultButton(QMessageBox::No);
        proceed = queryBox.exec() == QMessageBox::Yes;
    }

    if (!proceed)
        return;

    // Clear the messages
    dissolveWindow_->clearMessages();

    // Make sure the potential map is up to date
    dissolve_.regeneratePairPotentials();

    // Initialise the content
    configuration_->initialiseContent({dissolve_.worldPool(), dissolve_.potentialMap()});

    // Update
    updateControls();
    dissolveWindow_->updateStatusBar();
}

// Density units changed
void ConfigurationTab::on_DensityUnitsCombo_currentIndexChanged(int index) { updateDensityLabel(); }
