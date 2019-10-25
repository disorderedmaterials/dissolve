/*
	*** ConfigurationTab Functions
	*** src/gui/configurationtab_funcs.cpp
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

#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/getconfigurationnamedialog.h"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "modules/export/export.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"
#include <QMessageBox>

// Constructor / Destructor
ConfigurationTab::ConfigurationTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Configuration* cfg) : ListItem<ConfigurationTab>(), MainTab(dissolveWindow, dissolve, parent, CharString("Configuration: %s", title), this)
{
	ui_.setupUi(this);

	refreshing_ = true;

	configuration_ = cfg;

	// Populate coordinates file format combo
	ComboPopulator(ui_.CoordinatesFileFormatCombo, cfg->inputCoordinates().nFormats(), cfg->inputCoordinates().niceFormats());

	// Set target for ConfigurationViewer
	ui_.ViewerWidget->configurationViewer()->setConfiguration(configuration_);

	// Set target for ProcedureEditor, and connect signals
	ui_.ProcedureWidget->setUp(&configuration_->generator(), dissolve.coreData());
	connect(ui_.ProcedureWidget, SIGNAL(dataModified()), dissolveWindow, SLOT(setModified()));

	refreshing_ = false;

	// Set up the ModuleEditor
	ui_.ModulePanel->setUp(dissolveWindow, &cfg->moduleLayer(), configuration_);
}

ConfigurationTab::~ConfigurationTab()
{
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ConfigurationTab::type() const
{
	return MainTab::ConfigurationTabType;
}

// Raise suitable dialog for entering / checking new tab name
QString ConfigurationTab::getNewTitle(bool& ok)
{
	// Get a new, valid name for the Configuration
	GetConfigurationNameDialog nameDialog(this, dissolve_.coreData());
	ok = nameDialog.get(configuration_, configuration_->name());

	if (ok)
	{
		// Rename our Configuration, and flag that our data has been modified
		configuration_->setName(qPrintable(nameDialog.newName()));

		dissolveWindow_->setModified();
	}

	return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool ConfigurationTab::canChangeTitle() const
{
	return true;
}

/*
 * Configuration Target
 */

// Return displayed Configuration
Configuration* ConfigurationTab::configuration() const
{
	return configuration_;
}

/*
 * Update
 */

// Update controls in tab
void ConfigurationTab::updateControls()
{
	refreshing_ = true;

	// Temperature
	ui_.TemperatureSpin->setValue(configuration_->temperature());

	// Size Factor
	ui_.RequestedSizeFactorSpin->setValue(configuration_->requestedSizeFactor()); 
	ui_.AppliedSizeFactorSpin->setValue(configuration_->appliedSizeFactor());

	// Box
	const Box* box = configuration_->box();
	ui_.CurrentBoxTypeLabel->setText(Box::boxTypes().keyword(box->type()));
	ui_.CurrentBoxALabel->setText(QString::number(box->axisLengths().x));
	ui_.CurrentBoxBLabel->setText(QString::number(box->axisLengths().y));
	ui_.CurrentBoxCLabel->setText(QString::number(box->axisLengths().z));
	ui_.CurrentBoxAlphaLabel->setText(QString::number(box->axisAngles().x));
	ui_.CurrentBoxBetaLabel->setText(QString::number(box->axisAngles().y));
	ui_.CurrentBoxGammaLabel->setText(QString::number(box->axisAngles().z));

	// Input Coordinates
	ui_.CoordinatesFileEdit->setText(configuration_->inputCoordinates().filename());
	ui_.CoordinatesFileFormatCombo->setCurrentIndex(configuration_->inputCoordinates().formatIndex());

	// Viewer
	ui_.ViewerWidget->configurationViewer()->postRedisplay();

	refreshing_ = false;
}

// Disable sensitive controls within tab
void ConfigurationTab::disableSensitiveControls()
{
	ui_.GeneratorGroup->setEnabled(false);
	ui_.TemperatureGroup->setEnabled(false);
	ui_.SizeFactorGroup->setEnabled(false);
}

// Enable sensitive controls within tab
void ConfigurationTab::enableSensitiveControls()
{
	ui_.GeneratorGroup->setEnabled(true);
	ui_.TemperatureGroup->setEnabled(true);
	ui_.SizeFactorGroup->setEnabled(true);
}

/*
 * Signals / Slots
 */

void ConfigurationTab::on_TemperatureSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setTemperature(value);

	dissolveWindow_->setModified();
}

// Initial Coordinates
void ConfigurationTab::on_CoordinatesFileEdit_textChanged(QString text)
{
	if (refreshing_) return;
}

void ConfigurationTab::on_CoordinatesFileSelectButton_clicked(bool checked)
{
	if (refreshing_) return;
}

void ConfigurationTab::on_GeneratorRegenerateButton_clicked(bool checked)
{
	// Are we sure that's what we want to do?
	QMessageBox queryBox;
	queryBox.setText(QString("This will erase the current contents of the Configuration '%1'.").arg(configuration_->name()));
	queryBox.setInformativeText("Proceed?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Yes)
	{
		configuration_->initialiseContent(dissolve_.worldPool(), dissolve_.pairPotentialRange(), true);
		updateControls();
	}
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ConfigurationTab::readState(LineParser& parser, const CoreData& coreData)
{
	return true;
}

// Write widget state through specified LineParser
bool ConfigurationTab::writeState(LineParser& parser)
{
	return true;
}
