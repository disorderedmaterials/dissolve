/*
	*** MasterTerms Widget - Functions
	*** src/gui/mastertermswidget_funcs.cpp
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

#include "gui/mastertermswidget.h"
#include "gui/subwindow.h"
#include "gui/texponentialspindelegate.hui"
#include "gui/tablewidgetupdater.h"
#include "main/duq.h"
#include "classes/masterintra.h"
#include "classes/speciesbond.h"
#include "classes/speciesangle.h"
#include "classes/speciestorsion.h"
#include "base/lineparser.h"
#include "templates/variantpointer.h"
#include <QComboBox>

// Constructor
MasterTermsWidget::MasterTermsWidget(QWidget* parent, DUQ& dUQ) : SubWidget(parent), duq_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Set item delegates for tables
	for (int n=2; n<6; ++n)
	{
		ui.BondsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.AnglesTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.TorsionsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
	}

	// Ensure fonts for table headers are set correctly
	ui.BondsTable->horizontalHeader()->setFont(ui.InfoFrame->font());
	ui.AnglesTable->horizontalHeader()->setFont(ui.InfoFrame->font());
	ui.TorsionsTable->horizontalHeader()->setFont(ui.InfoFrame->font());

	// Update window contents
	updateControls();

	refreshing_ = false;
}

MasterTermsWidget::~MasterTermsWidget()
{
}

/*
 * Widget Functions, Signals, and Slots
 */

// Row update function for BondsTable
void MasterTermsWidget::updateBondsTableRow(int row, MasterIntra* masterBond, bool createItems)
{
	QTableWidgetItem* item;
	QComboBox* combo;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		ui.BondsTable->setItem(row, 0, item);
	}
	else item = ui.BondsTable->item(row, 0);
	item->setText(masterBond->name());

	// Form
	if (createItems)
	{
		combo = new QComboBox;
		QStringList forms;
		for (int n=0; n<SpeciesBond::nBondFunctions; ++n) forms << SpeciesBond::bondFunction( (SpeciesBond::BondFunction) n);
		combo->addItems(forms);
		combo->setItemData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(bondFunctionChanged(int)));
		ui.BondsTable->setCellWidget(row, 1, combo);
	}
	else combo = static_cast<QComboBox*>(ui.BondsTable->cellWidget(row, 1));
	combo->setCurrentIndex(masterBond->form());

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
			ui.BondsTable->setItem(row, n+2, item);
		}
		else item = ui.BondsTable->item(row, n+2);
		item->setText(QString::number(masterBond->parameter(n)));
	}
}

// Row update function for AnglesTable
void MasterTermsWidget::updateAnglesTableRow(int row, MasterIntra* masterAngle, bool createItems)
{
	QTableWidgetItem* item;
	QComboBox* combo;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		ui.AnglesTable->setItem(row, 0, item);
	}
	else item = ui.AnglesTable->item(row, 0);
	item->setText(masterAngle->name());

	// Form
	if (createItems)
	{
		combo = new QComboBox;
		QStringList forms;
		for (int n=0; n<SpeciesAngle::nAngleFunctions; ++n) forms << SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) n);
		combo->addItems(forms);
		combo->setItemData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(angleFunctionChanged(int)));
		ui.AnglesTable->setCellWidget(row, 1, combo);
	}
	else combo = static_cast<QComboBox*>(ui.AnglesTable->cellWidget(row, 1));
	combo->setCurrentIndex(masterAngle->form());

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
			ui.AnglesTable->setItem(row, n+2, item);
		}
		else item = ui.AnglesTable->item(row, n+2);
		item->setText(QString::number(masterAngle->parameter(n)));
	}
}

// Row update function for TorsionsTable
void MasterTermsWidget::updateTorsionsTableRow(int row, MasterIntra* masterTorsion, bool createItems)
{
	QTableWidgetItem* item;
	QComboBox* combo;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		ui.TorsionsTable->setItem(row, 0, item);
	}
	else item = ui.TorsionsTable->item(row, 0);
	item->setText(masterTorsion->name());

	// Form
	if (createItems)
	{
		combo = new QComboBox;
		QStringList forms;
		for (int n=0; n<SpeciesTorsion::nTorsionFunctions; ++n) forms << SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) n);
		combo->addItems(forms);
		combo->setItemData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(torsionFunctionChanged(int)));
		ui.TorsionsTable->setCellWidget(row, 1, combo);
	}
	else combo = static_cast<QComboBox*>(ui.TorsionsTable->cellWidget(row, 1));
	combo->setCurrentIndex(masterTorsion->form());

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
			ui.TorsionsTable->setItem(row, n+2, item);
		}
		else item = ui.TorsionsTable->item(row, n+2);
		item->setText(QString::number(masterTorsion->parameter(n)));
	}
}

void MasterTermsWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(this));
}

// Update controls within widget
void MasterTermsWidget::updateControls()
{
	refreshing_ = true;

	ui.BottomLabel->setText(CharString("%i Bonds, %i Angles, %i Torsions", duq_.nMasterBonds(), duq_.nMasterAngles(), duq_.nMasterTorsions()).get());

	// Bonds Table
	TableWidgetUpdater<MasterTermsWidget,MasterIntra,MasterTermsWidget> bondsUpdater(ui.BondsTable, duq_.masterBonds(), this, &MasterTermsWidget::updateBondsTableRow);

	// Angles Table
	TableWidgetUpdater<MasterTermsWidget,MasterIntra,MasterTermsWidget> anglesUpdater(ui.AnglesTable, duq_.masterAngles(), this, &MasterTermsWidget::updateAnglesTableRow);

	// Torsions Table
	TableWidgetUpdater<MasterTermsWidget,MasterIntra,MasterTermsWidget> torsionsUpdater(ui.TorsionsTable, duq_.masterTorsions(), this, &MasterTermsWidget::updateTorsionsTableRow);

	refreshing_ = false;
}


// Disable sensitive controls within widget, ready for main code to run
void MasterTermsWidget::disableSensitiveControls()
{
	ui.BondsTable->setEnabled(false);
	ui.AnglesTable->setEnabled(false);
	ui.TorsionsTable->setEnabled(false);
}

// Enable sensitive controls within widget, ready for main code to run
void MasterTermsWidget::enableSensitiveControls()
{
	ui.BondsTable->setEnabled(true);
	ui.AnglesTable->setEnabled(true);
	ui.TorsionsTable->setEnabled(true);
}

// Return string specifying widget type
const char* MasterTermsWidget::widgetType()
{
	return "MasterTerms";
}

// Write widget state through specified LineParser
bool MasterTermsWidget::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool MasterTermsWidget::readState(LineParser& parser)
{
	return true;
}

/*
 * Widget Slots
 */

void MasterTermsWidget::bondFunctionChanged(int index)
{
	if (refreshing_) return;

	// Cast the sender into our QComboBox
	QComboBox* combo = (QComboBox*) sender();

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = combo ? VariantPointer<MasterIntra>(combo->itemData(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	masterIntra->setForm((SpeciesBond::BondFunction) index);
}

void MasterTermsWidget::on_BondsTable_itemChanged(QTableWidgetItem* w)
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
			break;
		// Form of interaction
		case (1):
			// Handled by bondFunctionChanged()
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column() - 2, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of BondsTable.\n", w->column());
			break;
	}
}

void MasterTermsWidget::on_NewBondTermButton_clicked(bool checked)
{
}

void MasterTermsWidget::on_RemoveBondTermButton_clicked(bool checked)
{
}

void MasterTermsWidget::angleFunctionChanged(int index)
{
	if (refreshing_) return;

	// Cast the sender into our QComboBox
	QComboBox* combo = (QComboBox*) sender();

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = combo ? VariantPointer<MasterIntra>(combo->itemData(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	masterIntra->setForm((SpeciesAngle::AngleFunction) index);
}

void MasterTermsWidget::on_AnglesTable_itemChanged(QTableWidgetItem* w)
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
			break;
		// Form of interaction
		case (1):
			// Handled by angleFunctionChanged()
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column() - 2, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of AnglesTable.\n", w->column());
			break;
	}
}

void MasterTermsWidget::on_NewAngleTermButton_clicked(bool checked)
{
}

void MasterTermsWidget::on_RemoveAngleTermButton_clicked(bool checked)
{
}

void MasterTermsWidget::torsionFunctionChanged(int index)
{
	if (refreshing_) return;

	// Cast the sender into our QComboBox
	QComboBox* combo = (QComboBox*) sender();

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = combo ? VariantPointer<MasterIntra>(combo->itemData(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	masterIntra->setForm((SpeciesTorsion::TorsionFunction) index);
}

void MasterTermsWidget::on_TorsionsTable_itemChanged(QTableWidgetItem* w)
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
			break;
		// Form of interaction
		case (1):
			// Handled by torsionFunctionChanged()
			break;
		// Parameters
		case (2):
		case (3):
		case (4):
		case (5):
			masterIntra->setParameter(w->column() - 2, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of TorsionsTable.\n", w->column());
			break;
	}
}

void MasterTermsWidget::on_NewTorsionTermButton_clicked(bool checked)
{
}

void MasterTermsWidget::on_RemoveTorsionTermButton_clicked(bool checked)
{
}
