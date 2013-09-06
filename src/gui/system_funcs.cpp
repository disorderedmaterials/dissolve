/*
	*** System Dialog
	*** src/gui/system_funcs.cpp
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

#include "gui/system.h"
#include "gui/guiduq.uih"
#include "gui/dwenum.h"
#include "main/flags.h"
#include "classes/species.h"

// Constructor
DUQSystemDockWidget::DUQSystemDockWidget(QWidget* parent, DUQObject& duq) : QDockWidget(parent), dUQ_(duq)
{
	// Call the main creation function
	ui.setupUi(this);

	// Setup item delegates
	// -- Composition Table
	speciesRelativePopulationDelegate_ = new DoubleSpinDelegate(this, 0.0, 1e6, 1.0, 3);
	ui.CompositionTable->setItemDelegateForColumn(1, speciesRelativePopulationDelegate_);

	// Set initial variables
	refreshing_ = false;
}

// Destructor
DUQSystemDockWidget::~DUQSystemDockWidget()
{
}

// Window close event
void DUQSystemDockWidget::closeEvent(QCloseEvent* event)
{
}

// General refresh function
void DUQSystemDockWidget::refresh(int targets)
{
	refreshing_ = true;

	// Re-populate list
	if (targets&DockWidgetPanel::SystemCompositionPanel)
	{
		// Grab current selected item
		int oldRow = ui.CompositionTable->currentRow();

		// Get total 'relative' population of components and multiplier
		double totalRelative = dUQ_.totalRelative();
		int multiplier = dUQ_.multiplier();

		QStringList headers;
		headers << "Species" << "Relative Population" << "Fractional Population" << "NMolecules";
		ui.CompositionTable->setHorizontalHeaderLabels(headers);
		ui.CompositionTable->horizontalHeader()->setVisible(true);
		ui.CompositionTable->clearContents();
		ui.CompositionTable->setRowCount(dUQ_.nSpecies());

		QTableWidgetItem* item;
		int count = 0, atomCount = 0, moleculeCount = 0;

		for (Species* sp = dUQ_.species(); sp != NULL; sp = sp->next)
		{
			item = new QTableWidgetItem(sp->name());
			item->setFlags(Qt::ItemIsSelectable);
			ui.CompositionTable->setItem(count, 0, item);
			item = new QTableWidgetItem(QString::number(sp->relativePopulation()));
			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			ui.CompositionTable->setItem(count, 1, item);
			item = new QTableWidgetItem(QString::number(sp->relativePopulation()/totalRelative));
			item->setFlags(Qt::ItemIsSelectable);
			ui.CompositionTable->setItem(count, 2, item);
			item = new QTableWidgetItem(QString::number(int(sp->relativePopulation()*multiplier)));
			item->setFlags(Qt::ItemIsSelectable);
			ui.CompositionTable->setItem(count, 3, item);
			
			// Update counters
			++count;
			moleculeCount += multiplier*sp->relativePopulation();
			atomCount += multiplier*sp->relativePopulation()*sp->nAtoms();
		}
		ui.CompositionTable->resizeColumnsToContents();

		// Re-select the same index as before (if possible)
		if ((oldRow != -1) && (oldRow < ui.CompositionTable->rowCount())) ui.CompositionTable->setCurrentCell(oldRow,0);
		else ui.CompositionTable->setCurrentCell(0,0);
		
		// Change count label
		ui.NComponentsLabel->setText("System Composition (" + QString::number(ui.CompositionTable->rowCount()) + " Species)");
	
		// Update mutiplier and density
		ui.MultiplierSpin->setValue(dUQ_.multiplier());
		ui.DensitySpin->setValue(dUQ_.density());
		ui.DensityUnitsCombo->setCurrentIndex(dUQ_.densityIsAtomic() ? 0 : 1);
		
		// Update system info text
		ui.TotalAtomsLabel->setText(QString::number(atomCount));
		ui.TotalMoleculesLabel->setText(QString::number(moleculeCount));

		// Set enabled status of some components
		ui.CompositionTable->setEnabled(ui.CompositionTable->rowCount());
	}

	refreshing_ = false;
}

/*
 * Private Functions
 */

/*
 * Slots
 */

// Different Component selected in list
void DUQSystemDockWidget::on_CompositionTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (refreshing_) return;
	refresh(65535);
}

// Component data changed
void DUQSystemDockWidget::on_CompositionTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// We need to keep an eye on column '1' which is the relative population
	if (column == 1)
	{
		// The row corresponds to the component Species
		Species* sp = dUQ_.species(row);
		if (sp == NULL) return;
		
		// Now grab sender widget and value
		QTableWidgetItem* item = ui.CompositionTable->item(row, column);
		if (item == NULL)
		{
			printf("GUI_ERROR - Row and column don't correspond to a QWidgetItem.\n");
			return;
		}
		sp->setRelativePopulation(atof(qPrintable(item->text())));
	}

	// Update
	refresh(65535);
}

// Set system multiplier
void DUQSystemDockWidget::on_MultiplierSpin_valueChanged(int value)
{
	if (refreshing_) return;

	dUQ_.setMultiplier(value);
	
	// Update
	refresh(65535);
}

// Set system density
void DUQSystemDockWidget::on_DensitySpin_valueChanged(double value)
{
	if (refreshing_) return;
	
	// Need current index of units combo
	if (ui.DensityUnitsCombo->currentIndex() == 0) dUQ_.setAtomicDensity(value);
	else dUQ_.setAtomicDensity(value);
}

// Density units changed
void DUQSystemDockWidget::on_DensityUnitsCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;
	
	if (ui.DensityUnitsCombo->currentIndex() == 0) dUQ_.setAtomicDensity(dUQ_.density());
	else dUQ_.setAtomicDensity(dUQ_.density());	
}

// Box relative length A changed
void DUQSystemDockWidget::on_BoxRelativeASpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> lengths(value, ui.BoxRelativeBSpin->value(), ui.BoxRelativeCSpin->value());
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box relative length B changed
void DUQSystemDockWidget::on_BoxRelativeBSpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> lengths(ui.BoxRelativeASpin->value(), value, ui.BoxRelativeCSpin->value());
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box relative length C changed
void DUQSystemDockWidget::on_BoxRelativeCSpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> lengths(ui.BoxRelativeASpin->value(), ui.BoxRelativeBSpin->value(), value);
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box angle alpha changed
void DUQSystemDockWidget::on_BoxAlphaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> angles(value, ui.BoxBetaSpin->value(), ui.BoxGammaSpin->value());
	dUQ_.setBoxAngles(angles);
}

// Box angle beta changed
void DUQSystemDockWidget::on_BoxBetaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> angles(ui.BoxAlphaSpin->value(), value, ui.BoxGammaSpin->value());
	dUQ_.setBoxAngles(angles);
}

// Box angle gamma changed
void DUQSystemDockWidget::on_BoxGammaSpin_valueChanged(double value)
{
	if (refreshing_) return;
	Vec3<double> angles(ui.BoxAlphaSpin->value(), ui.BoxBetaSpin->value(), value);
	dUQ_.setBoxAngles(angles);
}
