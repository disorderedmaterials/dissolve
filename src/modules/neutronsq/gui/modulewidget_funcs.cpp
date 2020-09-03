/*
    *** Neutron SQ Module Widget - Functions
    *** src/modules/neutronsq/gui/modulewidget_funcs.cpp
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
#include "modules/neutronsq/gui/modulewidget.h"
#include "modules/neutronsq/neutronsq.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

NeutronSQModuleWidget::NeutronSQModuleWidget(QWidget *parent, NeutronSQModule *module, Dissolve &dissolve)
    : ModuleWidget(parent), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up partial g(r) graph
    partialGRGraph_ = ui_.PartialGRPlotWidget->dataViewer();
    // -- Set view
    partialGRGraph_->view().setViewType(View::FlatXYView);
    partialGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    partialGRGraph_->view().axes().setMax(0, 10.0);
    partialGRGraph_->view().axes().setTitle(1, "g(r)");
    partialGRGraph_->view().axes().setMin(1, -1.0);
    partialGRGraph_->view().axes().setMax(1, 1.0);
    partialGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    partialGRGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    partialGRGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    partialGRGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    partialGRGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    partialGRGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    partialGRGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    partialGRGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    partialGRGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    partialGRGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

    // Set up partial S(Q) graph
    partialSQGraph_ = ui_.PartialSQPlotWidget->dataViewer();
    // -- Set view
    partialSQGraph_->view().setViewType(View::FlatXYView);
    partialSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    partialSQGraph_->view().axes().setMax(0, 10.0);
    partialSQGraph_->view().axes().setTitle(1, "S(Q)");
    partialSQGraph_->view().axes().setMin(1, -1.0);
    partialSQGraph_->view().axes().setMax(1, 1.0);
    partialSQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    partialSQGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    partialSQGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    partialSQGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    partialSQGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    partialSQGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    partialSQGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    partialSQGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    partialSQGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    partialSQGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);

    // Set up total G(r) graph
    totalGRGraph_ = ui_.TotalGRPlotWidget->dataViewer();
    // -- Set view
    totalGRGraph_->view().setViewType(View::FlatXYView);
    totalGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    totalGRGraph_->view().axes().setMax(0, 10.0);
    totalGRGraph_->view().axes().setTitle(1, "G(r)");
    totalGRGraph_->view().axes().setMin(1, -1.0);
    totalGRGraph_->view().axes().setMax(1, 1.0);
    totalGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    totalGRGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    totalGRGraph_->groupManager().setGroupColouring("Reference", RenderableGroup::FixedGroupColouring);
    totalGRGraph_->groupManager().setGroupFixedColour("Reference", StockColours::RedStockColour);

    // Set up total F(Q) graph
    totalFQGraph_ = ui_.TotalSQPlotWidget->dataViewer();
    // -- Set view
    totalFQGraph_->view().setViewType(View::FlatXYView);
    totalFQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    totalFQGraph_->view().axes().setMax(0, 10.0);
    totalFQGraph_->view().axes().setTitle(1, "F(Q)");
    totalFQGraph_->view().axes().setMin(1, -1.0);
    totalFQGraph_->view().axes().setMax(1, 1.0);
    totalFQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::NoVerticalShift);
    totalFQGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    totalFQGraph_->groupManager().setGroupColouring("Reference", RenderableGroup::FixedGroupColouring);
    totalFQGraph_->groupManager().setGroupFixedColour("Reference", StockColours::RedStockColour);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

NeutronSQModuleWidget::~NeutronSQModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void NeutronSQModuleWidget::updateControls(int flags)
{
    ui_.PartialGRPlotWidget->updateToolbar();
    ui_.PartialSQPlotWidget->updateToolbar();
    ui_.TotalGRPlotWidget->updateToolbar();
    ui_.TotalSQPlotWidget->updateToolbar();

    // Clear and recreate graph data targets?
    if (flags & ModuleWidget::ResetGraphDataTargetsFlag)
        setGraphDataTargets(module_);

    partialGRGraph_->postRedisplay();
    partialSQGraph_->postRedisplay();
    totalGRGraph_->postRedisplay();
    totalFQGraph_->postRedisplay();
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool NeutronSQModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!partialGRGraph_->writeSession(parser))
        return false;
    if (!partialSQGraph_->writeSession(parser))
        return false;
    if (!totalGRGraph_->writeSession(parser))
        return false;
    if (!totalFQGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool NeutronSQModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!partialGRGraph_->readSession(parser))
        return false;
    if (!partialSQGraph_->readSession(parser))
        return false;
    if (!totalGRGraph_->readSession(parser))
        return false;
    if (!totalFQGraph_->readSession(parser))
        return false;

    return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void NeutronSQModuleWidget::setGraphDataTargets(NeutronSQModule *module)
{
    // Clear any current renderables
    ui_.PartialGRPlotWidget->clearRenderableData();
    ui_.PartialSQPlotWidget->clearRenderableData();
    ui_.TotalGRPlotWidget->clearRenderableData();
    ui_.TotalSQPlotWidget->clearRenderableData();

    // Add partials
    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        /*
         * Partial g(r)
         */

        // Full partial
        partialGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedGR//{}//Full", module_->uniqueName(), id),
                                          fmt::format("{} (Full)", id), "Full");

        // Bound partial
        partialGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedGR//{}//Bound", module_->uniqueName(), id),
                                          fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        partialGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedGR//{}//Unbound", module_->uniqueName(), id),
                                          fmt::format("{} (Unbound)", id), "Unbound");

        /*
         * Partial S(Q)
         */

        // Full partial
        partialSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedSQ//{}//Full", module_->uniqueName(), id),
                                          fmt::format("{} (Full)", id), "Full");

        // Bound partial
        partialSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedSQ//{}//Bound", module_->uniqueName(), id),
                                          fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        partialSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//WeightedSQ//{}//Unbound", module_->uniqueName(), id),
                                          fmt::format("{} (Unbound)", id), "Unbound");
    });

    // Add calculated total G(r)
    auto *totalGR = totalGRGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//WeightedGR//Total", module_->uniqueName()), "Calculated (Direct)");
    totalGRGraph_->addRenderableToGroup(totalGR, "Calculated");

    // Add calculated total representative G(r) (from FT of S(Q))
    auto *repGR = totalGRGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//RepresentativeTotalGR", module_->uniqueName()), "Calculated (via FT)");
    repGR->lineStyle().setStipple(LineStipple::HalfDashStipple);
    totalGRGraph_->addRenderableToGroup(repGR, "Calculated");

    // Add calculate total F(Q)
    auto *totalFQ = totalFQGraph_->createRenderable(Renderable::Data1DRenderable,
                                                    fmt::format("{}//WeightedSQ//Total", module_->uniqueName()), "Calculated");
    totalFQGraph_->addRenderableToGroup(totalFQ, "Calculated");

    // Add on reference data if present
    const Data1DImportFileFormat &referenceFileAndFormat = module->referenceFQFileAndFormat();
    if (referenceFileAndFormat.hasValidFileAndFormat())
    {
        // Add FT of reference data total G(r)
        auto *refGR = totalGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                                      fmt::format("{}//ReferenceDataFT", module_->uniqueName()), "Reference");
        totalGRGraph_->addRenderableToGroup(refGR, "Reference");

        // Add calculate total F(Q)
        auto *refFQ = totalFQGraph_->createRenderable(Renderable::Data1DRenderable,
                                                      fmt::format("{}//ReferenceData", module_->uniqueName()), "Reference");
        totalFQGraph_->addRenderableToGroup(refFQ, "Reference");
    }
}
