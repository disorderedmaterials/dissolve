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
	ui.setupUi(this);

	refreshing_ = true;

	configuration_ = cfg;

	// Populate coordinates file format combo
	ComboPopulator(ui.CoordinatesFileFormatCombo, cfg->inputCoordinates().nFormats(), cfg->inputCoordinates().niceFormats());

	// Set target for ConfigurationViewer
	ui.ViewerWidget->configurationViewer()->setConfiguration(configuration_);

	refreshing_ = false;

	// Set up the ModuleEditor
	ui.ModulePanel->setUp(dissolveWindow, &cfg->moduleLayer(), configuration_);
}

ConfigurationTab::~ConfigurationTab()
{
}

/*
 * Data
 */

// Return tab type
const char* ConfigurationTab::tabType() const
{
	return "ConfigurationTab";
}

/*
 * Configuration Target
 */

// Return displayed Configuration
const Configuration* ConfigurationTab::configuration() const
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

	// Definition
	ui.NameEdit->setText(configuration_->name());
	ui.TemperatureSpin->setValue(configuration_->temperature());

	// Size Factor
	ui.RequestedSizeFactorSpin->setValue(configuration_->requestedSizeFactor()); 
	ui.AppliedSizeFactorSpin->setValue(configuration_->appliedSizeFactor());

	// Box
	const Box* box = configuration_->box();
	ui.CurrentBoxTypeLabel->setText(Box::boxTypes().keyword(box->type()));
	ui.CurrentBoxALabel->setText(QString::number(box->axisLengths().x));
	ui.CurrentBoxBLabel->setText(QString::number(box->axisLengths().y));
	ui.CurrentBoxCLabel->setText(QString::number(box->axisLengths().z));
	ui.CurrentBoxAlphaLabel->setText(QString::number(box->axisAngles().x));
	ui.CurrentBoxBetaLabel->setText(QString::number(box->axisAngles().y));
	ui.CurrentBoxGammaLabel->setText(QString::number(box->axisAngles().z));

	// Input Coordinates
	ui.CoordinatesFileEdit->setText(configuration_->inputCoordinates().filename());
	ui.CoordinatesFileFormatCombo->setCurrentIndex(configuration_->inputCoordinates().formatIndex());
// 	ui.CoordinatesFromFileGroup->setChecked(configuration_->inputCoordinates().is);

	// Viewer
	ui.ViewerWidget->configurationViewer()->postRedisplay();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ConfigurationTab::disableSensitiveControls()
{
	ui.DefinitionGroup->setEnabled(false);
	ui.SizeFactorGroup->setEnabled(false);
}

// Enable sensitive controls within tab, ready for main code to run
void ConfigurationTab::enableSensitiveControls()
{
	ui.DefinitionGroup->setEnabled(true);
	ui.SizeFactorGroup->setEnabled(true);
}

/*
 * Signals / Slots
 */

void ConfigurationTab::on_NameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	configuration_->setName(qPrintable(text));

	dissolveWindow_->setModified();
}

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

void ConfigurationTab::on_GeneratorGenerateButton_clicked(bool checked)
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
		// TODO
// 		configuration_->initialise(dissolve_.worldPool(), true, dissolve_.pairPotentialRange(), dissolve_.nBoxNormalisationPoints());
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
