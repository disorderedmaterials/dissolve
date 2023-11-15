// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/gui.h"
#include "main/dissolve.h"
#include <QMessageBox>

// Clear all module data
bool DissolveWindow::clearModuleData(bool queryUser)
{
    QMessageBox::StandardButton buttonResult = QMessageBox::Yes;
    if (queryUser)
        buttonResult = QMessageBox::warning(
            this, "Clear Module Data",
            "This will delete all data generated by modules, and reset the iteration counter to zero.\n"
            "Configuration contents will remain as-is.\n\nThis cannot be undone! Proceed?",
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);

    if (buttonResult == QMessageBox::StandardButton::Yes)
    {
        // Invalidate all renderables before we clear the data
        Renderable::invalidateAll();
        Renderable::setSourceDataAccessEnabled(false);

        // Clear main processing data
        dissolve_.processingModuleData().clear();

        // Set iteration counter to zero
        dissolve_.resetIterationCounter();

        // Regenerate pair potentials
        dissolve_.regeneratePairPotentials();

        Renderable::setSourceDataAccessEnabled(true);

        fullUpdate();
    }

    return buttonResult == QMessageBox::StandardButton::Yes;
}

void DissolveWindow::setupIteration(int count)
{
    // Ensure that the simulation can run
    if (dissolve_.inputFilename().empty())
    {
        QMessageBox::warning(this, "No Input File", "You must save the file before running the simulation");
        return;
    }

    // Clear the messages tab
    clearMessages();

    // Prepare the simulation
    if (!dissolve_.prepare())
    {
        updateStatusBar();
        return;
    }

    // Prepare the GUI
    preventEditing();
    Renderable::setSourceDataAccessEnabled(false);
    dissolveIterating_ = true;

    // Update the controls
    fullUpdate();

    Q_EMIT iterate(count);

    // Start the main timer
    elapsedTimer_.zero();
    if (count == -1)
    {
        elapsedTimer_.start();
    }
    else
    {
        elapsedTimer_.zero();
    }
}

// Disable editing
void DissolveWindow::preventEditing()
{
    // Disable necessary menu items
    ui_.FileLoadRestartPointAction->setEnabled(false);
    ui_.FileSaveRestartPointAction->setEnabled(false);
    ui_.SimulationRunAction->setEnabled(false);
    ui_.SimulationStopAction->setEnabled(true);
    ui_.SimulationStepAction->setEnabled(false);
    ui_.SimulationStepFiveAction->setEnabled(false);
    ui_.SimulationDataManagerAction->setEnabled(false);

    // Disable necessary menus
    ui_.SpeciesMenu->setEnabled(false);
    ui_.ConfigurationMenu->setEnabled(false);
    ui_.LayerMenu->setEnabled(false);

    // Disable editing in all tabs
    ui_.MainTabs->preventEditing();
}

// Allow editing
void DissolveWindow::allowEditing()
{
    // Enable necessary menu items
    ui_.FileLoadRestartPointAction->setEnabled(true);
    ui_.FileSaveRestartPointAction->setEnabled(true);
    ui_.SimulationRunAction->setEnabled(true);
    ui_.SimulationStopAction->setEnabled(false);
    ui_.SimulationStepAction->setEnabled(true);
    ui_.SimulationStepFiveAction->setEnabled(true);
    ui_.SimulationDataManagerAction->setEnabled(true);

    // Enable necessary menus
    ui_.SpeciesMenu->setEnabled(true);
    ui_.ConfigurationMenu->setEnabled(true);
    ui_.LayerMenu->setEnabled(true);

    // Allow editing in all tabs
    ui_.MainTabs->allowEditing();
}

// All iterations requested are complete
void DissolveWindow::iterationsComplete()
{
    elapsedTimer_.stop();

    allowEditing();
    Renderable::setSourceDataAccessEnabled(true);
    dissolveIterating_ = false;

    fullUpdate();
}

/*
 * Tabs
 */

void DissolveWindow::on_MainTabs_currentChanged(int index)
{
    if (refreshing_)
        return;

    // Retrieve the widget corresponding to the index provided - it will be a MainTab widget, from which all our tab widgets
    // are derived
    MainTab *currentTab = dynamic_cast<MainTab *>(ui_.MainTabs->widget(index));
    if (!currentTab)
    {
        Messenger::print("Can't cast this tab widget (index {}) into a MainTab.\n", index);
        return;
    }

    // Update contents
    currentTab->updateControls();

    // Update menu items
    updateMenus();
}

// Return whether the GUI is currently running a simulation
bool DissolveWindow::dissolveIterating() const { return dissolveIterating_; }

// Return list of all current tabs
const std::vector<MainTab *> DissolveWindow::allTabs() const { return ui_.MainTabs->allTabs(); }

void DissolveWindow::closeTab(QWidget *page)
{
    auto *tab = ui_.MainTabs->findTab(page);
    if (!tab)
        return;

    // Determine what we need to delete...
    if (tab->type() == MainTab::TabType::Configuration)
    {
        auto *cfg = dynamic_cast<ConfigurationTab *>(tab)->configuration();
        ui_.MainTabs->removeByPage(page);
        dissolve_.coreData().removeConfiguration(cfg);
        setModified({DissolveSignals::ConfigurationsMutated});
    }
    else if (tab->type() == MainTab::TabType::Layer)
    {
        auto *layerTab = dynamic_cast<LayerTab *>(tab);
        layerTab->removeModuleControlWidgets();
        ui_.MainTabs->removeByPage(page);
        dissolve_.coreData().removeProcessingLayer(layerTab->moduleLayer());
        setModified({DissolveSignals::ModulesMutated});
    }
    else if (tab->type() == MainTab::TabType::Species)
    {
        auto *sp = dynamic_cast<SpeciesTab *>(tab)->species();
        ui_.MainTabs->removeByPage(page);
        dissolve_.coreData().removeSpecies(sp);
        setModified({DissolveSignals::DataMutations::SpeciesMutated, DissolveSignals::DataMutations::IsotopologuesMutated});
    }
    else
        return;

    fullUpdate();
}
