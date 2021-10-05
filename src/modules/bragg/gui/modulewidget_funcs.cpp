// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/bragg/bragg.h"
#include "modules/bragg/gui/modulewidget.h"
#include "templates/algorithms.h"
#include "templates/variantpointer.h"

BraggModuleWidget::BraggModuleWidget(QWidget *parent, const GenericList &processingData, BraggModule *module)
    : ModuleWidget(parent, processingData), module_(module)
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
void BraggModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    refreshing_ = true;

    // Check / update summed atom types data
    if (!reflectionAtomTypesData_)
        reflectionAtomTypesData_ = processingData_.valueIf<const AtomTypeList>("SummedAtomTypes", module_->uniqueName());

    // Need to recreate renderables if requested as the updateType
    if (updateType == ModuleWidget::UpdateType::RecreateRenderables)
    {
        ui_.PlotWidget->clearRenderableData();

        if (ui_.TotalsButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//OriginalBragg//Total", module_->uniqueName()), "Total",
                                                       "Totals");
        }
        else if (ui_.PartialsButton->isChecked())
        {
            if (reflectionAtomTypesData_)
            {
                PairIterator pairs(reflectionAtomTypesData_->get().nItems());
                std::for_each(pairs.begin(), pairs.end(), [this](auto it) {
                    const auto &at1 = this->reflectionAtomTypesData_->get()[std::get<0>(it)];
                    const auto &at2 = this->reflectionAtomTypesData_->get()[std::get<1>(it)];
                    const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

                    this->graph_->createRenderable<RenderableData1D>(
                        fmt::format("{}//OriginalBragg//{}", module_->uniqueName(), id), fmt::format("{}", id), "Full");
                });
            }
        }
    }

    // Validate renderables if they need it
    graph_->validateRenderables(processingData_);
    graph_->postRedisplay();
    ui_.PlotWidget->updateToolbar();

    // Reflections table
    if (ui_.ReflectionsButton->isChecked())
    {
        auto optReflxns = processingData_.valueIf<const std::vector<BraggReflection>>("Reflections", module_->uniqueName());
        if (!optReflxns)
        {
            reflectionData_ = std::nullopt;
            reflectionDataDisplayVersion_ = -1;
            braggModel_.setReflections(std::nullopt);
        }
        else if (!reflectionData_ || (&reflectionData_->get() != &optReflxns->get()) ||
                 (reflectionDataDisplayVersion_ != processingData_.version("Reflections", module_->uniqueName())))
        {
            braggModel_.setReflections(optReflxns);
            reflectionDataDisplayVersion_ = processingData_.version("Reflections", module_->uniqueName());

            // Retrieve the atom types list so we know which reflections correspond to which pairs
            if (reflectionAtomTypesData_)
            {
                const auto &atl = reflectionAtomTypesData_->get();
                std::vector<std::string> columnHeaders;
                columnHeaders.reserve(atl.nItems() * (atl.nItems() + 1) / 2);
                dissolve::for_each_pair(
                    ParallelPolicies::seq, atl.begin(), atl.end(),
                    [&columnHeaders](int typeI, const AtomTypeData &atd1, int typeJ, const AtomTypeData &atd2) {
                        columnHeaders.emplace_back(fmt::format("{}-{}", atd1.atomTypeName(), atd2.atomTypeName()));
                    });
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

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void BraggModuleWidget::on_TotalsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.Stack->setCurrentIndex(0);

    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}

void BraggModuleWidget::on_ReflectionsButton_clicked(bool checked)
{
    if (!checked)
        return;

    ui_.Stack->setCurrentIndex(1);

    updateControls(ModuleWidget::UpdateType::Normal);
}

void BraggModuleWidget::on_HideSmallIntensitiesCheck_clicked(bool checked) { braggFilterProxy_.setEnabled(checked); }
