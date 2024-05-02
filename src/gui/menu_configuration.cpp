// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/addConfigurationDialog.h"
#include "gui/configurationTab.h"
#include "gui/gui.h"
#include "gui/selectSpeciesDialog.h"
#include "io/export/coordinates.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"
#include "procedure/nodes/generalRegion.h"
#include "procedure/nodes/parameters.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

/*
 * Helper Functions
 */

std::vector<std::shared_ptr<AddProcedureNode>> createRelativeMix(const std::vector<const Species *> &mixSpecies,
                                                                 Procedure &generator,
                                                                 std::shared_ptr<ParametersProcedureNode> &paramsNode)
{
    std::vector<std::shared_ptr<AddProcedureNode>> addNodes;

    auto count = 0;
    for (auto *sp : mixSpecies)
    {
        // Set the population equation, creating a new ratio parameter if we need one
        std::string popString;
        if (count == 0)
            popString = "populationA";
        else
        {
            auto parameterName = fmt::format("ratio{}", char(65 + count));
            paramsNode->addParameter(parameterName, 1);
            popString = fmt::format("{}*populationA", parameterName);
        }

        // Set up coordinate set, but only if we have a suitable species
        if (sp->nAtoms() > 1)
        {
            auto coordSets = generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("{}_Sets", sp->name()), sp);

            // Create the Add node
            addNodes.emplace_back(generator.createRootNode<AddProcedureNode>(sp->name(), coordSets,
                                                                             NodeValue(popString, paramsNode->parameters()),
                                                                             NodeValue("rho", paramsNode->parameters())));
        }
        else
            addNodes.emplace_back(generator.createRootNode<AddProcedureNode>(
                sp->name(), sp, NodeValue(popString, paramsNode->parameters()), NodeValue("rho", paramsNode->parameters())));

        ++count;
    }

    return addNodes;
}

/*
 * Menu Functions
 */

void DissolveWindow::on_ConfigurationCreateAction_triggered(bool checked)
{
    AddConfigurationDialog addConfigurationDialog(this, dissolve_);

    if (addConfigurationDialog.exec() == QDialog::Accepted)
    {

        auto newConfig = dissolve_.coreData().configurations().back().get();

        // Make sure the potential map is up to date
        dissolve_.regeneratePairPotentials();

        // Initialise the content
        newConfig->initialiseContent({dissolve_.worldPool(), dissolve_});

        // Fully update GUI
        setModified();
        fullUpdate();

        ui_.MainTabs->setCurrentTab(newConfig);
    }
}

void DissolveWindow::on_ConfigurationRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab, then call its rename() function
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Configuration))
        return;
    tab->rename();
}

void DissolveWindow::on_ConfigurationDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Configuration))
        return;

    // Cast up the tab to a ConfigurationTab so we can get the ModuleLayer pointer
    auto cfgTab = dynamic_cast<ConfigurationTab *>(tab);
    if (!cfgTab)
        return;

    // Check that we really want to delete the Configuration
    if (!cfgTab->canClose())
        return;

    ui_.MainTabs->removeByPage(cfgTab->page());
    dissolve_.coreData().removeConfiguration(cfgTab->configuration());

    setModified();
    fullUpdate();
}

void DissolveWindow::on_ConfigurationExportToXYZAction_triggered(bool checked)
{
    // Get the currently-displayed Configuration
    auto *cfg = ui_.MainTabs->currentConfiguration();
    if (!cfg)
        return;

    // Get a suitable export file name
    QString exportFile =
        QFileDialog::getSaveFileName(this, "Choose XYZ export file name", QDir().absolutePath(), "XYZ Coordinates (*.xyz)");
    if (exportFile.isEmpty())
        return;

    CoordinateExportFileFormat fileAndFormat(qPrintable(exportFile), CoordinateExportFileFormat::CoordinateExportFormat::XYZ);
    if (!fileAndFormat.exportData(cfg))
        QMessageBox::warning(this, "Error", "Failed to export the configuration. Check the messages for details.",
                             QMessageBox::Ok, QMessageBox::Ok);
    else
        Messenger::print("Successfully exported configuration '{}' to '{}'.\n", cfg->name(), fileAndFormat.filename());
}

void DissolveWindow::on_ConfigurationAdjustTemperatureAction_triggered(bool checked)
{
    // Get the currently-displayed Configuration
    auto *cfg = ui_.MainTabs->currentConfiguration();
    if (!cfg)
        return;

    auto ok = false;
    auto temperature = QInputDialog::getDouble(this, "Set temperature for configuration",
                                               "Enter the temperature (K) to apply to this configuration", cfg->temperature(),
                                               0.0, 1000000.0, 3, &ok);
    if (!ok)
        return;

    cfg->setTemperature(temperature);

    setModified();

    fullUpdate();
}
