// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/helpers/comboboxcontroller.h"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/modulewidget.h"

Q_DECLARE_METATYPE(Configuration *);

EnergyModuleWidget::EnergyModuleWidget(QWidget *parent, const GenericList &processingData, EnergyModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    // Grab our DataViewer widget
    NumberFormat numberFormat;
    energyGraph_ = ui_.PlotWidget->dataViewer();
    auto &view = energyGraph_->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "Iteration");
    view.axes().setMax(0, 10.0);
    numberFormat = view.axes().numberFormat(0);
    numberFormat.setNDecimals(0);
    view.axes().setNumberFormat(0, numberFormat);
    view.axes().setTitle(1, "Energy, kJ mol\\sup{-1}");
    view.axes().setMin(1, -1.0);
    view.axes().setMax(1, 1.0);
    numberFormat = view.axes().numberFormat(1);
    numberFormat.setType(NumberFormat::FormatType::Scientific);
    numberFormat.setUseENotation(true);
    view.axes().setNumberFormat(1, numberFormat);
    view.setAutoFollowType(View::XAutoFollow);

    refreshing_ = false;
}

EnergyModuleWidget::~EnergyModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void EnergyModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    // Update partial set (Configuration) targets
    auto optConfig = combo_box_updater(ui_.ConfigurationTargetCombo, module_->targetConfigurations().begin(),
                                       module_->targetConfigurations().end(), [](auto item) { return item.item()->name(); });

    // Set gradient and stability labels
    auto stabilityWindow = module_->keywords().asInt("StabilityWindow");
    ui_.GradientInfoLabel->setText(QString("Gradient (last %1 points) : ").arg(stabilityWindow));

    // Create / update renderables?
    if (optConfig == module_->targetConfigurations().end())
        energyGraph_->clearRenderables();
    else if (updateType == ModuleWidget::UpdateType::RecreateRenderables || energyGraph_->renderables().empty())
    {
        // Clear any existing renderables
        energyGraph_->clearRenderables();

        auto cfg = *optConfig;
        auto prefix = fmt::format("{}//{}", module_->uniqueName(), cfg->niceName());
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Total", prefix), "Total", "Totals");
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Inter", prefix), "Inter", "Totals")
            ->setColour(StockColours::RedStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Intra", prefix), "Intra", "Totals")
            ->setColour(StockColours::BlueStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Bond", prefix), "Bond", "Intramolecular")
            ->setColour(StockColours::GreenStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Angle", prefix), "Angle", "Intramolecular")
            ->setColour(StockColours::PurpleStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Torsion", prefix), "Torsion", "Intramolecular")
            ->setColour(StockColours::OrangeStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Improper", prefix), "Improper", "Intramolecular")
            ->setColour(StockColours::CyanStockColour);
    }

    // Validate renderables if they need it
    energyGraph_->validateRenderables(processingData_);

    // Update labels
    QPalette labelPalette = ui_.StableLabel->palette();
    if (optConfig != module_->targetConfigurations().end())
    {
        auto cfg = *optConfig;

        if (processingData_.contains(fmt::format("{}//EnergyGradient", cfg->niceName()), module_->uniqueName()))
            ui_.GradientValueLabel->setText(QString::number(
                processingData_.value<double>(fmt::format("{}//EnergyGradient", cfg->niceName()), module_->uniqueName())));
        else
            ui_.GradientValueLabel->setText("N/A");

        if (processingData_.valueOr<bool>(fmt::format("{}//EnergyStable", cfg->niceName()), module_->uniqueName(), false))
        {
            labelPalette.setColor(QPalette::WindowText, Qt::darkGreen);
            ui_.StableLabel->setText("Yes");
        }
        else
        {
            labelPalette.setColor(QPalette::WindowText, Qt::red);
            ui_.StableLabel->setText("No");
        }
    }
    else
    {
        ui_.GradientValueLabel->setText("N/A");
        labelPalette.setColor(QPalette::WindowText, Qt::red);
        ui_.StableLabel->setText("No");
    }
    ui_.StableLabel->setPalette(labelPalette);

    ui_.PlotWidget->updateToolbar();

    energyGraph_->postRedisplay();
}

/*
 * Widgets / Functions
 */

void EnergyModuleWidget::on_ConfigurationTargetCombo_currentIndexChanged(int index)
{
    updateControls(ModuleWidget::UpdateType::RecreateRenderables);
}
