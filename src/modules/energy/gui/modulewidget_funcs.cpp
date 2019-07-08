/*
	*** Energy Module Widget - Functions
	*** src/modules/energy/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/energy/gui/modulewidget.h"
#include "gui/viewer/dataviewer.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "genericitems/listhelper.h"

// Constructor
EnergyModuleWidget::EnergyModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_(dynamic_cast<EnergyModule*>(module)), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our DataViewer widget
	energyGraph_ = ui.PlotWidget->dataViewer();

	// Start a new, empty session
	View& view = energyGraph_->view();
	view.setViewType(View::FlatXYView);
	view.axes().setTitle(0, "Iteration");
	view.axes().setMax(0, 10.0);
	view.axes().numberFormat(0).setNDecimals(0);
	view.axes().setTitle(1, "Energy, kJ mol\\sup{-1}");
	view.axes().setMin(1, -1.0);
	view.axes().setMax(1, 1.0);
	view.axes().numberFormat(1).setType(NumberFormat::ScientificFormat);
	view.axes().numberFormat(1).setUseENotation(true);
	view.setAutoFollowType(View::XAutoFollow);

	currentConfiguration_ = NULL;

	setGraphDataTargets(module_);

	updateControls();

	refreshing_ = false;
}

EnergyModuleWidget::~EnergyModuleWidget()
{
}

// Update controls within widget
void EnergyModuleWidget::updateControls()
{
	// Set gradient and stability labels
	int stabilityWindow = module_->keywords().asInt("StabilityWindow");
	ui.GradientInfoLabel->setText(QString("Gradient (last %1 points) : ").arg(stabilityWindow));

	QPalette labelPalette = ui.StableLabel->palette();
	if (currentConfiguration_)
	{
		const Data1D& totalEnergyArray = GenericListHelper<Data1D>::value(currentConfiguration_->moduleData(), "Total", module_->uniqueName(), Data1D());
		if (totalEnergyArray.nValues() < stabilityWindow) ui.GradientValueLabel->setText("N/A");
		else
		{
			double grad = GenericListHelper<double>::value(currentConfiguration_->moduleData(), "EnergyGradient", "", 0.0);
			ui.GradientValueLabel->setText(QString::number(grad));
		}

		bool stable = GenericListHelper<bool>::value(currentConfiguration_->moduleData(), "EnergyStable", "", false);

		if (stable)
		{
			labelPalette.setColor(QPalette::WindowText, Qt::darkGreen);
			ui.StableLabel->setText("Yes");
		}
		else
		{
			labelPalette.setColor(QPalette::WindowText, Qt::red);
			ui.StableLabel->setText("No");
		}
	}
	else
	{
		ui.GradientValueLabel->setText("N/A");
		labelPalette.setColor(QPalette::WindowText, Qt::red);
		ui.StableLabel->setText("No");
	}
	ui.StableLabel->setPalette(labelPalette);

	ui.PlotWidget->updateToolbar();

	energyGraph_->postRedisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void EnergyModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void EnergyModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool EnergyModuleWidget::writeState(LineParser& parser)
{
	// Write DataViewer sessions
	if (!energyGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool EnergyModuleWidget::readState(LineParser& parser)
{
	// Read DataViewer sessions
	if (!energyGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void EnergyModuleWidget::setGraphDataTargets(EnergyModule* module)
{
	if (!module) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));
}

void EnergyModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current data
	energyGraph_->clearRenderables();

	// Get target Configuration
	currentConfiguration_ = VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
	if (!currentConfiguration_) return;

	// Add data targets
	Renderable* rend;
	energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Total", currentConfiguration_->niceName(), module_->uniqueName()), "Total", "Totals");
	rend = energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Inter", currentConfiguration_->niceName(), module_->uniqueName()), "Inter", "Totals");
	rend->setColour(ColourDefinition::RedStockColour);
	rend = energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Intra", currentConfiguration_->niceName(), module_->uniqueName()), "Intra", "Totals");
	rend->setColour(ColourDefinition::BlueStockColour);

	rend = energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Bond", currentConfiguration_->niceName(), module_->uniqueName()), "Bond", "Intramolecular");
	rend->setColour(ColourDefinition::GreenStockColour);
	rend = energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Angle", currentConfiguration_->niceName(), module_->uniqueName()), "Angle", "Intramolecular");
	rend->setColour(ColourDefinition::PurpleStockColour);
	rend = energyGraph_->createRenderable(Renderable::Data1DRenderable, CharString("Data1D@%s//%s//Torsion", currentConfiguration_->niceName(), module_->uniqueName()), "Torsion", "Intramolecular");
	rend->setColour(ColourDefinition::OrangeStockColour);

	updateControls();
}
