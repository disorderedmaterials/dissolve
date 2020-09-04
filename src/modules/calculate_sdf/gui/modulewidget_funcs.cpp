/*
    *** CalculateSDF Module Widget - Functions
    *** src/modules/calculate_sdf/gui/modulewidget_funcs.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/helpers/comboboxupdater.h"
#include "gui/render/renderabledata3d.h"
#include "gui/render/renderablespecies.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_sdf/gui/modulewidget.h"
#include "modules/calculate_sdf/sdf.h"

CalculateSDFModuleWidget::CalculateSDFModuleWidget(QWidget *parent, CalculateSDFModule *module, const CoreData &coreData)
    : ModuleWidget(parent), module_(module), coreData_(coreData)
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
    ListIterator<Species> speciesIterator(coreData_.constSpecies());
    while (Species *sp = speciesIterator.iterate())
        refMolecules.append(sp, fmt::format("{} (Species)", sp->name()));
    ComboBoxUpdater<Species> refMoleculeUpdater(ui_.ReferenceMoleculeCombo, refMolecules, referenceMolecule_, 1, 0);

    refreshing_ = false;
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool CalculateSDFModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!sdfGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool CalculateSDFModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!sdfGraph_->readSession(parser))
        return false;

    setGraphDataTargets();

    return true;
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
        sdfRenderable_ = dynamic_cast<RenderableData3D *>(sdfGraph_->createRenderable(
            Renderable::Data3DRenderable, fmt::format("{}//Process3D//{}//SDF", module_->uniqueName(), cfg->niceName()),
            fmt::format("SDF//{}", cfg->niceName()), cfg->niceName()));

        if (sdfRenderable_)
        {
            sdfRenderable_->setColour(StockColours::BlueStockColour);

            auto lowerCutoff = (cfg->nMolecules() / cfg->box()->volume()) * 3.0;
            auto upperCutoff = 1.0;

            // Set cutoff limits and initial values in spin widgets
            ui_.LowerCutoffSpin->setValue(lowerCutoff);
            ui_.UpperCutoffSpin->setValue(upperCutoff);

            sdfRenderable_->setLowerCutoff(lowerCutoff);
            sdfRenderable_->setUpperCutoff(upperCutoff);
        }

        // Reference molecule
        if (referenceMolecule_)
            referenceMoleculeRenderable_ = dynamic_cast<RenderableSpecies *>(sdfGraph_->createRenderable(
                Renderable::SpeciesRenderable, referenceMolecule_->objectTag(), "Reference Molecule"));
    }
}

void CalculateSDFModuleWidget::on_LowerCutoffSpin_valueChanged(double value)
{
    if (refreshing_ || !sdfRenderable_)
        return;

    sdfRenderable_->setLowerCutoff(value);

    sdfGraph_->postRedisplay();
}

void CalculateSDFModuleWidget::on_UpperCutoffSpin_valueChanged(double value)
{
    if (refreshing_ || !sdfRenderable_)
        return;

    sdfRenderable_->setUpperCutoff(value);

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
