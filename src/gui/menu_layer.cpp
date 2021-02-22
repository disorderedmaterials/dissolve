// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/layertab.h"
#include "gui/selectconfigurationdialog.h"
#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_rdf/rdf.h"
#include "modules/calculate_sdf/sdf.h"
#include "modules/epsr/epsr.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"

void DissolveWindow::on_LayerCreateEmptyAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveBasicAtomicAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Basic Atomic)"));

    Module *module;

    // Add some Monte Carlo
    module = dissolve_.createModuleInstance("AtomShake", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add energy calculation
    module = dissolve_.createModuleInstance("Energy", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveAtomicAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Atomic)"));

    Module *module;

    // Add some Monte Carlo
    module = dissolve_.createModuleInstance("AtomShake", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add some MD
    module = dissolve_.createModuleInstance("MD", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());
    module->setFrequency(5);

    // Add energy calculation
    module = dissolve_.createModuleInstance("Energy", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveMolecularAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (Standard)"));

    Module *module;

    // Add a Monte Carlo shake (MolShake) module
    module = dissolve_.createModuleInstance("MolShake", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add some MD
    module = dissolve_.createModuleInstance("MD", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());
    module->setFrequency(5);

    // Add energy calculation
    module = dissolve_.createModuleInstance("Energy", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveEPSRAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Evolve (EPSR)"));

    Module *module;

    // Add some Monte Carlo
    module = dissolve_.createModuleInstance("MolShake", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add an intramolecular shake
    module = dissolve_.createModuleInstance("IntraShake", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add energy calculation
    module = dissolve_.createModuleInstance("Energy", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateRefineEPSRAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Refine (EPSR)"));
    newLayer->setFrequency(5);

    // Add the EPSR module
    EPSRModule *epsr = dynamic_cast<EPSRModule *>(dissolve_.createModuleInstance("EPSR", newLayer));

    // Set any suitable module targets
    epsr->keywords().set<std::vector<Module *>>("Target", dissolve_.findModuleInstances("NeutronSQ"));

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCalculateRDFAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF"));
    newLayer->setFrequency(5);

    // Add the RDF module
    Module *module = dissolve_.createModuleInstance("RDF", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCalculateRDFStructureFactorAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Unweighted S(Q)"));
    newLayer->setFrequency(5);

    // Add the RDF module
    RDFModule *rdfModule = dynamic_cast<RDFModule *>(dissolve_.createModuleInstance("RDF", newLayer));
    rdfModule->addTargetConfigurations(dissolve_.configurations());

    // Add a plain structure factor module
    Module *module = dissolve_.createModuleInstance("SQ", newLayer);
    module->keywords().set<const RDFModule *>("SourceRDFs", rdfModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCalculateRDFNeutronAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Neutron S(Q)"));
    newLayer->setFrequency(5);

    // Add the RDF module
    RDFModule *rdfModule = dynamic_cast<RDFModule *>(dissolve_.createModuleInstance("RDF", newLayer));
    rdfModule->addTargetConfigurations(dissolve_.configurations());

    // Add a plain structure factor module
    SQModule *sqModule = dynamic_cast<SQModule *>(dissolve_.createModuleInstance("SQ", newLayer));
    sqModule->keywords().set<const RDFModule *>("SourceRDFs", rdfModule);

    // Add a NeutronSQ module
    Module *module = dissolve_.createModuleInstance("NeutronSQ", newLayer);
    module->keywords().set<const SQModule *>("SourceSQs", sqModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCalculateRDFNeutronXRayAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("RDF / Neutron S(Q) / X-Ray S(Q)"));
    newLayer->setFrequency(5);

    // Add the RDF module
    RDFModule *rdfModule = dynamic_cast<RDFModule *>(dissolve_.createModuleInstance("RDF", newLayer));
    rdfModule->addTargetConfigurations(dissolve_.configurations());

    // Add a plain structure factor module
    SQModule *sqModule = dynamic_cast<SQModule *>(dissolve_.createModuleInstance("SQ", newLayer));
    sqModule->keywords().set<const RDFModule *>("SourceRDFs", rdfModule);

    // Add a NeutronSQ module
    Module *module = dissolve_.createModuleInstance("NeutronSQ", newLayer);
    module->keywords().set<const SQModule *>("SourceSQs", sqModule);

    // Add an XRaySQ module
    module = dissolve_.createModuleInstance("XRaySQ", newLayer);
    module->keywords().set<const SQModule *>("SourceSQs", sqModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseRDFCNAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Analyse RDF/CN"));

    Module *module;
    CalculateRDFModule *calcRDFModule;

    // Add the CalculateRDF module
    calcRDFModule = dynamic_cast<CalculateRDFModule *>(dissolve_.createModuleInstance("CalculateRDF", newLayer));
    calcRDFModule->addTargetConfigurations(dissolve_.configurations());

    // Add a CalculateCN module
    module = dissolve_.createModuleInstance("CalculateCN", newLayer);
    module->keywords().set<const CalculateRDFModule *>("SourceRDF", calcRDFModule);

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseAvgMolSDFAction_triggered(bool checked)
{
    ModuleLayer *newLayer = dissolve_.addProcessingLayer();
    newLayer->setName(dissolve_.uniqueProcessingLayerName("Analyse AvgMol/SDF"));

    Module *module;

    // Add the CalculateAvgMol module
    module = dissolve_.createModuleInstance("CalculateAvgMol", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Add a CalculateSDF module
    module = dissolve_.createModuleInstance("CalculateSDF", newLayer);
    module->addTargetConfigurations(dissolve_.configurations());

    // Run set-up stages for modules
    newLayer->setUpAll(dissolve_, dissolve_.worldPool());

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerRenameAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a LayerTab, then call its rename() function
    MainTab *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::LayerTabType))
        return;
    tab->rename();
}

void DissolveWindow::on_LayerDeleteAction_triggered(bool checked)
{
    // Get the current tab - make sure it is a ConfigurationTab
    MainTab *tab = ui_.MainTabs->currentTab();
    if ((!tab) || (tab->type() != MainTab::ConfigurationTabType))
        return;

    // Cast up the tab to a ConfigurationTab so we can get the ModuleLayer pointer
    LayerTab *layerTab = dynamic_cast<LayerTab *>(tab);
    if (!layerTab)
        return;

    // Check that we really want to delete the layer
    if (!layerTab->close())
        return;

    // Update the GUI
    ui_.MainTabs->removeByPage(layerTab->page());
    setModified();
    fullUpdate();
}
