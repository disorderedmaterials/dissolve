// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/kVector.h"
#include "gui/dataViewer.hui"
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
        reflectionAtomTypesData_ =
            dissolve_.processingModuleData().valueIf<const AtomTypeMix>("SummedAtomTypes", module_->name());

    // Need to recreate renderables if requested as the updateType
    if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag))
    {
        ui_.PlotWidget->clearRenderableData();

        if (ui_.TotalsButton->isChecked())
        {
            graph_->createRenderable<RenderableData1D>(fmt::format("{}//OriginalBragg//Total", module_->name()), "Total",
                                                       "Totals");
        }
        else if (ui_.PartialsButton->isChecked())
        {
            if (reflectionAtomTypesData_)
            {
                PairIterator pairs(reflectionAtomTypesData_->get().nItems());
                for (auto [first, second] : pairs)
                {
                    const AtomTypeData &at1 = reflectionAtomTypesData_->get()[first];
                    const AtomTypeData &at2 = reflectionAtomTypesData_->get()[second];
                    const std::string id = fmt::format("{}-{}", at1.atomTypeName(), at2.atomTypeName());

                    graph_->createRenderable<RenderableData1D>(fmt::format("{}//OriginalBragg//{}", module_->name(), id),
                                                               fmt::format("{}", id), "Full");
                };
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
                const auto &atl = reflectionAtomTypesData_->get();
                std::vector<std::string> columnHeaders;
                columnHeaders.reserve(atl.nItems() * (atl.nItems() + 1) / 2);
                for (auto [first, second] : PairIterator(atl.nItems()))
                    columnHeaders.emplace_back(fmt::format("{}-{}", atl[first].atomTypeName(), atl[second].atomTypeName()));
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
