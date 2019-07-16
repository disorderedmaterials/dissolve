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

	// Set item delegates for tables
	// -- SpeciesInfo
	ui.SpeciesInfoTable->setItemDelegateForColumn(0, new ComboListDelegate(this, new ComboNameListItems<Species>(dissolve.species())));
	ui.SpeciesInfoTable->setItemDelegateForColumn(1, new ExponentialSpinDelegate(this));
	ui.SpeciesInfoTable->setItemDelegateForColumn(3, new ComboListDelegate(this, new ComboListEnumItems(SpeciesInfo::nPositioningTypes, SpeciesInfo::positioningTypeKeywords())));

	// Ensure fonts for table headers are set correctly
	ui.SpeciesInfoTable->horizontalHeader()->setFont(font());

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

	// Positioning type
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesInfo>(speciesInfo));
		ui.SpeciesInfoTable->setItem(row, 3, item);
	}
	else item = ui.SpeciesInfoTable->item(row, 3);
	item->setText(SpeciesInfo::positioningType(speciesInfo->insertionPositioning()));
}

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
	TableWidgetUpdater<ConfigurationTab,SpeciesInfo> speciesInfoUpdater(ui.SpeciesInfoTable, configuration_->usedSpecies(), this, &ConfigurationTab::updateSpeciesInfoTableRow);
	ui.SpeciesInfoTable->resizeColumnsToContents();

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
	ui.BoxGroup->setEnabled(false);
	ui.ContentsGroup->setEnabled(false);
	ui.SizeFactorGroup->setEnabled(false);
	ui.ActionsGroup->setEnabled(false);
}

// Enable sensitive controls within tab, ready for main code to run
void ConfigurationTab::enableSensitiveControls()
{
	ui.DefinitionGroup->setEnabled(true);
	ui.BoxGroup->setEnabled(true);
	ui.ContentsGroup->setEnabled(true);
	ui.SizeFactorGroup->setEnabled(true);
	ui.ActionsGroup->setEnabled(true);
}

/*
 * Signals / Slots
 */

// Return current SpeciesInfo
SpeciesInfo* ConfigurationTab::currentSpeciesInfo() const
{
	if (!ui.SpeciesInfoTable->currentItem()) return NULL;

	SpeciesInfo* spInfo = VariantPointer<SpeciesInfo>(ui.SpeciesInfoTable->currentItem()->data(Qt::UserRole));

	return spInfo;
}

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

// Box
void ConfigurationTab::on_BoxASpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(0, value);

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_BoxBSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(1, value);

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_BoxCSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setRelativeBoxLength(2, value);

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_BoxAlphaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(0, value);

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_BoxBetaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(1, value);

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_BoxGammaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	configuration_->setBoxAngle(2, value);

	dissolveWindow_->setModified();
}

// Content
void ConfigurationTab::on_MultiplierSpin_valueChanged(int value)
{
	if (refreshing_) return;

	configuration_->setMultiplier(value);

	dissolveWindow_->setModifiedAndInvalidated();
}

void ConfigurationTab::on_DensitySpin_valueChanged(double value)
{
	if (refreshing_) return;

	if (ui.DensityUnitsCombo->currentIndex() == 0) configuration_->setAtomicDensity(value);
	else configuration_->setChemicalDensity(value);

	dissolveWindow_->setModifiedAndInvalidated();
}

void ConfigurationTab::on_SpeciesInfoAddButton_clicked(bool checked)
{
	if (refreshing_) return;

	configuration_->addUsedSpecies(dissolve_.species().first(), 1.0);

	updateControls();

	dissolveWindow_->setModifiedAndInvalidated();
}

void ConfigurationTab::on_SpeciesInfoRemoveButton_clicked(bool checked)
{
	if (refreshing_) return;

// 	dissolveWindow_->setModified();
}

void ConfigurationTab::on_DensityUnitsCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	if (ui.DensityUnitsCombo->currentIndex() == 0) configuration_->setAtomicDensity(ui.DensitySpin->value());
	else configuration_->setChemicalDensity(ui.DensitySpin->value());

	dissolveWindow_->setModified();
}

void ConfigurationTab::on_SpeciesInfoTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get the currently-selected SpeciesInfo
	SpeciesInfo* spInfo = currentSpeciesInfo();
	if (!spInfo) return;

	// Column of passed item tells us the type of data we need to change
	Species* sp;
	SpeciesInfo::PositioningType pt;
	switch (w->column())
	{
		// Species
		case (0):
			// Find the named Species
			sp = dissolve_.findSpecies(qPrintable(w->text()));
			if (!sp)
			{
				Messenger::error("Unrecognised Species '%s' found in SpeciesInfo table for Configuration '%s'.\n", qPrintable(w->text()), configuration_->name());
				return;
			}
			spInfo->setSpecies(sp);
			dissolveWindow_->setModifiedAndInvalidated();
			break;
		// Relative Population
		case (1):
			spInfo->setPopulation(w->text().toDouble());
			dissolveWindow_->setModifiedAndInvalidated();
			break;
		// Rotation on insertion
		case (2):
			spInfo->setRotateOnInsertion(w->checkState() == Qt::Checked);
			dissolveWindow_->setModified();
			break;
		// Translate on insertion
		case (3):
			pt = SpeciesInfo::positioningType(qPrintable(w->text()));
			if (pt != SpeciesInfo::nPositioningTypes) spInfo->setInsertionPositioning(pt);
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of SpeciesInfo table.\n", w->column());
			break;
	}
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
		configuration_->initialise(dissolve_.worldPool(), true, dissolve_.pairPotentialRange(), dissolve_.nBoxNormalisationPoints());
	}
}

void ConfigurationTab::on_ExportButton_clicked(bool checked)
{
	// TODO Offer more control here!
	CoordinateExportFileFormat fileFormat(CharString("%s.xyz", configuration_->niceName()), CoordinateExportFileFormat::XYZCoordinates);
	fileFormat.exportData(configuration_);
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
