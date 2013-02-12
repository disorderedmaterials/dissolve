/*
	*** Main Window - PotentialTab Functions
	*** src/gui/mainwindow_potential.cpp
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

#include "gui/mainwindow.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "base/ptable.h"
#include "base/sysfunc.h"

// Refresh tab
void MainWindow::refreshPotentialTab(bool pairPotentialsGroup, bool pairPotentialsGraph, bool intraGroup)
{
	// Set the refreshing_ flag
	potentialTabRefreshing_ = TRUE;

	// Refresh the Pair Potentials Group?
	if (pairPotentialsGroup)
	{
		// Update spin controls
		ui.PairPotentialDeltaSpin->setValue(dUQ_.pairPotentialDelta());
		ui.PairPotentialRangeSpin->setValue(dUQ_.pairPotentialRange());
		ui.PairPotentialTruncationWidthSpin->setValue(dUQ_.pairPotentialTruncationWidth());
		
		// Clear the existing list
		ui.PairPotentialsTable->clearContents();
		ui.PairPotentialsTable->setRowCount(dUQ_.nPairPotentials());
		
		QTableWidgetItem* item;
		int count = 0;
		
		for (PairPotential* pot = dUQ_.pairPotentials(); pot != NULL; pot = pot->next)
		{
			item = new QTableWidgetItem(pot->atomTypeI() ? pot->atomTypeI()->name() : "???");
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.PairPotentialsTable->setItem(count, 0, item);
			item = new QTableWidgetItem(pot->atomTypeJ() ? pot->atomTypeJ()->name() : "???");
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			ui.PairPotentialsTable->setItem(count, 1, item);
			item = new QTableWidgetItem(PairPotential::pairPotentialType(pot->type()));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui.PairPotentialsTable->setItem(count, 2, item);
			item = new QTableWidgetItem(ftoa(pot->sigmaIJ()));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui.PairPotentialsTable->setItem(count, 3, item);
			item = new QTableWidgetItem(ftoa(pot->epsilonIJ()));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui.PairPotentialsTable->setItem(count, 4, item);
			item = new QTableWidgetItem(ftoa(pot->chargeI()));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui.PairPotentialsTable->setItem(count, 5, item);
			item = new QTableWidgetItem(ftoa(pot->chargeJ()));
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui.PairPotentialsTable->setItem(count, 6, item);
			++count;
		}
		
		// Resize columns to contents
		ui.PairPotentialsTable->resizeColumnsToContents();
	}

	// Refresh the Species Intramolecular lists
	if (intraGroup)
	{
		Species* currentSpecies = selectedSpecies();
		ui.SpeciesBondsTable->clearContents();
		ui.SpeciesAnglesTable->clearContents();

		if (currentSpecies)
		{
			QTableWidgetItem* item;
			int count;
			
			// Bonds
			count = 0;
			ui.SpeciesBondsTable->setRowCount(currentSpecies->nBonds());
			for (Bond* b = currentSpecies->bonds(); b != NULL; b = b->next)
			{
				item = new QTableWidgetItem(itoa(b->indexI()+1));
				ui.SpeciesBondsTable->setItem(count, 0, item);
				item = new QTableWidgetItem(itoa(b->indexJ()+1));
				ui.SpeciesBondsTable->setItem(count, 1, item);
				item = new QTableWidgetItem(ftoa(b->equilibrium()));
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
				ui.SpeciesBondsTable->setItem(count, 2, item);
				item = new QTableWidgetItem(ftoa(b->forceConstant()));
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
				ui.SpeciesBondsTable->setItem(count, 3, item);
				++count;
			}
			ui.SpeciesBondsTable->resizeColumnsToContents();
			
			// Angles
			count = 0;
			ui.SpeciesAnglesTable->setRowCount(currentSpecies->nAngles());
			for (Angle* a = currentSpecies->angles(); a != NULL; a = a->next)
			{
				item = new QTableWidgetItem(itoa(a->indexI()+1));
				ui.SpeciesAnglesTable->setItem(count, 0, item);
				item = new QTableWidgetItem(itoa(a->indexJ()+1));
				ui.SpeciesAnglesTable->setItem(count, 1, item);
				item = new QTableWidgetItem(itoa(a->indexK()+1));
				ui.SpeciesAnglesTable->setItem(count, 2, item);
				item = new QTableWidgetItem(ftoa(a->equilibrium()));
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
				ui.SpeciesAnglesTable->setItem(count, 3, item);
				item = new QTableWidgetItem(ftoa(a->forceConstant()));
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
				ui.SpeciesAnglesTable->setItem(count, 4, item);
				++count;
			}
			ui.SpeciesAnglesTable->resizeColumnsToContents();
		}
	}

	// Refresh the PairPotentials Graph?
	if (pairPotentialsGraph)
	{
		ui.PairPotentialsGraph->repaint();
	}
	
	// Unset the refreshing_ flag
	potentialTabRefreshing_ = FALSE;
}

// Return selected Potential
PairPotential* MainWindow::selectedPairPotential()
{
	int currentRow = ui.PairPotentialsTable->currentRow();
	if (currentRow == -1) return NULL;
	return dUQ_.pairPotential(currentRow);
}

// Update PairPotential Graph to display selected PairPotential
void MainWindow::updatePairPotentialGraph()
{
	ui.PairPotentialsGraph->removeAllDataSets();
	PairPotential* pot = selectedPairPotential();
	if (pot != NULL)
	{
		int index = dUQ_.indexOf(pot);
		ui.PairPotentialsGraph->addDataSet(pot->u(), "Energy", TRUE, 0, Qt::black, Qt::SolidLine, PlotData::SqrtModifier);
		ui.PairPotentialsGraph->addDataSet(pot->dU(), "Force", FALSE, 0, Qt::red, Qt::DashLine, PlotData::SqrtModifier);
	}
	refresh(PairPotentialsGraph);
}

/*
// Pair Potentials Group
*/

// Potential range changed
void MainWindow::on_PairPotentialRangeSpin_valueChanged(double value)
{
	if (potentialTabRefreshing_) return;

	dUQ_.setPairPotentialRange(value);
	dUQ_.regeneratePairPotentials();
	dUQ_.updateCheckPointData2D(DUQ::CheckPointU);
	dUQ_.updateCheckPointData2D(DUQ::CheckPointDU);

	updatePairPotentialGraph();
}

// Potential truncation width changed
void MainWindow::on_PairPotentialTruncationWidthSpin_valueChanged(double value)
{
	if (potentialTabRefreshing_) return;

	dUQ_.setPairPotentialTruncationWidth(value);
	dUQ_.regeneratePairPotentials();
	dUQ_.updateCheckPointData2D(DUQ::CheckPointU);
	dUQ_.updateCheckPointData2D(DUQ::CheckPointDU);

	updatePairPotentialGraph();
}

// Potential delta changed
void MainWindow::on_PairPotentialDeltaSpin_valueChanged(double value)
{
	if (potentialTabRefreshing_) return;

	dUQ_.setPairPotentialDelta(value);
	dUQ_.regeneratePairPotentials();
	dUQ_.updateCheckPointData2D(DUQ::CheckPointU);
	dUQ_.updateCheckPointData2D(DUQ::CheckPointDU);

	updatePairPotentialGraph();
}

// Different pair potential selected
void MainWindow::on_PairPotentialsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (potentialTabRefreshing_) return;

	updatePairPotentialGraph();
}

// Pair potential data changed
void MainWindow::on_PairPotentialsTable_cellChanged(int row, int column)
{
	if (potentialTabRefreshing_) return;
	
	PairPotential* pot = selectedPairPotential();
	if (pot == NULL) return;
	
	Dnchar text = qPrintable(ui.PairPotentialsTable->currentItem()->text());
	printf("Text is '%s'\n", text.get());
	switch (column)
	{
		// AtomType columns - not editable
		case (0):
		case (1):
			printf("Can't edit the AtomType in this table.\n");
			return;
			break;
		// PairPotential Type
		case (2):
			pot->setType( PairPotential::pairPotentialType(text.get()) );
			break;
		// SigmaIJ
		case (3):
			pot->setSigmaIJ( text.asDouble() );
			break;
		// EpsilonIJ
		case (4):
			pot->setEpsilonIJ( text.asDouble() );
			break;
		// Charge I
		case (5):
			pot->setChargeI( text.asDouble() );
			break;
		// Charge J
		case (6):
			pot->setChargeJ( text.asDouble() );
			break;
	}
	
	// Regenerate PairPotential and update graph
	dUQ_.regeneratePairPotential(pot);
	dUQ_.updateCheckPointData2D(DUQ::CheckPointU);
	dUQ_.updateCheckPointData2D(DUQ::CheckPointDU);

	updatePairPotentialGraph();
}

// Remove selected PairPotential
void MainWindow::on_RemovePairPotentialButton_clicked(bool changed)
{
	PairPotential* pot = selectedPairPotential();
	if (pot == NULL) return;
	
	dUQ_.removePairPotential(pot);
	refresh(PairPotentialsGroup);
}

// Add missing PairPotentials
void MainWindow::on_AddMissingPairPotentialsButton_clicked(bool changed)
{
	dUQ_.addMissingPairPotentials();
	refresh(PairPotentialsGroup);
}

/*
// Intramolecular Group
*/

// Add Bond definition to Species
void MainWindow::on_AddBondButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;

	// TODO
}

// Remove Bond definition from Species
void MainWindow::on_RemoveBondButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
}

void MainWindow::on_AddAngleButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
}

void MainWindow::on_RemoveAngleButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
}

void MainWindow::on_RegenerateTermsButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	currentSpecies->recalculateIntramolecular();
	
	refresh(MainWindow::IntramolecularTermsGroup);
}
