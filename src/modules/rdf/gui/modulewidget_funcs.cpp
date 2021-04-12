// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/isotopedata.h"
#include "gui/dataviewer.hui"
#include "gui/helpers/comboboxupdater.h"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/rdf/gui/modulewidget.h"
#include "modules/rdf/rdf.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

Q_DECLARE_METATYPE(Configuration *);

RDFModuleWidget::RDFModuleWidget(QWidget *parent, const GenericList &processingData, RDFModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, processingData), module_(module), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    rdfGraph_ = ui_.RDFPlotWidget->dataViewer();
    // -- Set view
    rdfGraph_->view().setViewType(View::FlatXYView);
    rdfGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    rdfGraph_->view().axes().setMax(0, 10.0);
    rdfGraph_->view().axes().setTitle(1, "G(r)");
    rdfGraph_->view().axes().setMin(1, -1.0);
    rdfGraph_->view().axes().setMax(1, 1.0);
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    rdfGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    rdfGraph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);
    rdfGraph_->groupManager().setGroupVerticalShifting("Full", RenderableGroup::IndividualVerticalShifting);
    rdfGraph_->groupManager().setGroupColouring("Bound", RenderableGroup::AutomaticIndividualColouring);
    rdfGraph_->groupManager().setGroupVerticalShifting("Bound", RenderableGroup::IndividualVerticalShifting);
    rdfGraph_->groupManager().setGroupStipple("Bound", LineStipple::HalfDashStipple);
    rdfGraph_->groupManager().setGroupColouring("Unbound", RenderableGroup::AutomaticIndividualColouring);
    rdfGraph_->groupManager().setGroupVerticalShifting("Unbound", RenderableGroup::IndividualVerticalShifting);
    rdfGraph_->groupManager().setGroupStipple("Unbound", LineStipple::DotStipple);
    rdfGraph_->groupManager().setGroupColouring("Total", RenderableGroup::AutomaticIndividualColouring);
    rdfGraph_->groupManager().setGroupVerticalShifting("Total", RenderableGroup::IndividualVerticalShifting);

    refreshing_ = false;
}

RDFModuleWidget::~RDFModuleWidget() {}

/*
 * UI
 */

// Create renderables for current target PartialSet
void RDFModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    for_each_pair(ps.atomTypes().begin(), ps.atomTypes().end(), [&](int n, auto at1, int m, auto at2) {
        const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

        // Full partial
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->uniqueName(), targetPrefix, id),
                                                      fmt::format("{} (Full)", id), "Full");

        // Bound partial
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->uniqueName(), targetPrefix, id),
                                                      fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        rdfGraph_->createRenderable<RenderableData1D>(
            fmt::format("{}//{}//{}//Unbound", module_->uniqueName(), targetPrefix, id), fmt::format("{} (Unbound)", id),
            "Unbound");
    });
}

// Update controls within widget
void RDFModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    // Update partial set (Configuration) targets
    auto optConfig = combo_box_updater(ui_.ConfigurationTargetCombo, module_->targetConfigurations().begin(),
                                       module_->targetConfigurations().end(), [](auto item) { return item.item()->name(); });

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables || (!ui_.TotalsButton->isChecked() && !targetPartials_))
    {
        ui_.RDFPlotWidget->clearRenderableData();

        if (ui_.SummedPartialsButton->isChecked())
        {
            targetPartials_ = processingData_.valueIf<PartialSet>("UnweightedGR", module_->uniqueName());
            createPartialSetRenderables("UnweightedGR");
        }
        else if (ui_.ConfigurationPartialsButton->isChecked())
        {
            auto targetPrefix = fmt::format("{}//UnweightedGR", optConfig.item()->niceName());
            targetPartials_ = processingData_.valueIf<PartialSet>(targetPrefix, module_->uniqueName());
            createPartialSetRenderables(targetPrefix);
        }
        else
            for (auto *cfg : module_->targetConfigurations())
                rdfGraph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//{}//UnweightedGR//Total", module_->uniqueName(), cfg->niceName()), cfg->niceName(),
                    "Total");
    }

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(processingData_);

    rdfGraph_->postRedisplay();
    ui_.RDFPlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void RDFModuleWidget::on_SummedPartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(false);

    rdfGraph_->view().axes().setTitle(1, "g(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void RDFModuleWidget::on_TotalsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(false);

    rdfGraph_->view().axes().setTitle(1, "G(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void RDFModuleWidget::on_ConfigurationPartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(true);

    rdfGraph_->view().axes().setTitle(1, "g(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void RDFModuleWidget::on_ConfigurationTargetCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}
