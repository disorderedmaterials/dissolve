/*
	*** MasterTerms Widget - Functions
	*** src/gui/mastertermswidget_funcs.cpp
	Copyright T. Youngs 2007-2018

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
#include "main/duq.h"
#include "classes/masterintra.h"
#include "classes/speciesbond.h"
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

	// Update window contents
	updateControls();

	refreshing_ = false;
}

MasterTermsWidget::~MasterTermsWidget()
{
}

/*
 * SubWidget Reimplementations / Definitions
 */

void MasterTermsWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(this));
}

// Update controls within widget
void MasterTermsWidget::updateControls()
{
	refreshing_ = true;

	QTableWidgetItem* item;
	QComboBox* combo;

	// Our table may or may not be populated, and with different terms to those in the list.
	int currentRow = 0;
	for (MasterIntra* masterBond = duq_.masterBonds(); masterBond != NULL; masterBond = masterBond->next)
	{
		// If there is an item already on this row, check it
		// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
		while (currentRow < ui.BondsTable->rowCount())
		{
			item = ui.BondsTable->item(currentRow, 0);
			MasterIntra* rowData = (item ? VariantPointer<MasterIntra>(item->data(Qt::UserRole)) : NULL);
			if (rowData == masterBond)
			{
				// Update the current row
				item = ui.BondsTable->item(currentRow, 0);
				if (item) item->setText(masterBond->name());

				break;
			}
			else ui.BondsTable->removeRow(currentRow);
		}

		// If the current row index is (now) out of range, add a new row to the table
		if (currentRow == ui.BondsTable->rowCount())
		{
			// Increase row count
			ui.BondsTable->setRowCount(currentRow+1);

			/*
			 * Create new items at the current row index
			 */
			// Name
			item = new QTableWidgetItem(masterBond->name());
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
			ui.BondsTable->setItem(currentRow, 0, item);

			// Form
			combo = new QComboBox;
			QStringList forms;
			for (int n=0; n<SpeciesBond::nBondFunctions; ++n) forms << SpeciesBond::bondFunction( (SpeciesBond::BondFunction) n);
			combo->addItems(forms);
			combo->setItemData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
			QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(bondFunctionChanged(int)));
			ui.BondsTable->setCellWidget(currentRow, 1, combo);

			// Parameters
			for (int n=0; n<MAXINTRAPARAMS; ++n)
			{
				item = new QTableWidgetItem(QString::number(masterBond->parameter(n)));
				item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
				ui.BondsTable->setItem(currentRow, n+2, item);
			}

			// Increase counter
			++currentRow;
		}
	}

	// Finally, we set make sure the number of rows in the table matches the number of terms in the list (any extras will thus be deleted)
	ui.BondsTable->setRowCount(duq_.nMasterBonds());

	refreshing_ = false;
}


// Disable sensitive controls within widget, ready for main code to run
void MasterTermsWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void MasterTermsWidget::enableSensitiveControls()
{
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
	// Cast the sender into our QComboBox
	QComboBox* combo = (QComboBox*) sender();

	// Get target MasterIntra from the passed widget
	MasterIntra* masterIntra = combo ? VariantPointer<MasterIntra>(combo->itemData(Qt::UserRole)) : NULL;
	if (!masterIntra) return;

	masterIntra->setForm((SpeciesBond::BondFunction) index);
}

void MasterTermsWidget::on_BondsTable_itemChanged(QTableWidgetItem* w)
{
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
