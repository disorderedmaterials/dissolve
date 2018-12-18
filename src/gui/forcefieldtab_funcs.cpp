/*
	*** ForcefieldTab Functions
	*** src/gui/forcefieldtab_funcs.cpp
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

#include "gui/forcefieldtab.h"
#include "gui/gui.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/texponentialspin.hui"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/speciesbond.h"
#include "classes/speciesangle.h"
#include "classes/speciestorsion.h"
#include <QListWidgetItem>

// Constructor / Destructor
ForcefieldTab::ForcefieldTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title) : MainTab(dissolveWindow, dissolve, parent, title, this)
{
	ui.setupUi(this);

	refreshing_ = true;

	/*
	 * Master Terms
	 */
	
	// Set item delegates for tables
	// -- Functional Forms
	ui.MasterBondsTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboListEnumItems(SpeciesBond::nBondFunctions, SpeciesBond::bondFunctions())));
	ui.MasterAnglesTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboListEnumItems(SpeciesAngle::nAngleFunctions, SpeciesAngle::angleFunctions())));
	ui.MasterTorsionsTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboListEnumItems(SpeciesTorsion::nTorsionFunctions, SpeciesTorsion::torsionFunctions())));

	// -- Parameters
	for (int n=2; n<6; ++n)
	{
		ui.MasterBondsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.MasterAnglesTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.MasterTorsionsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
	}

	// Ensure fonts for table headers are set correctly
	ui.MasterBondsTable->horizontalHeader()->setFont(font());
	ui.MasterAnglesTable->horizontalHeader()->setFont(font());
	ui.MasterTorsionsTable->horizontalHeader()->setFont(font());

	/*
	 * Atom Types
	 */

	// Set item delegates for tables
	// -- Charge / Parameters
	for (int n=2; n<7; ++n) ui.AtomTypesTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));

	// Ensure fonts for table headers are set correctly
	ui.AtomTypesTable->horizontalHeader()->setFont(font());

	/*
	 * Pair Potentials
	 */
	
	for (int n=0; n<PairPotential::nShortRangeTypes; ++n) ui.ShortRangeFormCombo->addItem(PairPotential::shortRangeType( (PairPotential::ShortRangeType) n));
	for (int n=0; n<PairPotential::nCoulombTruncationSchemes; ++n) ui.CoulombTruncationCombo->addItem(PairPotential::coulombTruncationScheme( (PairPotential::CoulombTruncationScheme) n));
	for (int n=0; n<PairPotential::nShortRangeTruncationSchemes; ++n) ui.ShortRangeTruncationCombo->addItem(PairPotential::shortRangeTruncationScheme( (PairPotential::ShortRangeTruncationScheme) n));


	// Set delegates for table
	// -- Functional Forms
	ui.PairPotentialsTable->setItemDelegateForColumn(2, new ComboListDelegate(this, new ComboListEnumItems(PairPotential::nShortRangeTypes, PairPotential::shortRangeTypes())));

	// -- Charges / Parameters
	for (int n=3; n<9; ++n) ui.PairPotentialsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));

	refreshing_ = false;
}

ForcefieldTab::~ForcefieldTab()
{
}

/*
 * Data
 */

// Return tab type
const char* ForcefieldTab::tabType() const
{
	return "ForcefieldTab";
}

/*
 * Update
 */

// Row update function for BondsTable
void ForcefieldTab::updateBondsTableRow(int row, MasterIntra* masterBond, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		ui.MasterBondsTable->setItem(row, 0, item);
	}
	else item = ui.MasterBondsTable->item(row, 0);
	item->setText(masterBond->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		ui.MasterBondsTable->setItem(row, 1, item);
	}
	else item = ui.MasterBondsTable->item(row, 1);
	item->setText(SpeciesBond::bondFunction( (SpeciesBond::BondFunction) masterBond->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
			ui.MasterBondsTable->setItem(row, n+2, item);
		}
		else item = ui.MasterBondsTable->item(row, n+2);
		item->setText(QString::number(masterBond->parameter(n)));
	}
}

// Row update function for AnglesTable
void ForcefieldTab::updateAnglesTableRow(int row, MasterIntra* masterAngle, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		ui.MasterAnglesTable->setItem(row, 0, item);
	}
	else item = ui.MasterAnglesTable->item(row, 0);
	item->setText(masterAngle->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		ui.MasterAnglesTable->setItem(row, 1, item);
	}
	else item = ui.MasterAnglesTable->item(row, 1);
	item->setText(SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) masterAngle->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
			ui.MasterAnglesTable->setItem(row, n+2, item);
		}
		else item = ui.MasterAnglesTable->item(row, n+2);
		item->setText(QString::number(masterAngle->parameter(n)));
	}
}

// Row update function for TorsionsTable
void ForcefieldTab::updateTorsionsTableRow(int row, MasterIntra* masterTorsion, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		ui.MasterTorsionsTable->setItem(row, 0, item);
	}
	else item = ui.MasterTorsionsTable->item(row, 0);
	item->setText(masterTorsion->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		ui.MasterTorsionsTable->setItem(row, 1, item);
	}
	else item = ui.MasterTorsionsTable->item(row, 1);
	item->setText(SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) masterTorsion->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
			ui.MasterTorsionsTable->setItem(row, n+2, item);
		}
		else item = ui.MasterTorsionsTable->item(row, n+2);
		item->setText(QString::number(masterTorsion->parameter(n)));
	}
}

// Row update function for AtomTypesTable
void ForcefieldTab::updateAtomTypesTableRow(int row, AtomType* atomType, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		ui.AtomTypesTable->setItem(row, 0, item);
	}
	else item = ui.AtomTypesTable->item(row, 0);
	item->setText(atomType->name());

	// Charge
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		ui.AtomTypesTable->setItem(row, 1, item);
	}
	else item = ui.AtomTypesTable->item(row, 1);
	item->setText(QString::number(atomType->parameters().charge()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
			ui.AtomTypesTable->setItem(row, n+2, item);
		}
		else item = ui.AtomTypesTable->item(row, n+2);
		item->setText(QString::number(atomType->parameters().parameter(n)));
	}
}

// Row update function for PairPotentialsTable
void ForcefieldTab::updatePairPotentialsTableRow(int row, PairPotential* pairPotential, bool createItems)
{
	QTableWidgetItem* item;

	// Type I
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
		item->setFlags(Qt::ItemIsSelectable);
		ui.PairPotentialsTable->setItem(row, 0, item);
	}
	else item = ui.PairPotentialsTable->item(row, 0);
	item->setText(pairPotential->atomTypeNameI());

	// Type J
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
		item->setFlags(Qt::ItemIsSelectable);
		ui.PairPotentialsTable->setItem(row, 1, item);
	}
	else item = ui.PairPotentialsTable->item(row, 1);
	item->setText(pairPotential->atomTypeNameJ());

	// Short-Range Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
		ui.PairPotentialsTable->setItem(row, 2, item);
	}
	else item = ui.PairPotentialsTable->item(row, 2);
	item->setText(PairPotential::shortRangeType( (PairPotential::ShortRangeType) pairPotential->shortRangeType()));

	// Charge I
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
		ui.PairPotentialsTable->setItem(row, 3, item);
	}
	else item = ui.PairPotentialsTable->item(row, 3);
	item->setText(QString::number(pairPotential->chargeI()));

	// Charge I
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
		ui.PairPotentialsTable->setItem(row, 4, item);
	}
	else item = ui.PairPotentialsTable->item(row, 4);
	item->setText(QString::number(pairPotential->chargeJ()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<PairPotential>(pairPotential));
			ui.PairPotentialsTable->setItem(row, n+5, item);
		}
		else item = ui.PairPotentialsTable->item(row, n+5);
		item->setText(QString::number(pairPotential->parameter(n)));
	}
}

// Update controls in tab
void ForcefieldTab::updateControls()
{
	refreshing_ = true;

	// Master Bonds Table
	TableWidgetUpdater<ForcefieldTab,MasterIntra> bondsUpdater(ui.MasterBondsTable, dissolve_.masterBonds(), this, &ForcefieldTab::updateBondsTableRow);
	ui.MasterBondsTable->resizeColumnsToContents();

	// Master Angles Table
	TableWidgetUpdater<ForcefieldTab,MasterIntra> anglesUpdater(ui.MasterAnglesTable, dissolve_.masterAngles(), this, &ForcefieldTab::updateAnglesTableRow);
	ui.MasterAnglesTable->resizeColumnsToContents();

	// Torsions Table
	TableWidgetUpdater<ForcefieldTab,MasterIntra> torsionsUpdater(ui.MasterTorsionsTable, dissolve_.masterTorsions(), this, &ForcefieldTab::updateTorsionsTableRow);
	ui.MasterTorsionsTable->resizeColumnsToContents();

	// AtomTypes Table
	TableWidgetUpdater<ForcefieldTab,AtomType> atomTypesUpdater(ui.AtomTypesTable, dissolve_.atomTypes(), this, &ForcefieldTab::updateAtomTypesTableRow);
	ui.AtomTypesTable->resizeColumnsToContents();

	// PairPotentials
	ui.PairPotentialRangeSpin->setValue(dissolve_.pairPotentialRange());
	ui.PairPotentialDeltaSpin->setValue(dissolve_.pairPotentialDelta());
	ui.ShortRangeFormCombo->setCurrentIndex(PairPotential::LennardJonesType);
	ui.CoulombIncludeCheck->setChecked(dissolve_.pairPotentialsIncludeCoulomb());
	ui.ShortRangeTruncationCombo->setCurrentIndex(PairPotential::shortRangeTruncationScheme());
	ui.ShortRangeTruncationWidthSpin->setValue(PairPotential::shortRangeTruncationWidth());
	ui.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() == PairPotential::CosineShortRangeTruncation);
	ui.CoulombTruncationCombo->setCurrentIndex(PairPotential::coulombTruncationScheme());
	// -- Table
	TableWidgetUpdater<ForcefieldTab,PairPotential> ppUpdater(ui.PairPotentialsTable, dissolve_.pairPotentials(), this, &ForcefieldTab::updatePairPotentialsTableRow);
	ui.PairPotentialsTable->resizeColumnsToContents();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ForcefieldTab::disableSensitiveControls()
{
	setEnabled(false);
}

// Enable sensitive controls within tab, ready for main code to run
void ForcefieldTab::enableSensitiveControls()
{
	setEnabled(false);
}

/*
 * Signals / Slots
 */

void ForcefieldTab::on_AtomTypeAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_AtomTypeRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_AtomTypeLibraryButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterTermAddBondButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterTermRemoveBondButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterBondsTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = w ? VariantPointer<MasterIntra>(w->data(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	// Column of passed item tells us the type of data we need to change
	switch (w->column())
	{
		// Name
		case (0):
			masterIntra->setName(qPrintable(w->text()));
			dissolveWindow_->setModified();
			break;
		// Functional Form
		case (1):
			masterIntra->setForm(SpeciesBond::bondFunction(qPrintable(w->text())));
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column()-2, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of MasterIntra table.\n", w->column());
			break;
	}
}

void ForcefieldTab::on_MasterTermAddAngleButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterTermRemoveAngleButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterAnglesTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = w ? VariantPointer<MasterIntra>(w->data(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	// Column of passed item tells us the type of data we need to change
	switch (w->column())
	{
		// Name
		case (0):
			masterIntra->setName(qPrintable(w->text()));
			dissolveWindow_->setModified();
			break;
		// Functional Form
		case (1):
			masterIntra->setForm(SpeciesAngle::angleFunction(qPrintable(w->text())));
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column()-2, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of MasterIntra table.\n", w->column());
			break;
	}
}

void ForcefieldTab::on_MasterTermAddTorsionButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterTermRemoveTorsionButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET.\n");
}

void ForcefieldTab::on_MasterTorsionsTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = w ? VariantPointer<MasterIntra>(w->data(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	// Column of passed item tells us the type of data we need to change
	switch (w->column())
	{
		// Name
		case (0):
			masterIntra->setName(qPrintable(w->text()));
			dissolveWindow_->setModified();
			break;
		// Functional Form
		case (1):
			masterIntra->setForm(SpeciesTorsion::torsionFunction(qPrintable(w->text())));
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column()-2, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of MasterIntra table.\n", w->column());
			break;
	}
}

void ForcefieldTab::on_AtomTypesTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target AtomType from the passed widget
	AtomType* atomType = w ? VariantPointer<AtomType>(w->data(Qt::UserRole)) : NULL;
	if (!atomType) return;

	// Column of passed item tells us the type of data we need to change
	switch (w->column())
	{
		// Name
		case (0):
			atomType->setName(qPrintable(w->text()));
			dissolveWindow_->setModified();
			break;
		// Charge
		case (1):
			atomType->parameters().setCharge(w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			atomType->parameters().setParameter(w->column()-2, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of AtomTypes table.\n", w->column());
			break;
	}
}

void ForcefieldTab::on_PairPotentialRangeSpin_valueChanged(double value)
{
	if (refreshing_) return;

	dissolve_.setPairPotentialRange(value);
	
	dissolveWindow_->setModified();
}

void ForcefieldTab::on_PairPotentialDeltaSpin_valueChanged(double value)
{
	if (refreshing_) return;

	dissolve_.setPairPotentialDelta(value);
	
	dissolveWindow_->setModified();
}

void ForcefieldTab::on_CoulombIncludeCheck_clicked(bool checked)
{
	if (refreshing_) return;

	dissolve_.setPairPotentialsIncludeCoulomb(checked);
	
	dissolveWindow_->setModified();
}

void ForcefieldTab::on_ShortRangeTruncationCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	PairPotential::setShortRangeTruncationScheme( (PairPotential::ShortRangeTruncationScheme) index );
	ui.ShortRangeTruncationWidthSpin->setEnabled(PairPotential::shortRangeTruncationScheme() == PairPotential::CosineShortRangeTruncation);
	
	dissolveWindow_->setModified();
}

void ForcefieldTab::on_CoulombTruncationCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	PairPotential::setCoulombTruncationScheme( (PairPotential::CoulombTruncationScheme) index );

	dissolveWindow_->setModified();
}

void ForcefieldTab::on_RegenerateAllPairPotentialsButton_clicked(bool checked)
{
	dissolve_.regeneratePairPotentials((PairPotential::ShortRangeType) ui.ShortRangeFormCombo->currentIndex());

	refreshing_ = true;

	TableWidgetUpdater<ForcefieldTab,PairPotential> ppUpdater(ui.PairPotentialsTable, dissolve_.pairPotentials(), this, &ForcefieldTab::updatePairPotentialsTableRow);
	ui.PairPotentialsTable->resizeColumnsToContents();

	refreshing_ = false;

	dissolveWindow_->updateStatus();
}

void ForcefieldTab::on_UpdatePairPotentialsButton_clicked(bool checked)
{
	dissolve_.updateCurrentPairPotentials();

	updateControls();

	refreshing_ = true;

	TableWidgetUpdater<ForcefieldTab,PairPotential> ppUpdater(ui.PairPotentialsTable, dissolve_.pairPotentials(), this, &ForcefieldTab::updatePairPotentialsTableRow);
	ui.PairPotentialsTable->resizeColumnsToContents();

	refreshing_ = false;

	dissolveWindow_->updateStatus();
}

void ForcefieldTab::on_GenerateMissingPairPotentialsButton_clicked(bool checked)
{
	dissolve_.generateMissingPairPotentials((PairPotential::ShortRangeType) ui.ShortRangeFormCombo->currentIndex());

	refreshing_ = true;

	TableWidgetUpdater<ForcefieldTab,PairPotential> ppUpdater(ui.PairPotentialsTable, dissolve_.pairPotentials(), this, &ForcefieldTab::updatePairPotentialsTableRow);
	ui.PairPotentialsTable->resizeColumnsToContents();

	refreshing_ = false;

	dissolveWindow_->updateStatus();
}

void ForcefieldTab::on_PairPotentialsTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target PairPotential from the passed widget
	PairPotential* pairPotential = w ? VariantPointer<PairPotential>(w->data(Qt::UserRole)) : NULL;
	if (!pairPotential) return;

	// Column of passed item tells us the type of data we need to change
	switch (w->column())
	{
		// Functional form
		case (2):
			pairPotential->setShortRangeType(PairPotential::shortRangeType(qPrintable(w->text())));
			dissolveWindow_->setModified();
			break;
		// Charge I
		case (3):
			pairPotential->setChargeI(w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		// Charge J
		case (4):
			pairPotential->setChargeJ(w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (5):
		case (6):
		case (7):
		case (8):
			pairPotential->setParameter(w->column()-5, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of PairPotentials table.\n", w->column());
			break;
	}
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ForcefieldTab::readState(LineParser& parser, const CoreData& coreData)
{
	return true;
}

// Write widget state through specified LineParser
bool ForcefieldTab::writeState(LineParser& parser)
{
	return true;
}
