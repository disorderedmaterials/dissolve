// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/kVector.h"
#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/braggWidget.h"
#include "templates/algorithms.h"

BraggModuleWidget::BraggModuleWidget(QWidget *parent, BraggModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);
    braggFilterProxy_.setSourceModel(&braggModel_);
    ui_.ReflectionsTable->setModel(&braggFilterProxy_);

    // Set up graph
    graph_ = ui_.PlotWidget->dataViewer();
    // -- Set view
    graph_->view().setViewType(View::FlatXYView);
    graph_->view().axes().setTitle(0, "\\it{Q}, \\sym{angstrom}\\sup{-1}");
    graph_->view().axes().setMax(0, 10.0);
    graph_->view().axes().setTitle(1, "Intensity");
    graph_->view().axes().setMin(1, -1.0);
    graph_->view().axes().setMax(1, 1.0);
    graph_->view().setAutoFollowType(View::AllAutoFollow);
    // -- Set group styling
    graph_->groupManager().setGroupColouring("Full", RenderableGroup::AutomaticIndividualColouring);

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

// Update controls within widget
void BraggModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    refreshing_ = true;

    // Check / update summed atom types data
    if (!reflectionAtomTypesData_)
    {
        auto *targetConfiguration = module_->keywords().getConfiguration("Configuration");
        if (targetConfiguration)
            reflectionAtomTypesData_ = targetConfiguration->atomTypePopulations();
    }

    // Need to recreate renderables if requested as the updateType
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        ui_.PlotWidget->clearRenderableData();

        if (ui_.TotalsButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(std::format("{}//OriginalBragg//Total", module_->name()), "Total",
                                                       "Totals");
        }
        else if (ui_.PartialsButton->isChecked())
        {
            if (reflectionAtomTypesData_)
            {
                dissolve::for_each_pair(ParallelPolicies::seq, *reflectionAtomTypesData_,
                                        [&](int i, auto &popI, int j, auto &popJ)
                                        {
                                            const std::string id = std::format("{}-{}", popI.first->name(), popJ.first->name());

                                            graph_->createRenderable<RenderableData1D>(
                                                std::format("{}//OriginalBragg//{}", module_->name(), id),
                                                std::format("{}", id), "Full");
                                        });
            }
        }
    }

    // Validate renderables if they need it
    graph_->validateRenderables(dissolve_.processingModuleData());
    graph_->postRedisplay();
    ui_.PlotWidget->updateToolbar();

    // Reflections table
    if (ui_.ReflectionsButton->isChecked())
    {
        auto optReflxns =
            dissolve_.processingModuleData().valueIf<const std::vector<BraggReflection>>("Reflections", module_->name());
        if (!optReflxns)
        {
            reflectionData_ = std::nullopt;
            reflectionDataDisplayVersion_ = -1;
            braggModel_.setReflections(std::nullopt);
        }
        else if (!reflectionData_ || (&reflectionData_->get() != &optReflxns->get()) ||
                 (reflectionDataDisplayVersion_ != dissolve_.processingModuleData().version("Reflections", module_->name())))
        {
            braggModel_.setReflections(optReflxns);
            reflectionDataDisplayVersion_ = dissolve_.processingModuleData().version("Reflections", module_->name());

            // Retrieve the atom types list so we know which reflections correspond to which pairs
            if (reflectionAtomTypesData_)
            {
                const auto typeVector = *reflectionAtomTypesData_;
                std::vector<std::string> columnHeaders;
                columnHeaders.reserve(typeVector.size() * (typeVector.size() + 1) / 2);
                dissolve::for_each_pair(
                    ParallelPolicies::seq, typeVector,
                    [&](int i, auto &popI, int j, auto &popJ)
                    { columnHeaders.emplace_back(std::format("{}-{}", popI.first->name(), popJ.first->name())); });

                braggModel_.setIntensityHeaders(columnHeaders);
            }
            else
                braggModel_.setIntensityHeaders(std::vector<std::string>());
        }
    }

    refreshing_ = false;
}

/*
 * Widgets / Functions
 */

void BraggModuleWidget::on_PartialsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.Stack->setCurrentIndex(0);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void BraggModuleWidget::on_TotalsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.Stack->setCurrentIndex(0);

    updateControls(ModuleWidget::RecreateRenderablesFlag);
}

void BraggModuleWidget::on_ReflectionsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.Stack->setCurrentIndex(1);

    updateControls();
}

void BraggModuleWidget::on_HideSmallIntensitiesCheck_clicked(bool checked) { braggFilterProxy_.setEnabled(checked); }
