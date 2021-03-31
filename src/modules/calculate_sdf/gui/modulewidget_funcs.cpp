// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/helpers/comboboxupdater.h"
#include "gui/render/renderabledata3d.h"
#include "gui/render/renderablespecies.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_sdf/gui/modulewidget.h"
#include "modules/calculate_sdf/sdf.h"

CalculateSDFModuleWidget::CalculateSDFModuleWidget(QWidget *parent, const GenericList &processingData,
                                                   CalculateSDFModule *module, const CoreData &coreData)
    : ModuleWidget(parent, processingData), module_(module), coreData_(coreData)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set limits and step sizes in spin widgets
    ui_.LowerCutoffSpin->setMinimumLimit(0.0);
    ui_.LowerCutoffSpin->setSingleStep(0.01);
    ui_.UpperCutoffSpin->setMinimumLimit(0.0);
    ui_.UpperCutoffSpin->setSingleStep(0.01);

    // Set up SDF graph
    sdfGraph_ = ui_.SDFPlotWidget->dataViewer();

    View &sdfView = sdfGraph_->view();
    sdfView.setViewType(View::NormalView);
    sdfView.axes().setTitle(0, "X, \\sym{angstrom}");
    sdfView.axes().setRange(0, -10.0, 10.0);
    sdfView.axes().setTitle(1, "Y, \\sym{angstrom}");
    sdfView.axes().setRange(1, -10.0, 10.0);
    sdfView.axes().setTitle(2, "Z, \\sym{angstrom}");
    sdfView.axes().setRange(2, -10.0, 10.0);
    sdfView.setAutoFollowType(View::AllAutoFollow);

    sdfRenderable_ = nullptr;
    referenceMolecule_ = nullptr;
    referenceMoleculeRenderable_ = nullptr;

    // Add on "<None>" option for reference molecule
    ui_.ReferenceMoleculeCombo->addItem("<None>", VariantPointer<Species>(nullptr));

    refreshing_ = false;

    updateControls();

    setGraphDataTargets();
}

/*
 * UI
 */

// Update controls within widget
void CalculateSDFModuleWidget::updateControls(int flags)
{
    refreshing_ = true;

    // Update the viewer's toolbar
    ui_.SDFPlotWidget->updateToolbar();

    // Refresh the graph
    sdfGraph_->postRedisplay();

    // Get cutoff limits from data
    if (sdfRenderable_)
    {
        ui_.DataMinValueLabel->setText(QString::number(sdfRenderable_->valuesMin(), 'e', 4));
        ui_.DataMaxValueLabel->setText(QString::number(sdfRenderable_->valuesMax(), 'e', 4));
    }

    // Update available reference molecule combo
    RefDataList<Species, std::string> refMolecules;
    // -- Find available AvgMol results
    RefList<CalculateAvgMolModule> avgMolModules = coreData_.findModulesByClass<CalculateAvgMolModule>();
    for (CalculateAvgMolModule *module : avgMolModules)
        refMolecules.append(&module->averageSpecies(), fmt::format("{} (AvgMol)", module->averageSpecies().name()));
    // -- Add on current species
    for (const auto &sp : coreData_.species())
        refMolecules.append(sp.get(), fmt::format("{} (Species)", sp->name()));
    ComboBoxUpdater<Species> refMoleculeUpdater(ui_.ReferenceMoleculeCombo, refMolecules, referenceMolecule_, 1, 0);

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void CalculateSDFModuleWidget::setGraphDataTargets()
{
    // Remove any current data
    sdfGraph_->clearRenderables();
    sdfRenderable_ = nullptr;
    referenceMoleculeRenderable_ = nullptr;

    if (!module_)
        return;

    // Loop over Configuration targets in Module
    for (Configuration *cfg : module_->targetConfigurations())
    {
        // Calculated SDF
        sdfRenderable_ = sdfGraph_->createRenderable<RenderableData3D>(
            fmt::format("{}//Process3D//{}//SDF", module_->uniqueName(), cfg->niceName()),
            fmt::format("SDF//{}", cfg->niceName()), cfg->niceName());

        if (sdfRenderable_)
        {
            sdfRenderable_->setColour(StockColours::BlueStockColour);

            auto lowerCutoff = (cfg->nMolecules() / cfg->box()->volume()) * 3.0;
            auto upperCutoff = 1.0;

            // Set cutoff limits and initial values in spin widgets
            ui_.LowerCutoffSpin->setValue(lowerCutoff);
            ui_.UpperCutoffSpin->setValue(upperCutoff);

            auto *r3d = dynamic_cast<RenderableData3D *>(sdfRenderable_.get());
            r3d->setLowerCutoff(lowerCutoff);
            r3d->setUpperCutoff(upperCutoff);
        }

        // Reference molecule
        if (referenceMolecule_)
            referenceMoleculeRenderable_ =
                sdfGraph_->createRenderable<RenderableSpecies, Species>(referenceMolecule_, "Reference Molecule");
    }
}

void CalculateSDFModuleWidget::on_LowerCutoffSpin_valueChanged(double value)
{
    if (refreshing_ || !sdfRenderable_)
        return;

    auto *r3d = dynamic_cast<RenderableData3D *>(sdfRenderable_.get());
    r3d->setLowerCutoff(value);

    sdfGraph_->postRedisplay();
}

void CalculateSDFModuleWidget::on_UpperCutoffSpin_valueChanged(double value)
{
    if (refreshing_ || !sdfRenderable_)
        return;

    auto *r3d = dynamic_cast<RenderableData3D *>(sdfRenderable_.get());
    r3d->setUpperCutoff(value);

    sdfGraph_->postRedisplay();
}

void CalculateSDFModuleWidget::on_ReferenceMoleculeCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Check index...
    if (index == -1)
        referenceMolecule_ = nullptr;
    else
        referenceMolecule_ = VariantPointer<Species>(ui_.ReferenceMoleculeCombo->currentData());

    setGraphDataTargets();
}
