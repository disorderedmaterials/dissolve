/*
    *** Energy Module Widget - Functions
    *** src/modules/energy/gui/modulewidget_funcs.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/atomtype.h"
#include "genericitems/listhelper.h"
#include "gui/dataviewer.hui"
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
            GenericListHelper<Data1D>::value(currentConfiguration_->moduleData(), "Total", module_->uniqueName(), Data1D());
        if (totalEnergyArray.nValues() < stabilityWindow)
            ui_.GradientValueLabel->setText("N/A");
        else
        {
            auto grad = GenericListHelper<double>::value(currentConfiguration_->moduleData(), "EnergyGradient", "", 0.0);
            ui_.GradientValueLabel->setText(QString::number(grad));
        }

        auto stable = GenericListHelper<bool>::value(currentConfiguration_->moduleData(), "EnergyStable", "", false);

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
 * State I/O
 */

// Write widget state through specified LineParser
bool EnergyModuleWidget::writeState(LineParser &parser) const
{
    // Write DataViewer sessions
    if (!energyGraph_->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool EnergyModuleWidget::readState(LineParser &parser)
{
    // Read DataViewer sessions
    if (!energyGraph_->readSession(parser))
        return false;

    return true;
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
    Renderable *rend;
    energyGraph_->createRenderable(Renderable::Data1DRenderable,
                                   fmt::format("{}//{}//Total", currentConfiguration_->niceName(), module_->uniqueName()),
                                   "Total", "Totals");
    rend = energyGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//{}//Inter", currentConfiguration_->niceName(), module_->uniqueName()),
        "Inter", "Totals");
    rend->setColour(StockColours::RedStockColour);
    rend = energyGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//{}//Intra", currentConfiguration_->niceName(), module_->uniqueName()),
        "Intra", "Totals");
    rend->setColour(StockColours::BlueStockColour);

    rend = energyGraph_->createRenderable(Renderable::Data1DRenderable,
                                          fmt::format("{}//{}//Bond", currentConfiguration_->niceName(), module_->uniqueName()),
                                          "Bond", "Intramolecular");
    rend->setColour(StockColours::GreenStockColour);
    rend = energyGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//{}//Angle", currentConfiguration_->niceName(), module_->uniqueName()),
        "Angle", "Intramolecular");
    rend->setColour(StockColours::PurpleStockColour);
    rend = energyGraph_->createRenderable(
        Renderable::Data1DRenderable, fmt::format("{}//{}//Torsion", currentConfiguration_->niceName(), module_->uniqueName()),
        "Torsion", "Intramolecular");
    rend->setColour(StockColours::OrangeStockColour);

    updateControls();
}
