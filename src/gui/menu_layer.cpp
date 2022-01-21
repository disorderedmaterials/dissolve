// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/selectconfigurationdialog.h"
#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_rdf/rdf.h"
#include "modules/calculate_sdf/sdf.h"
#include "modules/epsr/epsr.h"
#include "modules/rdf/rdf.h"
#include "modules/registry.h"
#include "modules/sq/sq.h"

void DissolveWindow::on_LayerCreateEmptyAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveBasicAtomicAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Basic Atomic)"));

    Module *module;
    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create("AtomShake", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add energy calculation
    module = ModuleRegistry::create("Energy", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveAtomicAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Atomic)"));

    Module *module;
    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create("AtomShake", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add some MD
    module = ModuleRegistry::create("MD", newLayer);
    module->keywords().set("Configuration", firstCfg);
    module->setFrequency(5);

    // Add energy calculation
    module = ModuleRegistry::create("Energy", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveMolecularAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Standard)"));

    Module *module;
    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add a Monte Carlo shake (MolShake) module
    module = ModuleRegistry::create("MolShake", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add some MD
    module = ModuleRegistry::create("MD", newLayer);
    module->keywords().set("Configuration", firstCfg);
    module->setFrequency(5);

    // Add energy calculation
    module = ModuleRegistry::create("Energy", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveEPSRAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (EPSR)"));

    Module *module;
    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create("MolShake", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add an intramolecular shake
    module = ModuleRegistry::create("IntraShake", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add energy calculation
    module = ModuleRegistry::create("Energy", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateRefineEPSRAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Refine (EPSR)"));
    newLayer->setFrequency(5);

    // Add the EPSR module
    auto *epsr = dynamic_cast<EPSRModule *>(ModuleRegistry::create("EPSR", newLayer));

    // Set any suitable module targets
    epsr->keywords().set("Target", Module::allOfType("NeutronSQ"));

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF"));
    newLayer->setFrequency(5);

    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the RDF module
    auto *module = ModuleRegistry::create("RDF", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFStructureFactorAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Unweighted S(Q)"));
    newLayer->setFrequency(5);

    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the RDF module
    auto *rdfModule = dynamic_cast<RDFModule *>(ModuleRegistry::create("RDF", newLayer));
    rdfModule->keywords().set("Configuration", firstCfg);

    // Add a plain structure factor module
    auto *module = ModuleRegistry::create("SQ", newLayer);
    module->keywords().set<const Module *>("SourceRDFs", rdfModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFNeutronAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Neutron S(Q)"));
    newLayer->setFrequency(5);

    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the RDF module
    auto *rdfModule = ModuleRegistry::create("RDF", newLayer);
    rdfModule->keywords().set("Configuration", firstCfg);

    // Add a plain structure factor module
    auto *sqModule = dynamic_cast<SQModule *>(ModuleRegistry::create("SQ", newLayer));
    sqModule->keywords().set<const Module *>("SourceRDFs", rdfModule);

    // Add a NeutronSQ module
    Module *module = ModuleRegistry::create("NeutronSQ", newLayer);
    module->keywords().set<const Module *>("SourceSQs", sqModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFNeutronXRayAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Neutron S(Q) / X-Ray S(Q)"));
    newLayer->setFrequency(5);

    auto *firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the RDF module
    auto *rdfModule = ModuleRegistry::create("RDF", newLayer);
    rdfModule->keywords().set("Configuration", firstCfg);

    // Add a plain structure factor module
    auto *sqModule = ModuleRegistry::create("SQ", newLayer);
    sqModule->keywords().set<const Module *>("SourceRDFs", rdfModule);

    // Add a NeutronSQ module
    auto *module = ModuleRegistry::create("NeutronSQ", newLayer);
    module->keywords().set<const Module *>("SourceSQs", sqModule);

    // Add an XRaySQ module
    module = ModuleRegistry::create("XRaySQ", newLayer);
    module->keywords().set<const Module *>("SourceSQs", sqModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseRDFCNAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Analyse RDF/CN"));

    auto firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the CalculateRDF module
    auto *calcRDFModule = ModuleRegistry::create("CalculateRDF", newLayer);
    calcRDFModule->keywords().set("Configuration", firstCfg);

    // Add a CalculateCN module
    auto *module = ModuleRegistry::create("CalculateCN", newLayer);
    module->keywords().set<const Module *>("SourceRDF", calcRDFModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseAvgMolSDFAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Analyse AvgMol/SDF"));

    auto firstCfg = dissolve_.configurations().empty() ? nullptr : dissolve_.configurations().front().get();

    // Add the CalculateAvgMol module
    auto *module = ModuleRegistry::create("CalculateAvgMol", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add a CalculateSDF module
    module = ModuleRegistry::create("CalculateSDF", newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a LayerTab, then call its rename() function
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Layer))
        return;
    tab->rename();
}

void DissolveWindow::on_LayerDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab
    auto tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::TabType::Layer))
        return;

    // Cast up the tab to a LayerTab
    auto layerTab = dynamic_cast<LayerTab *>(tab);
    if (!layerTab)
        return;

    // Check that we really want to delete the layer
    if (!layerTab->canClose())
        return;

    ui_.MainTabs->removeByPage(layerTab->page());
    dissolve_.removeProcessingLayer(layerTab->moduleLayer());

    setModified();
    fullUpdate();
}
