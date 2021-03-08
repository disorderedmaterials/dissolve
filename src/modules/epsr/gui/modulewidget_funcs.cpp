// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "module/group.h"
#include "modules/epsr/epsr.h"
#include "modules/epsr/gui/modulewidget.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

EPSRModuleWidget::EPSRModuleWidget(QWidget *parent, EPSRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent), dissolve_(dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = true;

    NumberFormat numberFormat;

    // F(Q) Graph
    FQGraph_ = ui_.FQPlotWidget->dataViewer();

    // -- Set view
    FQGraph_->view().setViewType(View::FlatXYView);
    FQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    FQGraph_->view().axes().setMax(0, 10.0);
    FQGraph_->view().axes().setTitle(1, "F(\\it{Q})");
    FQGraph_->view().axes().setMin(1, -1.0);
    FQGraph_->view().axes().setMax(1, 1.0);
    FQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
    FQGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    FQGraph_->groupManager().setGroupColouring("Exp", RenderableGroup::AutomaticIndividualColouring);
    FQGraph_->groupManager().setGroupVerticalShifting("Exp", RenderableGroup::IndividualVerticalShifting);
    FQGraph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);
    FQGraph_->groupManager().setGroupVerticalShifting("Calc", RenderableGroup::IndividualVerticalShifting);
    FQGraph_->groupManager().setGroupStipple("Calc", LineStipple::QuarterDashStipple);
    FQGraph_->groupManager().setGroupColouring("Delta", RenderableGroup::AutomaticIndividualColouring);
    FQGraph_->groupManager().setGroupVerticalShifting("Delta", RenderableGroup::IndividualVerticalShifting);
    FQGraph_->groupManager().setGroupStipple("Delta", LineStipple::DotStipple);

    // F(Q) Fitting Graph
    FQFitGraph_ = ui_.FQFitPlotWidget->dataViewer();

    // -- Set view
    FQFitGraph_->view().setViewType(View::FlatXYView);
    FQFitGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    FQFitGraph_->view().axes().setMax(0, 10.0);
    FQFitGraph_->view().axes().setTitle(1, "\\sym{Delta}F(\\it{Q})");
    FQFitGraph_->view().axes().setMin(1, -1.0);
    FQFitGraph_->view().axes().setMax(1, 1.0);
    FQFitGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::HalfVerticalShift);
    FQFitGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    FQFitGraph_->groupManager().setGroupColouring("Delta", RenderableGroup::AutomaticIndividualColouring);
    FQFitGraph_->groupManager().setGroupVerticalShifting("Delta", RenderableGroup::IndividualVerticalShifting);
    FQFitGraph_->groupManager().setGroupColouring("Fit", RenderableGroup::AutomaticIndividualColouring);
    FQFitGraph_->groupManager().setGroupVerticalShifting("Fit", RenderableGroup::IndividualVerticalShifting);
    FQFitGraph_->groupManager().setGroupStipple("Fit", LineStipple::QuarterDashStipple);

    // Partial S(Q) Graph
    estimatedSQGraph_ = ui_.EstimatedSQPlotWidget->dataViewer();

    // -- Set view
    estimatedSQGraph_->view().setViewType(View::FlatXYView);
    estimatedSQGraph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    estimatedSQGraph_->view().axes().setMax(0, 10.0);
    estimatedSQGraph_->view().axes().setTitle(1, "S(\\it{Q})");
    estimatedSQGraph_->view().axes().setMin(1, -1.0);
    estimatedSQGraph_->view().axes().setMax(1, 1.0);
    estimatedSQGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    estimatedSQGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    estimatedSQGraph_->groupManager().setGroupColouring("Estimated", RenderableGroup::AutomaticIndividualColouring);
    estimatedSQGraph_->groupManager().setGroupVerticalShifting("Estimated", RenderableGroup::IndividualVerticalShifting);
    estimatedSQGraph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);
    estimatedSQGraph_->groupManager().setGroupVerticalShifting("Calc", RenderableGroup::IndividualVerticalShifting);
    estimatedSQGraph_->groupManager().setGroupStipple("Calc", LineStipple::QuarterDashStipple);
    estimatedSQGraph_->groupManager().setGroupColouring("Delta", RenderableGroup::AutomaticIndividualColouring);
    estimatedSQGraph_->groupManager().setGroupVerticalShifting("Delta", RenderableGroup::IndividualVerticalShifting);
    estimatedSQGraph_->groupManager().setGroupStipple("Delta", LineStipple::DotStipple);

    // g(r) Graph
    estimatedGRGraph_ = ui_.EstimatedGRPlotWidget->dataViewer();

    // -- Set view
    estimatedGRGraph_->view().setViewType(View::FlatXYView);
    estimatedGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    estimatedGRGraph_->view().axes().setMax(0, 10.0);
    estimatedGRGraph_->view().axes().setTitle(1, "g(r)");
    estimatedGRGraph_->view().axes().setMin(1, -1.0);
    estimatedGRGraph_->view().axes().setMax(1, 1.0);
    estimatedGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    estimatedGRGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    estimatedGRGraph_->groupManager().setGroupColouring("Estimated", RenderableGroup::AutomaticIndividualColouring);
    estimatedGRGraph_->groupManager().setGroupVerticalShifting("Estimated", RenderableGroup::IndividualVerticalShifting);
    estimatedGRGraph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);
    estimatedGRGraph_->groupManager().setGroupVerticalShifting("Calc", RenderableGroup::IndividualVerticalShifting);
    estimatedGRGraph_->groupManager().setGroupStipple("Calc", LineStipple::QuarterDashStipple);

    // F(r) Graph
    totalGRGraph_ = ui_.TotalGRPlotWidget->dataViewer();
    // -- Set view
    totalGRGraph_->view().setViewType(View::FlatXYView);
    totalGRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    totalGRGraph_->view().axes().setMax(0, 10.0);
    totalGRGraph_->view().axes().setTitle(1, "F(r)");
    totalGRGraph_->view().axes().setMin(1, -1.0);
    totalGRGraph_->view().axes().setMax(1, 1.0);
    totalGRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    totalGRGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    totalGRGraph_->groupManager().setGroupColouring("Exp", RenderableGroup::AutomaticIndividualColouring);
    totalGRGraph_->groupManager().setGroupVerticalShifting("Exp", RenderableGroup::IndividualVerticalShifting);
    totalGRGraph_->groupManager().setGroupColouring("Calc", RenderableGroup::AutomaticIndividualColouring);
    totalGRGraph_->groupManager().setGroupVerticalShifting("Calc", RenderableGroup::IndividualVerticalShifting);
    totalGRGraph_->groupManager().setGroupStipple("Calc", LineStipple::QuarterDashStipple);

    // Phi(r) (Empirical Potentials) Graph
    phiRGraph_ = ui_.PhiRPlotWidget->dataViewer();
    // -- Set view
    phiRGraph_->view().setViewType(View::FlatXYView);
    phiRGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    phiRGraph_->view().axes().setMax(0, 10.0);
    phiRGraph_->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
    phiRGraph_->view().axes().setMin(1, -1.0);
    phiRGraph_->view().axes().setMax(1, 1.0);
    phiRGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    phiRGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    phiRGraph_->groupManager().setGroupColouring("Phi", RenderableGroup::AutomaticIndividualColouring);

    // phi(r) Magnitude Graph
    phiMagGraph_ = ui_.PhiMagPlotWidget->dataViewer();
    // -- Set view
    phiMagGraph_->view().setViewType(View::FlatXYView);
    phiMagGraph_->view().axes().setTitle(0, "Iteration");
    numberFormat = phiMagGraph_->view().axes().numberFormat(0);
    numberFormat.setType(NumberFormat::IntegerFormat);
    phiMagGraph_->view().axes().setNumberFormat(0, numberFormat);
    phiMagGraph_->view().axes().setMax(0, 10.0);
    phiMagGraph_->view().axes().setTitle(1, "\\sym{Delta}\\sym{phi}(\\it{r}), kJ mol\\sup{-1} \\sym{angstrom}\\sup{-1}");
    phiMagGraph_->view().axes().setMin(1, 0.0);
    phiMagGraph_->view().axes().setMax(1, 1.0);
    phiMagGraph_->view().setAutoFollowType(View::XAutoFollow);

    // R-Factor Graph
    rFactorGraph_ = ui_.RFactorPlotWidget->dataViewer();
    // -- Set view
    rFactorGraph_->view().setViewType(View::FlatXYView);
    rFactorGraph_->view().axes().setTitle(0, "Iteration");
    numberFormat = rFactorGraph_->view().axes().numberFormat(0);
    numberFormat.setType(NumberFormat::IntegerFormat);
    rFactorGraph_->view().axes().setNumberFormat(0, numberFormat);
    rFactorGraph_->view().axes().setMax(0, 10.0);
    rFactorGraph_->view().axes().setTitle(1, "R-Factor");
    numberFormat = rFactorGraph_->view().axes().numberFormat(1);
    numberFormat.setType(NumberFormat::ScientificFormat);
    rFactorGraph_->view().axes().setNumberFormat(1, numberFormat);
    rFactorGraph_->view().axes().setMin(1, 0.0);
    rFactorGraph_->view().axes().setMax(1, 0.5);
    rFactorGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    rFactorGraph_->groupManager().setGroupColouring("RFactor", RenderableGroup::AutomaticIndividualColouring);

    // Debug Tab - EP Functions Graph
    DataViewer *epView = ui_.DebugEPFunctionsPlotWidget->dataViewer();
    // -- Set view
    epView->view().setViewType(View::FlatXYView);
    epView->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    epView->view().axes().setMax(0, 10.0);
    epView->view().axes().setTitle(1, "\\sym{phi}(\\it{r})");
    epView->view().axes().setMin(1, -1.0);
    epView->view().axes().setMax(1, 1.0);
    epView->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

EPSRModuleWidget::~EPSRModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void EPSRModuleWidget::updateControls(int flags)
{
    refreshing_ = true;

    ui_.FQPlotWidget->updateToolbar();
    ui_.FQFitPlotWidget->updateToolbar();
    ui_.EstimatedSQPlotWidget->updateToolbar();
    ui_.EstimatedGRPlotWidget->updateToolbar();
    ui_.TotalGRPlotWidget->updateToolbar();
    ui_.PhiRPlotWidget->updateToolbar();
    ui_.PhiMagPlotWidget->updateToolbar();
    ui_.RFactorPlotWidget->updateToolbar();

    FQGraph_->postRedisplay();
    FQFitGraph_->postRedisplay();
    estimatedSQGraph_->postRedisplay();
    estimatedGRGraph_->postRedisplay();
    totalGRGraph_->postRedisplay();
    phiRGraph_->postRedisplay();
    phiMagGraph_->postRedisplay();
    rFactorGraph_->postRedisplay();

    refreshing_ = false;
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool EPSRModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!FQGraph_->writeSession(parser))
        return false;
    if (!FQFitGraph_->writeSession(parser))
        return false;
    if (!estimatedSQGraph_->writeSession(parser))
        return false;
    if (!estimatedGRGraph_->writeSession(parser))
        return false;
    if (!totalGRGraph_->writeSession(parser))
        return false;
    if (!phiRGraph_->writeSession(parser))
        return false;
    if (!phiMagGraph_->writeSession(parser))
        return false;
    if (!rFactorGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool EPSRModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!FQGraph_->readSession(parser))
        return false;
    if (!FQFitGraph_->readSession(parser))
        return false;
    if (!estimatedSQGraph_->readSession(parser))
        return false;
    if (!estimatedGRGraph_->readSession(parser))
        return false;
    if (!totalGRGraph_->readSession(parser))
        return false;
    if (!phiRGraph_->readSession(parser))
        return false;
    if (!phiMagGraph_->readSession(parser))
        return false;
    if (!rFactorGraph_->readSession(parser))
        return false;

    return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void EPSRModuleWidget::setGraphDataTargets(EPSRModule *module)
{
    if (!module)
        return;

    // Add total R-Factor before any dataset R-Factors
    auto *rFacTot = rFactorGraph_->createRenderable(Renderable::Data1DRenderable,
                                                    fmt::format("{}//RFactor", module->uniqueName()), "Total", "Total");
    rFacTot->lineStyle().setStipple(LineStipple::HalfDashStipple);

    // Add reference data & calculated data to the FQGraph_, and percentage errors to the rFactorGraph_
    for (auto *targetModule : module->targets())
    {
        // Reference data
        FQGraph_->createRenderable(Renderable::Data1DRenderable, fmt::format("{}//ReferenceData", targetModule->uniqueName()),
                                   fmt::format("{} (Exp)", targetModule->uniqueName()), "Exp");

        // Calculated data from associated module
        if (DissolveSys::sameString(targetModule->type(), "NeutronSQ"))
        {
            // F(Q)
            FQGraph_->createRenderable(Renderable::Data1DRenderable,
                                       fmt::format("{}//WeightedSQ//Total", targetModule->uniqueName()),
                                       fmt::format("{} (Calc)", targetModule->uniqueName()), "Calc");

            // F(Q) diff w.r.t. reference
            FQGraph_->createRenderable(Renderable::Data1DRenderable,
                                       fmt::format("{}//Difference//{}", module->uniqueName(), targetModule->uniqueName()),
                                       fmt::format("{} (Delta)", targetModule->uniqueName()), "Delta");

            // R-Factor between F(Q) and reference
            rFactorGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//RFactor//{}", module->uniqueName(), targetModule->uniqueName()),
                                            targetModule->uniqueName(), "RFactor");

            // Reference F(r) (from direct FT of input data)
            totalGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//ReferenceDataFT", targetModule->uniqueName()),
                                            fmt::format("{} (Exp)", targetModule->uniqueName()), "Exp");

            // Simulated F(r) (from FT of the calculated F(Q))
            totalGRGraph_->createRenderable(
                Renderable::Data1DRenderable,
                fmt::format("{}//SimulatedFR//{}", module->uniqueName(), targetModule->uniqueName()),
                fmt::format("{} (Calc)", targetModule->uniqueName()), "Calc");
        }

        // Delta F(Q) and fit
        FQFitGraph_->createRenderable(Renderable::Data1DRenderable,
                                      fmt::format("{}//DeltaFQ//{}", module->uniqueName(), targetModule->uniqueName()),
                                      fmt::format("{} (Delta)", targetModule->uniqueName()), "Delta");

        FQFitGraph_->createRenderable(Renderable::Data1DRenderable,
                                      fmt::format("{}//DeltaFQFit//{}", module->uniqueName(), targetModule->uniqueName()),
                                      fmt::format("{} (Fit)", targetModule->uniqueName()), "Fit");
    }

    // Get (first) source RDF module for the partial data
    // Retrieve source SQ module, and then the related RDF module
    const RDFModule *rdfModule = nullptr;
    const auto &targets = module->keywords().retrieve<std::vector<Module *>>("Targets");
    if (!targets.empty())
    {
        const SQModule *sqModule = targets[0]->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            Messenger::error(
                "Couldn't get any S(Q) data from the first target module, so underlying partial g(r) will be unavailable.",
                module->uniqueName());
        else
            rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
        if (!rdfModule)
            Messenger::error(
                "First target's S(Q) module doesn't reference an RDFModule, so underlying partial g(r) will be unavailable.");
    }

    // Add experimentally-determined partial S(Q) and g(r) to the estimatedSQGraph_ and estimatedGRGraph_
    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        /*
         * Partial Structure Factors
         */

        // Unweighted estimated partial
        estimatedSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//EstimatedSQ//{}", module_->uniqueName(), id),
                                            fmt::format("{} (Estimated)", id), "Estimated");

        // Calculated / summed partial
        estimatedSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//UnweightedSQ//{}", module_->uniqueName(), id),
                                            fmt::format("{} (Calc)", id), "Calc");

        // Deltas
        estimatedSQGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//DeltaSQ//{}", module_->uniqueName(), id),
                                            fmt::format("{} (Delta)", id), "Delta");

        /*
         * Partial RDFs
         */

        // Experimentally-determined unweighted partial
        estimatedGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                            fmt::format("{}//EstimatedGR//{}", module_->uniqueName(), id),
                                            fmt::format("{} (Estimated)", id), "Estimated");

        // Calculated / summed partials, taken from the RDF module referenced by the first module target
        if (rdfModule)
            estimatedGRGraph_->createRenderable(Renderable::Data1DRenderable,
                                                fmt::format("{}//UnweightedGR//{}//Full", rdfModule->uniqueName(), id),
                                                fmt::format("{} (Calc)", id), "Calc");
    });

    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        // Generated potential
        phiRGraph_->createRenderable(Renderable::Data1DRenderable, fmt::format("PairPotential//{}//Additional", id), id, "Phi");
    });

    // Add phi magnitude data
    phiMagGraph_->createRenderable(Renderable::Data1DRenderable, fmt::format("{}//EPMag", module_->uniqueName()), "EReq",
                                   "EReq");
}

/*
 * Debug Tab
 */

// Update data shown on fit coefficients viewer
void EPSRModuleWidget::updateDebugEPFunctionsGraph(int from, int to)
{
    DataViewer *viewer = ui_.DebugEPFunctionsPlotWidget->dataViewer();
    viewer->clearRenderables();
    debugFunctionData_.clear();

    if (!module_)
        return;

    for_each_pair(dissolve_.atomTypes().begin(), dissolve_.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        const std::string id = fmt::format("{}-{}", at1->name(), at2->name());

        // Generate data for function range specified
        for (auto n = from; n <= to; ++n)
        {
            auto *data = debugFunctionData_.add();
            (*data) = module_->generateEmpiricalPotentialFunction(dissolve_, i, j, n);
            data->setObjectTag(fmt::format("PairPotential//{}//Function//{}", id, n));
            auto *rend =
                viewer->createRenderable(Renderable::Data1DRenderable, fmt::format("PairPotential//{}//Function//{}", id, n),
                                         fmt::format("{}/{}", id, n), id);
            rend->setColour(StockColours::RedStockColour);
        }

        // Add generate potential to graph
        viewer->createRenderable(Renderable::Data1DRenderable, fmt::format("PairPotential//{}//Additional", id), id, id);
    });
}

void EPSRModuleWidget::on_DebugFromSpin_valueChanged(int value)
{
    updateDebugEPFunctionsGraph(value, ui_.DebugToSpin->value());
}

void EPSRModuleWidget::on_DebugToSpin_valueChanged(int value)
{
    updateDebugEPFunctionsGraph(ui_.DebugFromSpin->value(), value);
}
