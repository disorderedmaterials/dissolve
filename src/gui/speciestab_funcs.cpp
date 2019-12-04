/*
	*** SpeciesTab Functions
	*** src/gui/speciestab_funcs.cpp
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

#include "gui/speciestab.h"
#include "gui/gui.h"
#include "gui/delegates/intraformcombo.hui"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/integerspin.hui"
#include "gui/delegates/isotopecombo.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/null.h"
#include "gui/getspeciesnamedialog.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include <QMessageBox>

// Constructor / Destructor
SpeciesTab::SpeciesTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, MainTabsWidget* parent, const char* title, Species* species) : ListItem<SpeciesTab>(), MainTab(dissolveWindow, dissolve, parent, CharString("Species: %s", title), this)
{
	ui_.setupUi(this);

	Locker refreshLocker(refreshLock_);

	species_ = species;

	// Set item delegates in tables
	// -- SpeciesAtomTable
	ui_.AtomTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboNameListItems<AtomType>(dissolve_.atomTypes())));
	for (int n=2; n<6; ++n) ui_.AtomTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
	ui_.AtomTable->horizontalHeader()->setFont(font());
	// -- Bond Table
	for (int n=0; n<2; ++n) ui_.BondTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	ui_.BondTable->setItemDelegateForColumn(2, new IntraFormComboDelegate(this, new ComboEnumOptionsItems<SpeciesBond::BondFunction>(SpeciesBond::bondFunctions()), dissolve_.coreData().masterBonds()));
	// -- Angle Table
	for (int n=0; n<3; ++n) ui_.AngleTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Torsion Table
	for (int n=0; n<4; ++n) ui_.TorsionTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Improper Table
	for (int n=0; n<4; ++n) ui_.ImproperTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Isotopologues Tree
	ui_.IsotopologuesTree->setItemDelegateForColumn(1, new NullDelegate(this));
	ui_.IsotopologuesTree->setItemDelegateForColumn(2, new IsotopeComboDelegate(this));

	// Ensure fonts for table headers are set correctly and the headers themselves are visible
	ui_.AtomTable->horizontalHeader()->setFont(font());
	ui_.AtomTable->horizontalHeader()->setVisible(true);
	ui_.AtomTable->verticalHeader()->setFont(font());
	ui_.AtomTable->verticalHeader()->setVisible(true);
	ui_.BondTable->horizontalHeader()->setFont(font());
	ui_.BondTable->horizontalHeader()->setVisible(true);
	ui_.AngleTable->horizontalHeader()->setFont(font());
	ui_.AngleTable->horizontalHeader()->setVisible(true);
	ui_.TorsionTable->horizontalHeader()->setFont(font());
	ui_.TorsionTable->horizontalHeader()->setVisible(true);
	ui_.ImproperTable->horizontalHeader()->setFont(font());
	ui_.ImproperTable->horizontalHeader()->setVisible(true);

	// Set up SpeciesViewer
	ui_.ViewerWidget->setCoreData(&dissolve.coreData());
	ui_.ViewerWidget->setSpecies(species_);

	// Set up SiteViewer
	ui_.SiteViewerWidget->setCoreData(&dissolve.coreData());
	ui_.SiteViewerWidget->setSpecies(species_);

	// Connect signals / slots
	connect(ui_.ViewerWidget, SIGNAL(dataModified()), this, SLOT(updateControls()));
	connect(ui_.ViewerWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
	connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(atomSelectionChanged()), this, SLOT(updateAtomTableSelection()));
	connect(ui_.SiteViewerWidget, SIGNAL(dataModified()), this, SLOT(updateSitesTab()));
	connect(ui_.SiteViewerWidget, SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));
}

SpeciesTab::~SpeciesTab()
{
}

/*
 * UI
 */

// Update controls in tab
void SpeciesTab::updateControls()
{
	Locker refreshLocker(refreshLock_);

	// View / Generate Tab
	ui_.ViewerWidget->postRedisplay();

	// Geometry Tab
	updateGeometryTab();

	// Isotopologues Tab
	updateIsotopologuesTab();

	// Sites Tab
	updateSitesTab();
}

// Disable sensitive controls within tab
void SpeciesTab::disableSensitiveControls()
{
	setEnabled(false);
}

// Enable sensitive controls within tab
void SpeciesTab::enableSensitiveControls()
{
	setEnabled(true);
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType SpeciesTab::type() const
{
	return MainTab::SpeciesTabType;
}

// Raise suitable dialog for entering / checking new tab name
QString SpeciesTab::getNewTitle(bool& ok)
{
	// Get a new, valid name for the Species
	GetSpeciesNameDialog nameDialog(this, dissolve_.coreData());
	ok = nameDialog.get(species_, species_->name());

	if (ok)
	{
		// Rename our Species, and flag that our data has been modified
		species_->setName(qPrintable(nameDialog.newName()));

		dissolveWindow_->setModified();
	}

	return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool SpeciesTab::canChangeTitle() const
{
	return true;
}

// Close tab, deleting any necessary data
bool SpeciesTab::close()
{
	// Check that we really want to delete this tab
	QMessageBox queryBox;
	queryBox.setText(QString("Really delete the species '%1'?\nThis cannot be undone!").arg(species_->name()));
	queryBox.setInformativeText("Proceed?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret != QMessageBox::Yes) return false;

	Locker refreshLocker(refreshLock_);

	// Remove the Species represented in this tab
	dissolve_.removeSpecies(species_);

	return true;
}

/*
 * Species Target
 */

// Return displayed Species
Species* SpeciesTab::species() const
{
	return species_;
}

/*
 * State
 */

// Read widget state through specified LineParser
bool SpeciesTab::readState(LineParser& parser, const CoreData& coreData)
{
	return true;
}

// Write widget state through specified LineParser
bool SpeciesTab::writeState(LineParser& parser)
{
	return true;
}
