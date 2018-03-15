/*
	*** ConfigurationTab Functions
	*** src/gui/configurationtab_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/configurationtab.h"
#include "gui/gui.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/texponentialspin.hui"
#include "gui/helpers/combopopulator.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/duq.h"
#include "modules/export/export.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"
#include <QMessageBox>

// Constructor / Destructor
ConfigurationTab::ConfigurationTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, Configuration* cfg) : MainTab(duqWindow, duq, parent, CharString("Configuration: %s", title), this)
{
	ui.setupUi(this);

	configuration_ = cfg;

	// Set item delegates for tables
	// -- SpeciesInfo
	ui.SpeciesInfoTable->setItemDelegateForColumn(0, new ComboListDelegate(this, new ComboNameListItems<Species>(List<Species>::masterInstance())));
	ui.SpeciesInfoTable->setItemDelegateForColumn(1, new TExponentialSpinDelegate(this));

	// Ensure fonts for table headers are set correctly
	ui.SpeciesInfoTable->horizontalHeader()->setFont(font());

	// Populate coordinates file format combo
	ComboPopulator(ui.CoordinatesFileFormatCombo, ImportModuleFormats::nCoordinateFormats, ImportModuleFormats::niceCoordinateFormats());

	// Add current Module widgets to layout
	addModuleWidgets(configuration_->modules().modules(), subWidgets_, ui.ModuleWidgetLayout, true);
}

ConfigurationTab::~ConfigurationTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return the tab's SubWindow area, if it has one
QMdiArea* ConfigurationTab::subWindowArea()
{
	return NULL;
}

// Return the tab's SubWidget layout, if it has one
QLayout* ConfigurationTab::subWidgetLayout()
{
	return ui.ModuleWidgetLayout;
}

/*
 * Update
 */

// Row update function for BondsTable
void ConfigurationTab::updateSpeciesInfoTableRow(int row, SpeciesInfo* speciesInfo, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesInfo>(speciesInfo));
		ui.SpeciesInfoTable->setItem(row, 0, item);
	}
	else item = ui.SpeciesInfoTable->item(row, 0);
	item->setText(speciesInfo->species() ? speciesInfo->species()->name() : "???");

	// Population
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesInfo>(speciesInfo));
		ui.SpeciesInfoTable->setItem(row, 1, item);
	}
	else item = ui.SpeciesInfoTable->item(row, 1);
	item->setText(QString::number(speciesInfo->population()));

	// Rotate on insertion?
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesInfo>(speciesInfo));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui.SpeciesInfoTable->setItem(row, 2, item);
	}
	else item = ui.SpeciesInfoTable->item(row, 2);
	item->setCheckState(speciesInfo->rotateOnInsertion() ? Qt::Checked : Qt::Unchecked);

	// Translate on insertion?
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesInfo>(speciesInfo));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		ui.SpeciesInfoTable->setItem(row, 3, item);
	}
	else item = ui.SpeciesInfoTable->item(row, 3);
	item->setCheckState(speciesInfo->translateOnInsertion() ? Qt::Checked : Qt::Unchecked);
}

// Update controls in tab
void ConfigurationTab::updateControls()
{
	refreshing_ = true;

	// Definition
	ui.NameEdit->setText(configuration_->name());
	ui.TemperatureSpin->setValue(configuration_->temperature());

	// Box
	ui.BoxASpin->setValue(configuration_->relativeBoxLengths().x);
	ui.BoxBSpin->setValue(configuration_->relativeBoxLengths().y);
	ui.BoxCSpin->setValue(configuration_->relativeBoxLengths().z);
	ui.BoxAlphaSpin->setValue(configuration_->boxAngles().x);
	ui.BoxBetaSpin->setValue(configuration_->boxAngles().y);
	ui.BoxGammaSpin->setValue(configuration_->boxAngles().z);

	// Contents
	ui.MultiplierSpin->setValue(configuration_->multiplier());
	ui.DensitySpin->setValue(configuration_->density());
	ui.DensityUnitsCombo->setCurrentIndex(configuration_->densityIsAtomic() ? 0 : 1);
	TableWidgetUpdater<ConfigurationTab,SpeciesInfo> bondsUpdater(ui.SpeciesInfoTable, configuration_->usedSpecies(), this, &ConfigurationTab::updateSpeciesInfoTableRow);
	ui.SpeciesInfoTable->resizeColumnsToContents();

	// Input Coordinates
	ui.CoordinatesFileEdit->setText(configuration_->inputCoordinatesFile());
	ui.CoordinatesFileFormatCombo->setCurrentIndex(configuration_->inputCoordinatesFormat());
	ui.CoordinatesFromFileGroup->setChecked(configuration_->hasInputCoordinatesFile());

	// Loop over our SubWidgets
	ListIterator<SubWidget> subWidgetIterator(subWidgets_);
	while (SubWidget* subWidget = subWidgetIterator.iterate()) subWidget->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ConfigurationTab::disableSensitiveControls()
{
	// Disable sensitive controls in SubWidgets
	ListIterator<SubWidget> subWidgetIterator(subWidgets_);
	while (SubWidget* subWidget = subWidgetIterator.iterate()) subWidget->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ConfigurationTab::enableSensitiveControls()
{
	// Enable sensitive controls in SubWidgets
	ListIterator<SubWidget> subWidgetIterator(subWidgets_);
	while (SubWidget* subWidget = subWidgetIterator.iterate()) subWidget->enableSensitiveControls();
}

/*
 * Signals / Slots
 */

void ConfigurationTab::on_NameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	configuration_->setName(qPrintable(text));

	duqWindow_->setModified();
}

void ConfigurationTab::on_TemperatureSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setTemperature(value);

	duqWindow_->setModified();
}

// Box
void ConfigurationTab::on_BoxASpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(0, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_BoxBSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(1, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_BoxCSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(2, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_BoxAlphaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(0, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_BoxBetaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(1, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_BoxGammaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(2, value);

	duqWindow_->setModified();
}

// Content
void ConfigurationTab::on_MultiplierSpin_valueChanged(int value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(2, value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_DensitySpin_valueChanged(double value)
{
	if (refreshing_) return;

	if (ui.DensityUnitsCombo->currentIndex() == 0) configuration_->setAtomicDensity(value);
	else configuration_->setChemicalDensity(value);

	duqWindow_->setModified();
}

void ConfigurationTab::on_SpeciesInfoAddButton_clicked(bool checked)
{
	if (refreshing_) return;

// 	duqWindow_->setModified();
}

void ConfigurationTab::on_SpeciesInfoRemoveButton_clicked(bool checked)
{
	if (refreshing_) return;

// 	duqWindow_->setModified();
}

void ConfigurationTab::on_DensityUnitsCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	if (ui.DensityUnitsCombo->currentIndex() == 0) configuration_->setAtomicDensity(ui.DensitySpin->value());
	else configuration_->setChemicalDensity(ui.DensitySpin->value());

	duqWindow_->setModified();
}

void ConfigurationTab::on_SpeciesInfoTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;
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

void ConfigurationTab::on_RegenerateNowButton_clicked(bool checked)
{
	// Are we sure that's what we want to do?
	QMessageBox queryBox;
	queryBox.setText(QString("This will erase the contents of the Configuration '%1'.").arg(configuration_->name()));
	queryBox.setInformativeText("Proceed?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Yes)
	{
		configuration_->initialise(duq_.worldPool(), true, duq_.pairPotentialRange(), duq_.nBoxNormalisationPoints());
	}
}

void ConfigurationTab::on_ExportButton_clicked(bool checked)
{
	LineParser parser;
	parser.openOutput(CharString("%s.xyz", configuration_->niceName()));
	ExportModule::writeConfigurationXYZ(parser, configuration_, CharString("Configuration '%s'", configuration_->name()));
	parser.closeFiles();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool ConfigurationTab::writeState(LineParser& parser)
{
	// Loop over our SubWidgets
	ListIterator<SubWidget> subWidgetIterator(subWidgets_);
	while (SubWidget* subWidget = subWidgetIterator.iterate())
	{
		// Write window state
		if (!parser.writeLineF("'%s'  %s  '%s'\n", title_.get(), subWidget->widgetType(), subWidget->title())) return false;
		if (!parser.writeLineF("0\n")) return false;
		if (!subWidget->writeState(parser)) return false;
	}

	return true;
}
