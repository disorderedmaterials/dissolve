// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/dataviewer.hui"
#include "gui/render/renderabledata1d.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/energy/gui/modulewidget.h"
#include "templates/variantpointer.h"

EnergyModuleWidget::EnergyModuleWidget(QWidget *parent, const GenericList &processingData, EnergyModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    NumberFormat numberFormat;

    // Grab our DataViewer widget
    energyGraph_ = ui_.PlotWidget->dataViewer();

    // Start a new, empty session
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

    currentConfiguration_ = nullptr;

    setGraphDataTargets(module_);

    updateControls(ModuleWidget::UpdateType::Normal);

    refreshing_ = false;
}

EnergyModuleWidget::~EnergyModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void EnergyModuleWidget::updateControls(ModuleWidget::UpdateType updateType)
{
    // Set gradient and stability labels
    auto stabilityWindow = module_->keywords().asInt("StabilityWindow");
    ui_.GradientInfoLabel->setText(QString("Gradient (last %1 points) : ").arg(stabilityWindow));

    QPalette labelPalette = ui_.StableLabel->palette();
    if (currentConfiguration_)
    {
        if (processingData_.contains(fmt::format("{}//EnergyGradient", currentConfiguration_->niceName()),
                                     module_->uniqueName()))
            ui_.GradientValueLabel->setText(QString::number(processingData_.value<double>(
                fmt::format("{}//EnergyGradient", currentConfiguration_->niceName()), module_->uniqueName())));
        else
            ui_.GradientValueLabel->setText("N/A");

        if (processingData_.valueOr<bool>(fmt::format("{}//EnergyStable", currentConfiguration_->niceName()),
                                          module_->uniqueName(), false))
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

// Set data targets in graphs
void EnergyModuleWidget::setGraphDataTargets(EnergyModule *module)
{
    if (!module)
        return;

    // Add Configuration targets to the combo box
    ui_.TargetCombo->clear();
    for (Configuration *config : module->targetConfigurations())
        ui_.TargetCombo->addItem(QString::fromStdString(std::string(config->name())), VariantPointer<Configuration>(config));
}

void EnergyModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
    // Remove any current data
    energyGraph_->clearRenderables();

    // Get target Configuration
    currentConfiguration_ = VariantPointer<Configuration>(ui_.TargetCombo->itemData(index));
    if (!currentConfiguration_)
        return;

    // Add data targets
    energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Total", module_->uniqueName(), currentConfiguration_->niceName()), "Total", "Totals");
    auto inter = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Inter", module_->uniqueName(), currentConfiguration_->niceName()), "Inter", "Totals");
    inter->setColour(StockColours::RedStockColour);
    auto intra = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Intra", module_->uniqueName(), currentConfiguration_->niceName()), "Intra", "Totals");
    intra->setColour(StockColours::BlueStockColour);

    auto bond = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Bond", module_->uniqueName(), currentConfiguration_->niceName()), "Bond", "Intramolecular");
    bond->setColour(StockColours::GreenStockColour);
    auto angle = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Angle", module_->uniqueName(), currentConfiguration_->niceName()), "Angle", "Intramolecular");
    angle->setColour(StockColours::PurpleStockColour);
    auto torsion = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Torsion", module_->uniqueName(), currentConfiguration_->niceName()), "Torsion", "Intramolecular");
    torsion->setColour(StockColours::OrangeStockColour);
    auto improper = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Improper", module_->uniqueName(), currentConfiguration_->niceName()), "Improper",
        "Intramolecular");
    improper->setColour(StockColours::CyanStockColour);

    updateControls(ModuleWidget::UpdateType::Normal);
}
