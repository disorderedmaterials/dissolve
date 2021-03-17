// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/selectspeciesdialog.h"
#include "io/export/coordinates.h"
#include "main/dissolve.h"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/parameters.h"
#include <QFileDialog>
#include <QMessageBox>

void DissolveWindow::on_ConfigurationCreateEmptyAction_triggered(bool checked)
{
    auto *newConfiguration = dissolve_.addConfiguration();

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the Species to add to the mix
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

    auto mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
    if (mixSpecies.nItems() == 0)
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto *paramsNode = new ParametersProcedureNode;
    paramsNode->addParameter("rho", 0.1);
    generator.addRootSequenceNode(paramsNode);
    generator.addRootSequenceNode(new BoxProcedureNode);
    for (auto sp : mixSpecies)
    {
        generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, 100, NodeValue("rho", paramsNode->parameters())));
    }

    // Run the generator
    newConfiguration->generate(dissolve_.worldPool(), dissolve_.pairPotentialRange());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the Species to add to the mix
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

    RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
    if (mixSpecies.nItems() == 0)
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto *paramsNode = new ParametersProcedureNode;
    paramsNode->addParameter("populationA", 100);
    paramsNode->addParameter("rho", 0.1);
    generator.addRootSequenceNode(paramsNode);
    generator.addRootSequenceNode(new BoxProcedureNode);
    auto count = 0;
    for (auto *sp : mixSpecies)
    {
        // Add a parameter for the ratio of this species to the first (or the population of the first)
        if (count == 0)
            generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue("populationA", paramsNode->parameters()),
                                                                      NodeValue("rho", paramsNode->parameters())));
        else
        {
            auto parameterName = fmt::format("ratio{}", char(65 + count));
            paramsNode->addParameter(parameterName, 1);

            generator.addRootSequenceNode(new AddSpeciesProcedureNode(
                sp, NodeValue(fmt::format("{}*populationA", parameterName), paramsNode->parameters()),
                NodeValue("rho", paramsNode->parameters())));
        }

        ++count;
    }

    // Run the generator
    newConfiguration->generate(dissolve_.worldPool(), dissolve_.pairPotentialRange());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab, then call its rename() function
    auto *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::ConfigurationTabType))
        return;
    tab->rename();
}

void DissolveWindow::on_ConfigurationDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab
    auto *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::ConfigurationTabType))
        return;

    // Cast up the tab to a ConfigurationTab so we can get the ModuleLayer pointer
    auto *cfgTab = dynamic_cast<ConfigurationTab *>(tab);
    if (!cfgTab)
        return;

    // Check that we really want to delete the Configuration
    if (!cfgTab->close())
        return;

    // Update the GUI
    ui_.MainTabs->removeByPage(cfgTab->page());
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

    CoordinateExportFileFormat fileAndFormat(qPrintable(exportFile), CoordinateExportFileFormat::XYZCoordinates);
    if (!fileAndFormat.exportData(cfg))
        QMessageBox::warning(this, "Error", "Failed to export the configuration. Check the messages for details.",
                             QMessageBox::Ok, QMessageBox::Ok);
    else
        Messenger::print("Successfully exported configuration '{}' to '{}'.\n", cfg->name(), fileAndFormat.filename());
}
