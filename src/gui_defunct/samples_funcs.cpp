/*
	*** Samples Dialog
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

#include "gui/samples.h"
#include "gui/guiduq.uih"
#include "gui/dwenum.h"
#include "main/flags.h"
#include "classes/species.h"
#include <QtGui/QFileDialog>

// Constructor
DUQSamplesDockWidget::DUQSamplesDockWidget(QWidget* parent, DUQObject& duq) : QDockWidget(parent), dUQ_(duq)
{
	// Call the main creation function
	ui.setupUi(this);

	// Setup item delegates
	// -- Assigned Isotopologues Table
	isotopologueRelativePopulationDelegate_ = new DoubleSpinDelegate(this, 0.0, 1000, 1.0, 3);
	ui.AssignedIsotopologuesTree->setItemDelegateForColumn(2, isotopologueRelativePopulationDelegate_);
	
	// Set initial variables
	refreshing_ = false;
}

// Destructor
DUQSamplesDockWidget::~DUQSamplesDockWidget()
{
}

// Window close event
void DUQSamplesDockWidget::closeEvent(QCloseEvent* event)
{
}

// General refresh function
void DUQSamplesDockWidget::refresh(int targets)
{
	refreshing_ = true;

	// Refresh the Sample list
	if (targets&DockWidgetPanel::SampleListPanel)
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
		ui.NSamplesLabel->setText("Samples (" + QString::number(ui.SamplesList->count()) + ")");
	}
	
	// Get currently-selected Sample
	Sample* currentSample = ui.SamplesList->currentRow() == -1 ? NULL : dUQ_.sample(ui.SamplesList->currentRow());
	ui.RemoveSampleButton->setEnabled(currentSample);
	
	// Refresh the Sample Isotopologue Mix table
	if (targets&DockWidgetPanel::SampleIsotopologuesPanel)
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
					item->setText(2, QString::number(ri->data));
					item->setText(3, QString::number(ri->data / totalRelative));
					item->setText(4, QString::number(int(ri->data / totalRelative)*mix->species()->relativePopulation()*multiplier));
					item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				}
			}

			// Resize columns to fit contents
			for (int n=0; n<5; ++n) ui.AssignedIsotopologuesTree->resizeColumnToContents(n);
		}
	}

	// Update reference file data
	if (targets&DockWidgetPanel::SampleReferenceDataPanel)
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

	refreshing_ = false;
}

/*
 * Private Functions
 */

// Return pointer to selected Sample
Sample* DUQSamplesDockWidget::selectedSample()
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

// Determine Species and Isotopologue from QVariant data
bool DUQSamplesDockWidget::getSpeciesAndIsotopologue(QStringList data, Species*& species, Isotopologue*& iso)
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

/*
 * Slots
 */

// Different Sample selected in list
void DUQSamplesDockWidget::on_SamplesList_currentRowChanged(int row)
{
	if (refreshing_) return;

	// Update
	refresh(65535);
}

// Item changed (i.e. text changed)
void DUQSamplesDockWidget::on_SamplesList_itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;
	
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
void DUQSamplesDockWidget::on_AddSampleButton_clicked(bool checked)
{
	Sample* sample = dUQ_.addSample(dUQ_.uniqueSampleName("NewSample"));
	sample->assignDefaultIsotopes();

	// Update
	refresh(65535);
}

// Remove Sample from system
void DUQSamplesDockWidget::on_RemoveSampleButton_clicked(bool checked)
{
	Sample *currentSample = selectedSample();
	if (currentSample == NULL) return;
	
	dUQ_.removeSample(currentSample);
	
	// Update
	refresh(65535);
}

// Copy Sample
void DUQSamplesDockWidget::on_CopySampleButton_clicked(bool checked)
{
	int currentRow = ui.SamplesList->currentRow();
	Sample *currentSample = (currentRow == -1 ? NULL : dUQ_.sample(currentRow));
	if (currentSample == NULL) return;
	
	// TODO
	
	// Update
	refresh(65535);
}

/*
 * Mixture Group
 */

// Something changed in the mixtures tree
void DUQSamplesDockWidget::on_AssignedIsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (refreshing_) return;

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
	refresh(65535);
}

// Add Isotopologue to mixture
void DUQSamplesDockWidget::on_AddIsotopologueToMixButton_clicked(bool checked)
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
	refresh(65535);
}

// Remove Isotopologue from mix
void DUQSamplesDockWidget::on_RemoveIsotopologueFromMixButton_clicked(bool checked)
{
	//TODO
}

// Different Isotopologue selected for mixture
void DUQSamplesDockWidget::mixtureIsotopologueChanged(int index)
{
	if (refreshing_) return;

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
	refresh(65535);
}

/*
 * Reference Data Group
 */

// Raise a file-selector and choose a new reference data filename
void DUQSamplesDockWidget::on_ReferenceDataFileSelectButton_clicked(bool checked)
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

	Flags::wave(Flags::ReferenceDataChanged);
	refresh(65535);
}

// Reference data filename changed
void DUQSamplesDockWidget::on_ReferenceDataFilenameEdit_textEdited(const QString& text)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->referenceDataFileName() = qPrintable(text);

	Flags::wave(Flags::ReferenceDataChanged);
	refresh(65535);
}

// Reference data normalisation type changed
void DUQSamplesDockWidget::on_ReferenceDataNormalisationComboCheck_currentIndexChanged(int index)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->setReferenceDataNormalisation( (Sample::NormalisationType) index);

	Flags::wave(Flags::ReferenceDataChanged);
	refresh(65535);
}

// Reference data self-subtraction status changed
void DUQSamplesDockWidget::on_ReferenceDataSubtractSelfCheck_clicked(bool checked)
{
	Sample* sample = selectedSample();
	if (!sample) return;

	// Set data
	sample->setReferenceSubtractSelf(checked);

	Flags::wave(Flags::ReferenceDataChanged);
	refresh(65535);
}

