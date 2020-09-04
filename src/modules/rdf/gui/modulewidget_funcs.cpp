/*
    *** RDF Module Widget - Functions
    *** src/modules/rdf/gui/modulewidget_funcs.cpp
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

#include "classes/atomtype.h"
#include "genericitems/listhelper.h"
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/rdf/gui/modulewidget.h"
#include "modules/rdf/rdf.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

RDFModuleWidget::RDFModuleWidget(QWidget *parent, RDFModule *module, Dissolve &dissolve)
    : ModuleWidget(parent), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up partial g(r) graph
    partialsGraph_ = ui_.PartialsPlotWidget->dataViewer();
    // -- Set view
    partialsGraph_->view().setViewType(View::FlatXYView);
    partialsGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    partialsGraph_->view().axes().setMax(0, 10.0);
    partialsGraph_->view().axes().setTitle(1, "g(r)");
    partialsGraph_->view().axes().setMin(1, -1.0);
    partialsGraph_->view().axes().setMax(1, 1.0);
    partialsGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    partialsGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    partialsGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    partialsGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    partialsGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    partialsGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    partialsGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    partialsGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    partialsGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    partialsGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

    // Set up total G(r) graph
    totalsGraph_ = ui_.TotalsPlotWidget->dataViewer();
    // -- Set view
    totalsGraph_->view().setViewType(View::FlatXYView);
    totalsGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    totalsGraph_->view().axes().setMax(0, 10.0);
    totalsGraph_->view().axes().setTitle(1, "g(r)");
    totalsGraph_->view().axes().setMin(1, -1.0);
    totalsGraph_->view().axes().setMax(1, 1.0);
    totalsGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);
    totalsGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    totalsGraph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;

    currentConfiguration_ = nullptr;

    updateControls();

    setGraphDataTargets(module_);
}

RDFModuleWidget::~RDFModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void RDFModuleWidget::updateControls(int flags)
{
    ui_.PartialsPlotWidget->updateToolbar();
    ui_.TotalsPlotWidget->updateToolbar();

    partialsGraph_->postRedisplay();
    totalsGraph_->postRedisplay();
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool RDFModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!partialsGraph_->writeSession(parser))
        return false;
    if (!totalsGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool RDFModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!partialsGraph_->readSession(parser))
        return false;
    if (!totalsGraph_->readSession(parser))
        return false;

    return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void RDFModuleWidget::setGraphDataTargets(RDFModule *module)
{
    if (!module)
        return;

    // Add Configuration targets to the combo box
    ui_.TargetCombo->clear();
    for (Configuration *config : module->targetConfigurations())
        ui_.TargetCombo->addItem(QString::fromStdString(std::string(config->name())), VariantPointer<Configuration>(config));

    // Loop over Configurations and add total G(R)
    for (Configuration *cfg : module->targetConfigurations())
    {
        // Add calculated total G(r)
        totalsGraph_->createRenderable(Renderable::Data1DRenderable, fmt::format("{}//UnweightedGR//Total", cfg->niceName()),
                                       cfg->niceName(), "Calc");
    }
}

void RDFModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
    // Remove any current data
    partialsGraph_->clearRenderables();

    // Get target Configuration
    currentConfiguration_ = VariantPointer<Configuration>(ui_.TargetCombo->itemData(index));
    if (!currentConfiguration_)
        return;

    const AtomTypeList cfgTypes = currentConfiguration_->usedAtomTypesList();
    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        // Full partial
        partialsGraph_->createRenderable(Renderable::Data1DRenderable,
                                         fmt::format("{}//UnweightedGR//{}//Full", currentConfiguration_->niceName(), id),
                                         fmt::format("{} (Full)", id), "Full");

        // Bound partial
        partialsGraph_->createRenderable(Renderable::Data1DRenderable,
                                         fmt::format("{}//UnweightedGR//{}//Bound", currentConfiguration_->niceName(), id),
                                         fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        partialsGraph_->createRenderable(Renderable::Data1DRenderable,
                                         fmt::format("{}//UnweightedGR//{}//Unbound", currentConfiguration_->niceName(), id),
                                         fmt::format("{} (Unbound)", id), "Unbound");
    });
}
