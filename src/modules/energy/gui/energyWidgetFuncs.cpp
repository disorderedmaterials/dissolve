// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/energyWidget.h"

Q_DECLARE_METATYPE(Configuration *);

EnergyModuleWidget::EnergyModuleWidget(QWidget *parent, EnergyModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
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

/*
 * UI
 */

// Update controls within widget
void EnergyModuleWidget::updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags)
{
    const auto *cfg = module_->keywords().getConfiguration("Configuration");

    // Set gradient and stability labels
    auto stabilityWindow = module_->keywords().getInt("StabilityWindow");
    ui_.GradientInfoLabel->setText(QString("Gradient (last %1 points) : ").arg(stabilityWindow));

    // Create / update renderables?
    if (!cfg)
        energyGraph_->clearRenderables();
    else if (updateFlags.isSet(ModuleWidget::RecreateRenderablesFlag) || energyGraph_->renderables().empty())
    {
        // Clear any existing renderables
        energyGraph_->clearRenderables();

        auto prefix = fmt::format("{}//{}", module_->name(), cfg->niceName());
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Total", prefix), "Total", "Totals");
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//PairPotential", prefix), "PairPotential", "Totals")
            ->setColour(StockColours::RedStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Bound", prefix), "Bound", "Totals")
            ->setColour(StockColours::BlueStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Cohesive", prefix), "CohesivePP", "PairPotential")
            ->setColour(StockColours::RedStockColour);
        energyGraph_
            ->createRenderable<RenderableData1D>(fmt::format("{}//IntraPP", prefix), "IntramolecularPP", "PairPotential")
            ->setColour(StockColours::BlueStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Bond", prefix), "Bond", "Bound")
            ->setColour(StockColours::GreenStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Angle", prefix), "Angle", "Bound")
            ->setColour(StockColours::PurpleStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Torsion", prefix), "Torsion", "Bound")
            ->setColour(StockColours::OrangeStockColour);
        energyGraph_->createRenderable<RenderableData1D>(fmt::format("{}//Improper", prefix), "Improper", "Bound")
            ->setColour(StockColours::CyanStockColour);
        energyGraph_->groupManager().setGroupStipple("PairPotential", LineStipple::DotStipple);
    }

    // Validate renderables if they need it
    energyGraph_->validateRenderables(dissolve_.processingModuleData());

    // Update labels
    QPalette labelPalette = ui_.StableLabel->palette();
    if (cfg)
    {
        if (dissolve_.processingModuleData().contains("EnergyGradient", cfg->niceName()))
            ui_.GradientValueLabel->setText(
                QString::number(dissolve_.processingModuleData().value<double>("EnergyGradient", cfg->niceName())));
        else
            ui_.GradientValueLabel->setText("N/A");

        if (dissolve_.processingModuleData().valueOr<bool>("EnergyStable", cfg->niceName(), false))
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
