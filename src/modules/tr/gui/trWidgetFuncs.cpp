// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopeData.h"
#include "gui/dataViewer.h"
#include "gui/helpers/comboBoxController.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/tr/tr.h"
#include "modules/tr/gui/trWidget.h"
#include "templates/algorithms.h"

Q_DECLARE_METATYPE(Configuration *);

TRModuleWidget::TRModuleWidget(QWidget *parent, TRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
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

/*
 * UI
 */

// Create renderables for current target PartialSet
void TRModuleWidget::createPartialSetRenderables(std::string_view targetPrefix)
{
    if (!targetPartials_)
        return;

    const PartialSet &ps = *targetPartials_;

    // Get the filter text (if there is any)
    std::optional<std::string> filterText;
    if (!ui_.FilterEdit->text().isEmpty())
        filterText = ui_.FilterEdit->text().toStdString();

    PairIterator pairs(ps.atomTypeMix().nItems());
    for (auto [first, second] : pairs)
    {
        auto &at1 = ps.atomTypeMix()[first];
        auto &at2 = ps.atomTypeMix()[second];
        const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

        // Filtering - does this 'id' match our filter?
        if (filterText && id.find(filterText.value()) == std::string::npos)
            continue;

        // Full partial
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->name(), targetPrefix, id),
                                                      fmt::format("{} (Full)", id), "Full");

        // Bound partial
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Bound", module_->name(), targetPrefix, id),
                                                      fmt::format("{} (Bound)", id), "Bound");

        // Unbound partial
        rdfGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Unbound", module_->name(), targetPrefix, id),
                                                      fmt::format("{} (Unbound)", id), "Unbound");
    }
}

// Update controls within widget
void TRModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    const auto cfgs = module_->keywords().getVectorConfiguration("Configurations");

    // Update partial set (Configuration) targets
    auto optConfig =
        combo_box_updater(ui_.ConfigurationTargetCombo, cfgs.begin(), cfgs.end(), [](auto *item) { return item->name(); });

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || (!ui_.TotalsButton->isChecked() && !targetPartials_))
    {
        ui_.RDFPlotWidget->clearRenderableData();

        if (ui_.SummedPartialsButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("UnweightedGR", module_->name());
            createPartialSetRenderables("UnweightedGR");
        }
        else if (ui_.ConfigurationPartialsButton->isChecked())
        {
            auto targetPrefix = fmt::format("{}//UnweightedGR", (*optConfig)->niceName());
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>(targetPrefix, module_->name());
            createPartialSetRenderables(targetPrefix);
        }
        else
            for (auto *cfg : cfgs)
                rdfGraph_->createRenderable<RenderableData1D>(
                    fmt::format("{}//{}//UnweightedGR//Total", module_->name(), cfg->niceName()), cfg->niceName(), "Total");
    }

    // Validate renderables if they need it
    rdfGraph_->validateRenderables(dissolve_.processingModuleData());

    rdfGraph_->postRedisplay();
    ui_.RDFPlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void TRModuleWidget::on_SummedPartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(false);

    rdfGraph_->view().axes().setTitle(1, "g(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_TotalsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(false);

    rdfGraph_->view().axes().setTitle(1, "G(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_ConfigurationPartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.ConfigurationTargetCombo->setEnabled(true);

    rdfGraph_->view().axes().setTitle(1, "g(r)");
    rdfGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_ConfigurationTargetCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void TRModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->setText("");
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}
