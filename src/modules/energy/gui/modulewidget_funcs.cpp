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

EnergyModuleWidget::EnergyModuleWidget(QWidget *parent, EnergyModule *module) : ModuleWidget(parent), module_(module)
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
    numberFormat.setType(NumberFormat::ScientificFormat);
    numberFormat.setUseENotation(true);
    view.axes().setNumberFormat(1, numberFormat);
    view.setAutoFollowType(View::XAutoFollow);

    currentConfiguration_ = nullptr;

    setGraphDataTargets(module_);

    updateControls();

    refreshing_ = false;
}

EnergyModuleWidget::~EnergyModuleWidget() {}

/*
 * UI
 */

// Update controls within widget
void EnergyModuleWidget::updateControls(int flags)
{
    // Set gradient and stability labels
    auto stabilityWindow = module_->keywords().asInt("StabilityWindow");
    ui_.GradientInfoLabel->setText(QString("Gradient (last %1 points) : ").arg(stabilityWindow));

    QPalette labelPalette = ui_.StableLabel->palette();
    if (currentConfiguration_)
    {
        const auto &totalEnergyArray =
            currentConfiguration_->moduleData().value<Data1D>("Total", module_->uniqueName(), Data1D());
        if (totalEnergyArray.nValues() < stabilityWindow)
            ui_.GradientValueLabel->setText("N/A");
        else
        {
            auto grad = currentConfiguration_->moduleData().value<double>("EnergyGradient", "", 0.0);
            ui_.GradientValueLabel->setText(QString::number(grad));
        }

        auto stable = currentConfiguration_->moduleData().value<bool>("EnergyStable", "", false);

        if (stable)
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
        fmt::format("{}//{}//Total", currentConfiguration_->niceName(), module_->uniqueName()), "Total", "Totals");
    auto inter = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Inter", currentConfiguration_->niceName(), module_->uniqueName()), "Inter", "Totals");
    inter->setColour(StockColours::RedStockColour);
    auto intra = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Intra", currentConfiguration_->niceName(), module_->uniqueName()), "Intra", "Totals");
    intra->setColour(StockColours::BlueStockColour);

    auto bond = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Bond", currentConfiguration_->niceName(), module_->uniqueName()), "Bond", "Intramolecular");
    bond->setColour(StockColours::GreenStockColour);
    auto angle = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Angle", currentConfiguration_->niceName(), module_->uniqueName()), "Angle", "Intramolecular");
    angle->setColour(StockColours::PurpleStockColour);
    auto torsion = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Torsion", currentConfiguration_->niceName(), module_->uniqueName()), "Torsion", "Intramolecular");
    torsion->setColour(StockColours::OrangeStockColour);
    auto improper = energyGraph_->createRenderable<RenderableData1D>(
        fmt::format("{}//{}//Improper", currentConfiguration_->niceName(), module_->uniqueName()), "Improper",
        "Intramolecular");
    improper->setColour(StockColours::CyanStockColour);

    updateControls();
}
