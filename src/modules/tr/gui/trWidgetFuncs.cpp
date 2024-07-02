// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopeData.h"
#include "gui/dataViewer.h"
#include "gui/helpers/comboBoxController.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/tr/gui/trWidget.h"
#include "modules/tr/tr.h"
#include "templates/algorithms.h"

TRModuleWidget::TRModuleWidget(QWidget *parent, TRModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Set up RDF graph
    trGraph_ = ui_.TRPlotWidget->dataViewer();
    // -- Set view
    trGraph_->view().setViewType(View::FlatXYView);
    trGraph_->view().axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
    trGraph_->view().axes().setMax(0, 10.0);
    trGraph_->view().axes().setTitle(1, "T(r)");
    trGraph_->view().axes().setMin(1, -1.0);
    trGraph_->view().axes().setMax(1, 1.0);
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);
    trGraph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    trGraph_->groupManager().setGroupColouring("Total", RenderableGroup::AutomaticIndividualColouring);
    trGraph_->groupManager().setGroupVerticalShifting("Total", RenderableGroup::IndividualVerticalShifting);

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
        trGraph_->createRenderable<RenderableData1D>(fmt::format("{}//{}//{}//Full", module_->name(), targetPrefix, id),
                                                     fmt::format("{} (Full)", id), "Full");
    }
}

// Update controls within widget
void TRModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Need to recreate renderables if requested as the updateType, or if we previously had no target PartialSet and have just
    // located it
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || (!ui_.TotalButton->isChecked() && !targetPartials_))
    {
        ui_.TRPlotWidget->clearRenderableData();

        if (ui_.TotalButton->isChecked())
        {
            trGraph_->createRenderable<RenderableData1D>(fmt::format("{}//WeightedTR//Total", module_->name()), "Total T(R)",
                                                         "Calculated");
        }
        else if (ui_.PartialsButton->isChecked())
        {
            targetPartials_ = dissolve_.processingModuleData().valueIf<PartialSet>("WeightedTR");
            createPartialSetRenderables("WeightedTR");
        }
    }

    // Validate renderables if they need it
    trGraph_->validateRenderables(dissolve_.processingModuleData());

    trGraph_->postRedisplay();
    ui_.TRPlotWidget->updateToolbar();

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void TRModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    trGraph_->view().axes().setTitle(1, "t(r)");
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::TwoVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_TotalButton_clicked(bool checked)
{
    if (!checked)
        return;

    trGraph_->view().axes().setTitle(1, "T(r)");
    trGraph_->groupManager().setVerticalShiftAmount(RenderableGroupManager::OneVerticalShift);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void TRModuleWidget::on_FilterEdit_textChanged(QString text) { updateControls(ModuleWidget::RecreateRenderablesFlag); }

void TRModuleWidget::on_ClearFilterButton_clicked(bool checked)
{
    ui_.FilterEdit->setText("");
    updateControls(ModuleWidget::RecreateRenderablesFlag);
}
