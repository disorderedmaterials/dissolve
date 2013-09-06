/*
	*** AtomTypes Dialog
	*** src/gui/samples_funcs.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/atomtypes.h"
#include "gui/guiduq.uih"
#include "gui/dwenum.h"
#include "main/flags.h"
#include "classes/atomtype.h"
#include <QtGui/QComboBox>

// Constructor
DUQAtomTypesDockWidget::DUQAtomTypesDockWidget(QWidget* parent, DUQObject& duq) : QDockWidget(parent), dUQ_(duq)
{
	// Call the main creation function
	ui.setupUi(this);

	// Set initial variables
	refreshing_ = false;
}

// Destructor
DUQAtomTypesDockWidget::~DUQAtomTypesDockWidget()
{
}

// Window close event
void DUQAtomTypesDockWidget::closeEvent(QCloseEvent* event)
{
}

// General refresh function
void DUQAtomTypesDockWidget::refresh(int targets)
{
	refreshing_ = true;

	// Refresh the Atom Types group?
	if (targets&DockWidgetPanel::AtomTypesPanel)
	{
		// Re-populate list
		ui.DefinedAtomTypesTree->clear();

		int count = 0;
		QTreeWidgetItem* item;
		QComboBox* combo;
		Parameters* params;

		for (AtomType* at = dUQ_.atomTypes(); at != NULL; at = at->next)
		{
		
			item = new QTreeWidgetItem(ui.DefinedAtomTypesTree);

			item->setData(0, Qt::UserRole, QVariant(count));
			item->setText(0, at->name());
			item->setText(1, PeriodicTable::element(at->element()).symbol());
			
			// Create a combo box with the available Parameters for this AtomType
			combo = new QComboBox(this);
			combo->setMinimumSize(100,20);
			for (params = PeriodicTable::element(at->element()).parameters(); params != NULL; params = params->next)
			{
				combo->addItem(params->description());
				if (params == at->parameters()) combo->setCurrentIndex(combo->count()-1);
			}
			combo->setItemData(0, QVariant(count));
			ui.DefinedAtomTypesTree->setItemWidget(item, 2, combo);
			QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(atomTypeParametersChanged(int)));

			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			++count;
		}

		for (int n = 0; n < 3; ++n) ui.DefinedAtomTypesTree->resizeColumnToContents(n);
		
		// Change count label
		ui.NAtomTypesLabel->setText("NAtomTypes = " + QString::number(dUQ_.nAtomTypes()));

		// Set control button states
		AtomType* at = selectedAtomType();
		ui.CopyAtomTypeButton->setEnabled(at);
		ui.RemoveAtomTypeButton->setEnabled(at);
	}


	refreshing_ = false;
}

/*
 * Private Functions
 */

// Return current selected AtomType, if any
AtomType* DUQAtomTypesDockWidget::selectedAtomType()
{
	QTreeWidgetItem* item = ui.DefinedAtomTypesTree->currentItem();
	if (item == NULL) return NULL;

	QVariant data = item->data(0, Qt::UserRole);
	if (!data.isValid()) return NULL;
	
	AtomType* at = dUQ_.atomType(data.toInt());
	return at;
}

/*
 * Slots
 */

// Combo slot
void DUQAtomTypesDockWidget::atomTypeParametersChanged(int index)
{
	if (refreshing_) return;

	// Data associated to column 0 gives us the AtomType index
	QComboBox *combo = static_cast<QComboBox*>(sender());
	if (!combo) return;

	QVariant data = combo->itemData(0, Qt::UserRole);
	if (!data.isValid()) return;
	
	
	AtomType* at = dUQ_.atomType(data.toInt());
	if (at == NULL) return;
	
	at->setParameters(PeriodicTable::element(at->element()).parameters(index));

	// Update
	dUQ_.updateAtomTypes();
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SpeciesViewPanel));
}

// Item clicked in AtomTypesTree
void DUQAtomTypesDockWidget::on_DefinedAtomTypesTree_itemClicked(QTreeWidgetItem* item, int column)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	ui.CopyAtomTypeButton->setEnabled(at);
	ui.RemoveAtomTypeButton->setEnabled(at);
}

// Item edited in AtomTypesTree
void DUQAtomTypesDockWidget::on_DefinedAtomTypesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (refreshing_) return;

	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	// Which column was edited?
	if (column == 0) at->setName(dUQ_.uniqueAtomTypeName(qPrintable(item->text(column)), at));
	else if (column == 1)
	{
		// Element of AtomType changed...
		int el = PeriodicTable::find(qPrintable(item->text(1)));
		if (el == -1) msg.print("Ignored AtomType edit - '%s' is not a valid chemical element.\n", qPrintable(item->text(1)));
		else
		{
			// Safest option here is to copy the existing AtomType data, remove the existing AtomType,
			// and then add a new one, updating the AtomTypes list afterwards.
			AtomType oldType;
			oldType.setName(at->name());
			oldType.setParameters(at->parameters());
			dUQ_.removeAtomType(at);
			at = dUQ_.addAtomType(el);
			at->setName(oldType.name());
			at->setParameters(oldType.parameters());
			
			dUQ_.updateAtomTypes();
			dUQ_.updateIsotopologues();
		}
	}

	// Update
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SpeciesViewPanel | DockWidgetPanel::SpeciesIsotopologuesPanel));
}

// Add AtomType Button
void DUQAtomTypesDockWidget::on_AddAtomTypeButton_clicked(bool checked)
{
	AtomType* newType = dUQ_.addAtomType(1);

	// Update
	refresh(65535);
}

// Copy AtomType definition
void DUQAtomTypesDockWidget::on_CopyAtomTypeButton_clicked(bool checked)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	// Add a copy of this AtomType, changing the name...
	AtomType* newType = dUQ_.addAtomType(at->element());
	newType->setParameters(at->parameters());

	// Update
	refresh(65535);
}

// Remove AtomType definition
void DUQAtomTypesDockWidget::on_RemoveAtomTypeButton_clicked(bool checked)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	dUQ_.removeAtomType(at);

	// Update
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SpeciesViewPanel | DockWidgetPanel::SpeciesIsotopologuesPanel));
}
