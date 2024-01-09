// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/layerTab.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/avgMol/avgMol.h"
#include "modules/epsr/epsr.h"
#include "modules/gr/gr.h"
#include "modules/registry.h"
#include "modules/sdf/sdf.h"
#include "modules/siteRDF/siteRDF.h"
#include "modules/sq/sq.h"

void DissolveWindow::on_LayerCreateEmptyAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveBasicAtomicAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Evolve (Basic Atomic)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));

    Module *module;
    auto *firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create(ModuleTypes::AtomShake, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add energy calculation
    module = ModuleRegistry::create(ModuleTypes::Energy, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveAtomicAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Evolve (Atomic)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));

    Module *module;
    auto *firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create(ModuleTypes::AtomShake, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add some MD
    module = ModuleRegistry::create(ModuleTypes::MD, newLayer);
    module->keywords().set("Configuration", firstCfg);
    module->setFrequency(5);

    // Add energy calculation
    module = ModuleRegistry::create(ModuleTypes::Energy, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveMolecularAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Evolve (Standard)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));

    Module *module;
    auto *firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add a Monte Carlo shake (MolShake) module
    module = ModuleRegistry::create(ModuleTypes::MolShake, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add some MD
    module = ModuleRegistry::create(ModuleTypes::MD, newLayer);
    module->keywords().set("Configuration", firstCfg);
    module->setFrequency(5);

    // Add energy calculation
    module = ModuleRegistry::create(ModuleTypes::Energy, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveMDAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Evolve (MD)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));

    Module *module;
    auto *firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add MD
    module = ModuleRegistry::create(ModuleTypes::MD, newLayer);
    module->keywords().set("Configuration", firstCfg);
    module->keywords().set("NSteps", 500);
    module->keywords().set("OnlyWhenEnergyStable", false);
    module->setFrequency(1);

    // Add energy calculation
    module = ModuleRegistry::create(ModuleTypes::Energy, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateEvolveEPSRAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Evolve (EPSR)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));

    Module *module;
    auto *firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add some Monte Carlo
    module = ModuleRegistry::create(ModuleTypes::MolShake, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add an intramolecular shake
    module = ModuleRegistry::create(ModuleTypes::IntraShake, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add energy calculation
    module = ModuleRegistry::create(ModuleTypes::Energy, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateRefineEPSRAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Refine (EPSR)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlags(ModuleLayer::RunControlFlag::EnergyStability,
                                         ModuleLayer::RunControlFlag::SizeFactors);

    // Add the EPSR module
    auto *epsr = dynamic_cast<EPSRModule *>(ModuleRegistry::create(ModuleTypes::EPSR, newLayer));

    // Set any suitable module targets
    epsr->keywords().set("Target", Module::allOfType(ModuleTypes::NeutronSQ));

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("GR", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    // Add the GR module
    newLayer->append(ModuleTypes::GR, dissolve_.coreData().configurations());

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFStructureFactorAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("G(r) / Unweighted S(Q)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    // Add the GR module
    newLayer->append(ModuleTypes::GR, dissolve_.coreData().configurations());

    // Add a plain structure factor module
    newLayer->append(ModuleTypes::SQ, dissolve_.coreData().configurations());

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFNeutronAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("G(r) / Neutron S(Q)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    // Add the GR module
    newLayer->append(ModuleTypes::GR, dissolve_.coreData().configurations());

    // Add a plain structure factor module
    newLayer->append(ModuleTypes::SQ, dissolve_.coreData().configurations());

    // Add a NeutronSQ module
    newLayer->append(ModuleTypes::NeutronSQ, dissolve_.coreData().configurations());

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFXRayAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("G(r) / X-ray S(Q)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    // Add the GR module
    newLayer->append(ModuleTypes::GR, dissolve_.coreData().configurations());

    // Add a plain structure factor module
    newLayer->append(ModuleTypes::SQ, dissolve_.coreData().configurations());

    // Add an XRaySQ module
    newLayer->append(ModuleTypes::XRaySQ, dissolve_.coreData().configurations());

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateCorrelationsRDFNeutronXRayAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("G(r) / Neutron S(Q) / X-ray S(Q)", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->setFrequency(5);
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    // Add the GR module
    newLayer->append(ModuleTypes::GR, dissolve_.coreData().configurations());

    // Add a plain structure factor module
    newLayer->append(ModuleTypes::SQ, dissolve_.coreData().configurations());

    // Add a NeutronSQ module
    newLayer->append(ModuleTypes::NeutronSQ, dissolve_.coreData().configurations());

    // Add an XRaySQ module
    newLayer->append(ModuleTypes::XRaySQ, dissolve_.coreData().configurations());

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseRDFCNAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Analyse RDF/CN", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    auto firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add the CalculateGR module
    auto *calcRDFModule = ModuleRegistry::create(ModuleTypes::SiteRDF, newLayer);
    calcRDFModule->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

    setModified();
    fullUpdate();
    ui_.MainTabs->setCurrentTab(newLayer);
}

void DissolveWindow::on_LayerCreateAnalyseAvgMolSDFAction_triggered(bool checked)
{
    auto *newLayer = dissolve_.coreData().addProcessingLayer();
    newLayer->setName(DissolveSys::uniqueName("Analyse AvgMol/SDF", dissolve_.coreData().processingLayers(),
                                              [&](const auto &l) { return newLayer == l.get() ? std::string() : l->name(); }));
    newLayer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);

    auto firstCfg =
        dissolve_.coreData().configurations().empty() ? nullptr : dissolve_.coreData().configurations().front().get();

    // Add the AvgMol module
    auto *module = ModuleRegistry::create(ModuleTypes::AvgMol, newLayer);
    module->keywords().set("Configuration", firstCfg);

    // Add an SDF module
    module = ModuleRegistry::create(ModuleTypes::SDF, newLayer);
    module->keywords().set("Configuration", firstCfg);

    ModuleContext context(dissolve_.worldPool(), dissolve_);

    // Run set-up stages for modules
    newLayer->setUpAll(context);

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
    dissolve_.coreData().removeProcessingLayer(layerTab->moduleLayer());

    setModified();
    fullUpdate();
}
