// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/selectspeciesdialog.h"
#include "io/export/coordinates.h"
#include "main/dissolve.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinatesets.h"
#include "procedure/nodes/generalregion.h"
#include "procedure/nodes/parameters.h"
#include <QFileDialog>
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

    auto mixSpecies = speciesSelectDialog.selectSpecies(SpeciesFilterProxy::FilterFlags::NoPeriodicBox);
    if (mixSpecies.empty())
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto paramsNode = generator.createRootNode<ParametersProcedureNode>({});
    paramsNode->addParameter("rho", 0.1);

    generator.createRootNode<BoxProcedureNode>({});

    for (const auto *sp : mixSpecies)
    {
        // Set up coordinate set, but only if we have a suitable species
        if (sp->nAtoms() > 1)
        {
            auto coordSets = generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("{}_Sets", sp->name()), sp);
            generator.createRootNode<AddProcedureNode>(sp->name(), coordSets, 100, NodeValue("rho", paramsNode->parameters()));
        }
        else
            generator.createRootNode<AddProcedureNode>(sp->name(), sp, 100, NodeValue("rho", paramsNode->parameters()));
    }

    // Run the generator
    newConfiguration->generate({dissolve_.worldPool(), dissolve_.potentialMap()});

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the Species to add to the mix
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

    auto mixSpecies = speciesSelectDialog.selectSpecies(SpeciesFilterProxy::FilterFlags::NoPeriodicBox);
    if (mixSpecies.empty())
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto paramsNode = generator.createRootNode<ParametersProcedureNode>({});
    paramsNode->addParameter("populationA", 100);
    paramsNode->addParameter("rho", 0.1);
    generator.createRootNode<BoxProcedureNode>({});

    // Create a relative mix from the selected components
    createRelativeMix(mixSpecies, generator, paramsNode);

    // Run the generator
    newConfiguration->generate({dissolve_.worldPool(), dissolve_.potentialMap()});

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateEmptyFrameworkAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the framework Species
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select framework species");

    const auto *framework = speciesSelectDialog.selectSingleSpecies(SpeciesFilterProxy::HasPeriodicBox);
    if (!framework)
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto frameworkNode = generator.createRootNode<AddProcedureNode>("Framework", framework, 1);
    frameworkNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::Set);
    frameworkNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
    frameworkNode->keywords().set("Rotate", false);

    // Run the generator
    newConfiguration->generate({dissolve_.worldPool(), dissolve_.potentialMap()});

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateFrameworkAdsorbatesAction_triggered(bool checked)
{
    // Create a SpeciesSelectDialog and use it to get the framework Species
    SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select framework species");

    const auto *framework = speciesSelectDialog.selectSingleSpecies(SpeciesFilterProxy::HasPeriodicBox);
    if (!framework)
        return;

    // Now retrieve adsorbate species
    const auto adsorbates = speciesSelectDialog.selectSpecies(SpeciesFilterProxy::NoPeriodicBox);
    if (adsorbates.empty())
        return;

    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.addConfiguration();
    auto &generator = newConfiguration->generator();
    auto frameworkNode = generator.createRootNode<AddProcedureNode>("Framework", framework, 1);
    frameworkNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::Set);
    frameworkNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
    frameworkNode->keywords().set("Rotate", false);

    // Add a parameters node
    auto paramsNode = generator.createRootNode<ParametersProcedureNode>({});
    paramsNode->addParameter("populationA", 100);

    // Add a GeneralRegion node
    auto regionNode = generator.createRootNode<GeneralRegionProcedureNode>({});
    regionNode->keywords().set("Tolerance", 5.0);

    // Create a relative mix from the selected components
    auto addNodes = createRelativeMix(adsorbates, generator, paramsNode);
    for (auto &addNode : addNodes)
    {
        addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
        addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Region);
        addNode->keywords().set("Region", regionNode);
    }

    // Run the generator
    newConfiguration->generate({dissolve_.worldPool(), dissolve_.potentialMap()});

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newConfiguration);
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
    dissolve_.removeConfiguration(cfgTab->configuration());

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
