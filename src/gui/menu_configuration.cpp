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
        std::shared_ptr<AddProcedureNode> add;

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
            auto coordSets = std::make_shared<CoordinateSetsProcedureNode>(sp);
            coordSets->setName(fmt::format("{}_Sets", sp->name()));
            generator.addRootSequenceNode(coordSets);

            // Create the Add node
            add = std::make_shared<AddProcedureNode>(coordSets.get(), NodeValue(popString, paramsNode->parameters()),
                                                     NodeValue("rho", paramsNode->parameters()));
        }
        else
            add = std::make_shared<AddProcedureNode>(sp, NodeValue(popString, paramsNode->parameters()),
                                                     NodeValue("rho", paramsNode->parameters()));

        // Add the node to the generator
        add->setName(sp->name());
        generator.addRootSequenceNode(add);

        addNodes.emplace_back(add);

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
    auto paramsNode = std::make_shared<ParametersProcedureNode>();
    paramsNode->addParameter("rho", 0.1);
    generator.addRootSequenceNode(paramsNode);
    generator.addRootSequenceNode(std::make_shared<BoxProcedureNode>());
    for (const auto *sp : mixSpecies)
    {
        std::shared_ptr<AddProcedureNode> add;

        // Set up coordinate set, but only if we have a suitable species
        if (sp->nAtoms() > 1)
        {
            auto coordSets = std::make_shared<CoordinateSetsProcedureNode>(sp);
            coordSets->setName(fmt::format("{}_Sets", sp->name()));
            generator.addRootSequenceNode(coordSets);
            add = std::make_shared<AddProcedureNode>(coordSets.get(), 100, NodeValue("rho", paramsNode->parameters()));
        }
        else
            add = std::make_shared<AddProcedureNode>(sp, 100, NodeValue("rho", paramsNode->parameters()));

        generator.addRootSequenceNode(add);
        add->setName(sp->name());
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
    auto paramsNode = std::make_shared<ParametersProcedureNode>();
    paramsNode->addParameter("populationA", 100);
    paramsNode->addParameter("rho", 0.1);
    generator.addRootSequenceNode(paramsNode);
    generator.addRootSequenceNode(std::make_shared<BoxProcedureNode>());

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
    auto node = std::make_shared<AddProcedureNode>(framework, 1);
    node->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::Set);
    node->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
    node->keywords().set("Rotate", false);
    generator.addRootSequenceNode(node);

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
    auto node = std::make_shared<AddProcedureNode>(framework, 1);
    node->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::Set);
    node->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
    node->keywords().set("Rotate", false);
    generator.addRootSequenceNode(node);

    // Add a parameters node
    auto paramsNode = std::make_shared<ParametersProcedureNode>();
    paramsNode->addParameter("populationA", 100);
    generator.addRootSequenceNode(paramsNode);

    // Add a GeneralRegion node
    auto regionNode = std::make_shared<GeneralRegionProcedureNode>();
    regionNode->keywords().set("Tolerance", 5.0);
    generator.addRootSequenceNode(regionNode);

    // Create a relative mix from the selected components
    auto addNodes = createRelativeMix(adsorbates, generator, paramsNode);
    for (auto &addNode : addNodes)
    {
        addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
        addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Region);
        addNode->keywords().set<std::shared_ptr<RegionProcedureNodeBase>>("Region", regionNode);
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
