/*
	*** Energy Module Widget - Functions
	*** src/modules/energy/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
EnergyModuleWidget::EnergyModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((EnergyModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our UChroma widget
	energyGraph_ = ui.PlotWidget;

	// Start a new, empty session
	energyGraph_->startNewSession(true);
	ViewPane* viewPane = energyGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "Iteration");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().numberFormat(0).setNDecimals(0);
	viewPane->axes().setTitle(1, "Energy, kJ mol\\sup{-1}");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->axes().numberFormat(1).setType(NumberFormat::ScientificFormat);
	viewPane->axes().numberFormat(1).setUseENotation(true);
	viewPane->setAutoFollowType(ViewPane::XFollow);

	currentConfiguration_ = NULL;

	setGraphDataTargets(module_);

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
		const XYData& totalEnergyArray = GenericListHelper<XYData>::value(currentConfiguration_->moduleData(), "Total", module_->uniqueName(), XYData());
		if (totalEnergyArray.nDataPoints() < stabilityWindow) ui.GradientValueLabel->setText("N/A");
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

	// Ensure that any displayed data are up-to-date
	energyGraph_->refreshReferencedDataSets();
	energyGraph_->updateDisplay();
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
	// Write UChromaWidget sessions
	if (!energyGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool EnergyModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
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
	// Remove any current collections
	energyGraph_->clearCollections();

	// Get target Configuration
	currentConfiguration_ = (Configuration*) VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
	if (!currentConfiguration_) return;

	// Set data targets
	CharString blockData;
	blockData.sprintf("Collection 'Total'; Group 'Total'; LineStyle 1.0 Solid; DataSet 'Total'; Source XYData 'XYData@%s//%s//Total'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Inter'; Group 'Inter'; LineStyle 1.0 Solid; DataSet 'Inter'; Source XYData 'XYData@%s//%s//Inter'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Intra'; Group 'Intra'; LineStyle 1.0 Solid; DataSet 'Intra'; Source XYData 'XYData@%s//%s//Intra'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Bond'; Group 'Bond'; LineStyle 1.0 Solid; DataSet 'Bond'; Source XYData 'XYData@%s//%s//Bond'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Angle'; Group 'Angle'; LineStyle 1.0 Solid; DataSet 'Angle'; Source XYData 'XYData@%s//%s//Angle'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);
	blockData.sprintf("Collection 'Torsion'; Group 'Torsion'; LineStyle 1.0 Solid; DataSet 'Torsion'; Source XYData 'XYData@%s//%s//Torsion'; EndDataSet; EndCollection", currentConfiguration_->niceName(), module_->uniqueName());
	energyGraph_->addCollectionFromBlock(blockData);

	updateControls();
}
