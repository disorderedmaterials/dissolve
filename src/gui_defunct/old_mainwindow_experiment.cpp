/*
	*** Main Window - ExperimentTab Functions
	*** src/gui/mainwindow_experiment.cpp
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
#include "gui/pointerdialog.h"
#include "classes/species.h"
#include "base/sysfunc.h"

// Refresh tab
void MainWindow::refreshExperimentTab(bool compositionGroup, bool sampleGroup, bool mixGroup, bool refDataGroup)
{
	// Set the refreshing_ flag
	experimentTabRefreshing_ = true;

	// Refresh the Component group?
	if (compositionGroup)
	{
		// Grab current selected item
		int oldRow = ui.SystemCompositionTable->currentRow();

		// Get total 'relative' population of components and multiplier
		double totalRelative = dUQ_.totalRelative();
		int multiplier = dUQ_.multiplier();

		// Re-populate list
		QStringList headers;
		headers << "Species" << "Relative Population" << "Fractional Population" << "NMolecules";
		ui.SystemCompositionTable->setHorizontalHeaderLabels(headers);
		ui.SystemCompositionTable->horizontalHeader()->setVisible(true);
		ui.SystemCompositionTable->clearContents();
		ui.SystemCompositionTable->setRowCount(dUQ_.nSpecies());

		QTableWidgetItem* item;
		int count = 0, atomCount = 0, moleculeCount = 0;

		for (Species* sp = dUQ_.species(); sp != NULL; sp = sp->next)
		{
			item = new QTableWidgetItem(sp->name());
			item->setFlags(Qt::ItemIsSelectable);
			ui.SystemCompositionTable->setItem(count, 0, item);
			item = new QTableWidgetItem(ftoa(sp->relativePopulation()));
			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			ui.SystemCompositionTable->setItem(count, 1, item);
			item = new QTableWidgetItem(ftoa(sp->relativePopulation()/totalRelative));
			item->setFlags(Qt::ItemIsSelectable);
			ui.SystemCompositionTable->setItem(count, 2, item);
			item = new QTableWidgetItem(itoa(int(sp->relativePopulation()*multiplier)));
			item->setFlags(Qt::ItemIsSelectable);
			ui.SystemCompositionTable->setItem(count, 3, item);
			
			// Update counters
			++count;
			moleculeCount += multiplier*sp->relativePopulation();
			atomCount += multiplier*sp->relativePopulation()*sp->nAtoms();
		}
		ui.SystemCompositionTable->resizeColumnsToContents();

		// Re-select the same index as before (if possible)
		if ((oldRow != -1) && (oldRow < ui.SystemCompositionTable->rowCount())) ui.SystemCompositionTable->setCurrentCell(oldRow,0);
		else ui.SystemCompositionTable->setCurrentCell(0,0);
		
		// Change group title
		Dnchar title(-1, "System Composition (%i Species)", ui.SystemCompositionTable->rowCount());
		ui.SystemCompositionGroup->setTitle(title.get());
		
		// Update mutiplier and density
		ui.SystemMultiplierSpin->setValue(dUQ_.multiplier());
		ui.SystemDensitySpin->setValue(dUQ_.density());
		ui.SystemDensityUnitsCombo->setCurrentIndex(dUQ_.densityIsAtomic() ? 0 : 1);
		
		// Update system info text
		ui.SystemTotalAtomsLabel->setText(itoa(atomCount));
		ui.SystemTotalMoleculesLabel->setText(itoa(moleculeCount));
	}

	// Set enabled status of some components
	ui.SystemCompositionTable->setEnabled(ui.SystemCompositionTable->rowCount());

	// Refresh the Sample lists?
	if (sampleGroup)
	{
		// Grab current selected item
		int oldRow = ui.SamplesList->currentRow();

		// Re-populate list
		ui.SamplesList->clear();
		QListWidgetItem* item;
		int count = 0;
		for (Sample* s = dUQ_.samples(); s != NULL; s = s->next)
		{
			item = new QListWidgetItem(ui.SamplesList);
			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setText(s->name());
			item->setData(Qt::UserRole, QVariant(count));
			++count;
		}

		// Re-select the same index as before (if possible)
		if (oldRow == -1) oldRow = 0;
		else oldRow = (oldRow < ui.SamplesList->count() ? oldRow : ui.SamplesList->count()-1);
		ui.SamplesList->setCurrentRow(oldRow);
		
		// Change group title
		Dnchar title(-1, "Samples (%i)", ui.SamplesList->count());
		ui.SampleDefinitionsGroup->setTitle(title.get());
	}
	
	// Get currently-selected Sample
	Sample* currentSample = ui.SamplesList->currentRow() == -1 ? NULL : dUQ_.sample(ui.SamplesList->currentRow());
	ui.RemoveSampleButton->setEnabled(currentSample);
	
	// Refresh the Sample Isotopologue Mix table?
	if (mixGroup)
	{
		ui.AssignedIsotopologuesGroup->setEnabled(currentSample);

		// Re-populate list
		ui.AssignedIsotopologuesTree->clear();
		
		// Recreate headers
		ui.AssignedIsotopologuesTree->setColumnCount(5);
		QStringList headers;
		headers << "Species" << "Isotopologue" << "Relative" << "Fraction" << "nMolecules";
		ui.AssignedIsotopologuesTree->setHeaderLabels(headers);
		ui.AssignedIsotopologuesTree->header()->setVisible(true);
		ui.AssignedIsotopologuesTree->setHeaderHidden(false);
			
		if (currentSample != NULL)
		{
			QTreeWidgetItem* topItem, *item;
			double totalRelative;
			RefList<Isotopologue,int> isoList;
			QComboBox* combo;
			QStringList data;

			// Every item added to the list will *at least* have a QVariant with the Species name contained within it
			
			// Loop over defined IsotopologueMix items - these will constitute top-level items (and should match the composition)
			for (IsotopologueMix* mix = currentSample->isotopologueMixtures(); mix != NULL; mix = mix->next)
			{
				topItem = new QTreeWidgetItem(ui.AssignedIsotopologuesTree);
				topItem->setText(0, mix->species()->name());
				topItem->setExpanded(true);
				topItem->setFlags(Qt::ItemIsSelectable);
				data.clear();
				data << mix->species()->name();
				topItem->setData(0, Qt::UserRole, QVariant(data));
				int multiplier = dUQ_.multiplier() * mix->species()->relativePopulation();

				// Check for no Isotopologues in list...
				if (mix->nIsotopologues() == 0)
				{
					item = new QTreeWidgetItem(topItem);
					item->setText(0, "<< No Isotopologues Available >>");
					continue;
				}

				// Grab total relative population here, and a list of flagged Isotopologues for the Species
				totalRelative = mix->totalRelative();
				isoList.clear();
				for (Isotopologue* iso = mix->species()->isotopologues(); iso != NULL; iso = iso->next)
				{
					// Add the isotopologue to our temporary list, flagging whether it has been used in the mix?
					isoList.add(iso, mix->hasIsotopologue(iso) ? 1 : 0);
				}
				
				// Now cycle over Isotopologues defined in this mix
				for (RefListItem<Isotopologue,double>* ri = mix->isotopologues(); ri != NULL; ri = ri->next)
				{
					item = new QTreeWidgetItem(topItem);
					// Create a combo box with the current isotopologue and all unused isotopologues
					combo = new QComboBox(this);
					combo->setMinimumSize(100,20);
					for (RefListItem<Isotopologue,int>* rj = isoList.first(); rj != NULL; rj = rj->next)
					{
						// QVariant data will contain the component Species and current Isotopologue names
						data.clear();
						data << mix->species()->name();
						data << ri->item->name();
						if (rj->data == 0) combo->addItem(rj->item->name(), QVariant(data));
						else if (rj->item == ri->item)
						{
							combo->addItem(rj->item->name());
							combo->setCurrentIndex(combo->count()-1);
							combo->setItemData(combo->count()-1, QVariant(data));
						}
					}
					ui.AssignedIsotopologuesTree->setItemWidget(item, 1, combo);
					QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(mixtureIsotopologueChanged(int)));

					item->setData(0, Qt::UserRole, QVariant(data));
					item->setText(2, ftoa(ri->data, "%6.3f"));
					item->setText(3, ftoa(ri->data / totalRelative, "%6.3f"));
					item->setText(4, itoa(int(ri->data / totalRelative)*mix->species()->relativePopulation()*multiplier));
					item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				}
			}

			// Resize columns to fit contents
			for (int n=0; n<5; ++n) ui.AssignedIsotopologuesTree->resizeColumnToContents(n);
		}
	}

	// Update reference file data?
	if (refDataGroup)
	{
		ui.ReferenceDataGroup->setEnabled(currentSample);
		if (currentSample && (!currentSample->referenceDataFileName().isEmpty()))
		{
			ui.ReferenceDataFilenameEdit->setText(currentSample->referenceDataFileName().get());
			ui.ReferenceDataNormalisationCombo->setCurrentIndex(currentSample->referenceDataNormalisation());
			ui.ReferenceDataSubtractSelfCheck->setChecked(currentSample->referenceSubtractSelf());
		}
		else
		{
			ui.ReferenceDataFilenameEdit->clear();
		}
	}

	// Unset the refreshing_ flag
	experimentTabRefreshing_ = false;
}

// Return pointer to selected Sample
Sample* MainWindow::selectedSample()
{
	int currentRow = ui.SamplesList->currentRow();
	if (currentRow == -1) return NULL;
	Sample* sample = dUQ_.sample(currentRow);
	if (!sample)
	{
		printf("GUI_ERROR - Failed to get current Sample.\n");
		return NULL;
	}
	return sample;
}

/*
// System Component Group
*/

// Different Component selected in list
void MainWindow::on_SystemCompositionTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (experimentTabRefreshing_) return;
	refreshExperimentTab(false, false, false);
}

// Component data changed
void MainWindow::on_SystemCompositionTable_cellChanged(int row, int column)
{
	if (experimentTabRefreshing_) return;

	// We need to keep an eye on column '1' which is the relative population
	if (column == 1)
	{
		// The row corresponds to the component Species
		Species* sp = dUQ_.species(row);
		if (sp == NULL) return;
		
		// Now grab sender widget and value
		QTableWidgetItem* item = ui.SystemCompositionTable->item(row, column);
		if (item == NULL)
		{
			printf("GUI_ERROR - Row and column don't correspond to a QWidgetItem.\n");
			return;
		}
		sp->setRelativePopulation(atof(qPrintable(item->text())));
	}

	// Update
	refresh(SystemCompositionGroup | AssignedIsotopologuesGroup);
}

// Set system multiplier
void MainWindow::on_SystemMultiplierSpin_valueChanged(int value)
{
	if (experimentTabRefreshing_) return;

	dUQ_.setMultiplier(value);
	
	// Update
	refresh(SystemCompositionGroup | AssignedIsotopologuesGroup);
}

// Set system density
void MainWindow::on_SystemDensitySpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	
	// Need current index of units combo
	if (ui.SystemDensityUnitsCombo->currentIndex() == 0) dUQ_.setAtomicDensity(value);
	else dUQ_.setAtomicDensity(value);
}

// Density units changed
void MainWindow::on_SystemDensityUnitsCombo_currentIndexChanged(int index)
{
	if (experimentTabRefreshing_) return;
	
	if (ui.SystemDensityUnitsCombo->currentIndex() == 0) dUQ_.setAtomicDensity(dUQ_.density());
	else dUQ_.setAtomicDensity(dUQ_.density());	
}

// Box relative length A changed
void MainWindow::on_SystemBoxRelativeASpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> lengths(value, ui.SystemBoxRelativeBSpin->value(), ui.SystemBoxRelativeCSpin->value());
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box relative length B changed
void MainWindow::on_SystemBoxRelativeBSpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> lengths(ui.SystemBoxRelativeASpin->value(), value, ui.SystemBoxRelativeCSpin->value());
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box relative length C changed
void MainWindow::on_SystemBoxRelativeCSpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> lengths(ui.SystemBoxRelativeASpin->value(), ui.SystemBoxRelativeBSpin->value(), value);
	dUQ_.setRelativeBoxLengths(lengths);
}

// Box angle alpha changed
void MainWindow::on_SystemBoxAlphaSpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> angles(value, ui.SystemBoxBetaSpin->value(), ui.SystemBoxGammaSpin->value());
	dUQ_.setBoxAngles(angles);
}

// Box angle beta changed
void MainWindow::on_SystemBoxBetaSpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> angles(ui.SystemBoxAlphaSpin->value(), value, ui.SystemBoxGammaSpin->value());
	dUQ_.setBoxAngles(angles);
}

// Box angle gamma changed
void MainWindow::on_SystemBoxGammaSpin_valueChanged(double value)
{
	if (experimentTabRefreshing_) return;
	Vec3<double> angles(ui.SystemBoxAlphaSpin->value(), ui.SystemBoxBetaSpin->value(), value);
	dUQ_.setBoxAngles(angles);
}

/*
// Samples Group
*/

// Different Sample selected in list
void MainWindow::on_SamplesList_currentRowChanged(int row)
{
	if (experimentTabRefreshing_) return;

	// Update
	refresh(AssignedIsotopologuesGroup+ReferenceDataGroup);
}

// Item changed (i.e. text changed)
void MainWindow::on_SamplesList_itemChanged(QListWidgetItem* item)
{
	if (experimentTabRefreshing_) return;
	
	// We previously stored the items integer index as a QVariant...
	int id = item->data(Qt::UserRole).toInt();
	Sample* s = dUQ_.sample(id);
	if (s == NULL)
	{
		printf("GUI_ERROR - Item in Samples list refers to an unknown item.\n");
		return;
	}
	
	// Check current text - if its the same, do nothing. If different, then set a (unique) version of it
	if (item->text() != s->name()) s->setName(dUQ_.uniqueSampleName(qPrintable(item->text()), s));
}

// Add Sample to system
void MainWindow::on_AddSampleButton_clicked(bool checked)
{
	Sample* sample = dUQ_.addSample(dUQ_.uniqueSampleName("NewSample"));
	sample->assignDefaultIsotopes();

	// Update
	refresh(SamplesGroup | AssignedIsotopologuesGroup | ReferenceDataGroup);
}

// Remove Sample from system
void MainWindow::on_RemoveSampleButton_clicked(bool checked)
{
	Sample *currentSample = selectedSample();
	if (currentSample == NULL) return;
	
	dUQ_.removeSample(currentSample);
	
	// Update
	refresh(SamplesGroup | AssignedIsotopologuesGroup | ReferenceDataGroup);
}

// Copy Sample
void MainWindow::on_CopySampleButton_clicked(bool checked)
{
	int currentRow = ui.SamplesList->currentRow();
	Sample *currentSample = (currentRow == -1 ? NULL : dUQ_.sample(currentRow));
	if (currentSample == NULL) return;
	
	// TODO
	
	// Update
	refresh(SamplesGroup | AssignedIsotopologuesGroup | ReferenceDataGroup);
}

/*
// Mixture Group
*/

// Determine Species and Isotopologue from QVariant data
bool MainWindow::getSpeciesAndIsotopologue(QStringList data, Species*& species, Isotopologue*& iso)
{
	if (data.count() != 2)
	{
		printf("GUI_ERROR - QStringList must contain exactly two items (Species and Isotopologue names).\n");
		return false;
	}
	
	// Determine Species
	species = dUQ_.findSpecies(qPrintable(data[0]));
	if (!species)
	{
		printf("GUI_ERROR - QVariant data specifies an unknown Species ('%s').\n", qPrintable(data[0]));
		return false;
	}
	
	// Now we have the species, determine the isotopologue...
	iso = species->findIsotopologue(qPrintable(data[1]));
	if (!iso)
	{
		printf("GUI_ERROR - QVariant data specifies an unknown Isotopologue ('%s').\n", qPrintable(data[1]));
		return false;
	}

	return true;
}

// Something changed in the mixtures tree
void MainWindow::on_AssignedIsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (experimentTabRefreshing_) return;

	// Column 0 is the isotopologue (handled by the combo callback) and 2 is the fractional population.
	// So, we only care about column 1 (the relative population)
	if (column != 1) return;

	// Same as for the Combo box, the QVariant attached to the QTreeWidgetItem contains species and isotopologue names
	QVariant variant = item->data(0, Qt::UserRole);
	if (!variant.isValid())
	{
		printf("GUI_ERROR - Failed to get QVariant data from QTreeWidgetItem.\n");
		return;
	}
	QStringList data = variant.toStringList();
	Species* species;
	Isotopologue* iso;
	if (!getSpeciesAndIsotopologue(data, species, iso)) return;
	Sample* sample = selectedSample();
	if (sample == NULL) return;

	// Grab new relative population
	double relPop = atof(qPrintable(item->text(1)));
	
	// Now set new data...
	sample->setIsotopologueInMixture(species, iso, relPop);
	
	// Update
	refresh(AssignedIsotopologuesGroup);
}

// Add Isotopologue to mixture
void MainWindow::on_AddIsotopologueToMixButton_clicked(bool checked)
{
	// Need to determine current component highlighted in the list
	QTreeWidgetItem* currentItem = ui.AssignedIsotopologuesTree->currentItem();
	if (currentItem == NULL) return;

	// The QVariant attached to the QTreeWidgetItem contains at least the Species name
	QVariant variant = currentItem->data(0, Qt::UserRole);
	if (!variant.isValid())
	{
		printf("GUI_ERROR - Failed to get QVariant data from QTreeWidgetItem.\n");
		return;
	}
	QStringList data = variant.toStringList();

	// Determine Species
	Species* species = dUQ_.findSpecies(qPrintable(data[0]));
	if (!species)
	{
		printf("GUI_ERROR - QVariant data specifies an unknown Species ('%s').\n", qPrintable(data[0]));
		return;
	}
	
	// Grab Sample
	Sample* sample = selectedSample();
	if (!sample) return;
	
	sample->addIsotopologueToMixture(species, NULL, 1.0);

	// Update
	refresh(AssignedIsotopologuesGroup);
}

// Remove Isotopologue from mix
void MainWindow::on_RemoveIsotopologueFromMixButton_clicked(bool checked)
{
	//TODO
}

// Different Isotopologue selected for mixture
void MainWindow::mixtureIsotopologueChanged(int index)
{
	if (experimentTabRefreshing_) return;

	// Need to get the QComboBox which sent the signal
	QComboBox *combo = static_cast<QComboBox*>(sender());
	if (!combo) return;
	
	// The QVariant data attached to the combo item contains Species and Isotopologue names
	QVariant variant = combo->itemData(index);
	if (!variant.isValid())
	{
		printf("GUI_ERROR - Failed to get QVariant data from QComboBox sender.\n");
		return;
	}
	QStringList data = variant.toStringList();
	Species* species;
	Isotopologue* iso;
	if (!getSpeciesAndIsotopologue(data, species, iso)) return;
	Sample* sample = selectedSample();
	if (!sample) return;
	
	// Get new Isotopologue pointer (from current combo text)
	Isotopologue* newIso = species->findIsotopologue(qPrintable(combo->currentText()));
	if (!newIso)
	{
		printf("GUI_ERROR - QComboBox text specifies an unknown Isotopologue ('%s').\n", qPrintable(combo->currentText()));
		return;
	}
	
	// Same Isotopologue?
	if (iso == newIso) return;
	
	sample->setIsotopologueInMixture(species, iso, -1.0, newIso);
	
	// Update
	refresh(AssignedIsotopologuesGroup);
}

/*
// Reference Data Group
*/

// Raise a file-selector and choose a new reference data filename
void MainWindow::on_ReferenceDataFileSelectButton_clicked(bool checked)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Create a file selector
	static QDir currentDir(getenv("PWD"));
	QString fileName = QFileDialog::getOpenFileName(this, "Open DataFile", currentDir.path(), "Gudrun processed files (*.mint01)");

	// Was a filename provided?
	if (fileName.isEmpty()) return;

	// Set data
	sample->referenceDataFileName() = qPrintable(fileName);
	ui.ReferenceDataFilenameEdit->setText(fileName);

	SET_MODIFIED;
	refresh();
}

// Reference data filename changed
void MainWindow::on_ReferenceDataFilenameEdit_textEdited(const QString& text)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->referenceDataFileName() = qPrintable(text);

	SET_MODIFIED;
	refresh();
}

// Reference data normalisation type changed
void MainWindow::on_ReferenceDataNormalisationComboCheck_currentIndexChanged(int index)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->setReferenceDataNormalisation( (Sample::NormalisationType) index);

	SET_MODIFIED;
	refresh();
}

// Reference data self-subtraction status changed
void MainWindow::on_ReferenceDataSubtractSelfCheck_clicked(bool checked)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->setReferenceSubtractSelf(checked);

	SET_MODIFIED;
	refresh();
}
