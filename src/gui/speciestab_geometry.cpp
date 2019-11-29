/*
	*** SpeciesTab Functions - Geometry
	*** src/gui/speciestab_geometry.cpp
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
// #include "gui/delegates/intraformcombo.hui"
// #include "gui/delegates/combolist.hui"
// #include "gui/delegates/integerspin.hui"
// #include "gui/delegates/isotopecombo.hui"
// #include "gui/delegates/exponentialspin.hui"
// #include "gui/delegates/null.h"
// #include "gui/getspeciesnamedialog.h"
#include "gui/selectforcefielddialog.h"
// #include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/tablewidgetupdater.h"
// #include "gui/helpers/treewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
// #include "classes/species.h"
// #include "classes/speciesbond.h"
// #include "data/fflibrary.h"
// #include <QListWidgetItem>
#include <QMessageBox>

/*
 * Private Functions
 */

// SpeciesAtomTable row update function
void SpeciesTab::updateAtomTableRow(int row, SpeciesAtom* speciesAtom, bool createItems)
{
	QTableWidgetItem* item;

	// Element
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui_.AtomTable->setItem(row, 0, item);
	}
	else item = ui_.AtomTable->item(row, 0);
	item->setText(speciesAtom->element()->name());
	item->setSelected(speciesAtom->isSelected());

	// AtomType
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui_.AtomTable->setItem(row, 1, item);
	}
	else item = ui_.AtomTable->item(row, 1);
	item->setText(speciesAtom->atomType() ? speciesAtom->atomType()->name() : "");
	item->setSelected(speciesAtom->isSelected());

	// Coordinates
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
			ui_.AtomTable->setItem(row, n+2, item);
		}
		else item = ui_.AtomTable->item(row, n+2);
		item->setText(QString::number(speciesAtom->r().get(n)));
		item->setSelected(speciesAtom->isSelected());
	}

	// Charge
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui_.AtomTable->setItem(row, 5, item);
	}
	else item = ui_.AtomTable->item(row, 5);
	item->setText(QString::number(speciesAtom->charge()));
	item->setSelected(speciesAtom->isSelected());
}

// Update atom table selection
void SpeciesTab::updateAtomTableSelection()
{
	Locker refreshLocker(refreshLock_);

	QTableWidgetItem* item;
	SpeciesAtom* i;

	// Set atom selection in table to reflect the atom data
	for (int n=0; n<ui_.AtomTable->rowCount(); ++n)
	{
		item = ui_.AtomTable->item(n, 0);
		i = VariantPointer<SpeciesAtom>(item->data(Qt::UserRole));

		if (i->isSelected()) for (int m=0; m<6; ++m) ui_.AtomTable->item(n,m)->setSelected(true);
		else for (int m=0; m<6; ++m) ui_.AtomTable->item(n,m)->setSelected(false);
	}
}

// BondTable row update function
void SpeciesTab::updateBondTableRow(int row, SpeciesBond* speciesBond, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<2; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
			ui_.BondTable->setItem(row, n, item);
		}
		else item = ui_.BondTable->item(row, n);
		item->setText(QString::number(speciesBond->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
		ui_.BondTable->setItem(row, 2, item);
	}
	else item = ui_.BondTable->item(row, 2);
	item->setText(speciesBond->masterParameters() ? QString("@%1").arg(speciesBond->masterParameters()->name()) : SpeciesBond::bondFunctions().keywordFromInt(speciesBond->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
			ui_.BondTable->setItem(row, n+3, item);
		}
		else item = ui_.BondTable->item(row, n+3);
		item->setText(QString::number(speciesBond->parameter(n)));
		item->setFlags(speciesBond->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// AngleTable row update function
void SpeciesTab::updateAngleTableRow(int row, SpeciesAngle* speciesAngle, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
			ui_.AngleTable->setItem(row, n, item);
		}
		else item = ui_.AngleTable->item(row, n);
		item->setText(QString::number(speciesAngle->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
		ui_.AngleTable->setItem(row, 3, item);
	}
	else item = ui_.AngleTable->item(row, 3);
	item->setText(speciesAngle->masterParameters() ? QString("@%1").arg(speciesAngle->masterParameters()->name()) : SpeciesAngle::angleFunctions().keywordFromInt(speciesAngle->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
			ui_.AngleTable->setItem(row, n+4, item);
		}
		else item = ui_.AngleTable->item(row, n+4);
		item->setText(QString::number(speciesAngle->parameter(n)));
		item->setFlags(speciesAngle->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// TorsionTable row update function
void SpeciesTab::updateTorsionTableRow(int row, SpeciesTorsion* speciesTorsion, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
			ui_.TorsionTable->setItem(row, n, item);
		}
		else item = ui_.TorsionTable->item(row, n);
		item->setText(QString::number(speciesTorsion->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
		ui_.TorsionTable->setItem(row, 4, item);
	}
	else item = ui_.TorsionTable->item(row, 4);
	item->setText(speciesTorsion->masterParameters() ? QString("@%1").arg(speciesTorsion->masterParameters()->name()) : SpeciesTorsion::torsionFunctions().keywordFromInt(speciesTorsion->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
			ui_.TorsionTable->setItem(row, n+5, item);
		}
		else item = ui_.TorsionTable->item(row, n+5);
		item->setText(QString::number(speciesTorsion->parameter(n)));
		item->setFlags(speciesTorsion->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// ImproperTable row update function
void SpeciesTab::updateImproperTableRow(int row, SpeciesImproper* speciesImproper, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
			ui_.ImproperTable->setItem(row, n, item);
		}
		else item = ui_.ImproperTable->item(row, n);
		item->setText(QString::number(speciesImproper->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
		ui_.ImproperTable->setItem(row, 4, item);
	}
	else item = ui_.ImproperTable->item(row, 4);
	item->setText(speciesImproper->masterParameters() ? QString("@%1").arg(speciesImproper->masterParameters()->name()) : SpeciesImproper::improperFunctions().keywordFromInt(speciesImproper->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesImproper>(speciesImproper));
			ui_.ImproperTable->setItem(row, n+5, item);
		}
		else item = ui_.ImproperTable->item(row, n+5);
		item->setText(QString::number(speciesImproper->parameter(n)));
		item->setFlags(speciesImproper->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

/*
 * Private Slots
 */

void SpeciesTab::on_ForcefieldButton_clicked(bool checked)
{
	// Select the desired Forcefield
	static SelectForcefieldDialog selectForcefieldDialog(this, ForcefieldLibrary::forcefields());
	Forcefield* ff = selectForcefieldDialog.selectForcefield(species_->forcefield());
	if (!ff) return;

	// Confirm, and ask whether to overwrite existing forcefield terms
	QMessageBox queryBox;
	queryBox.setText("Would you like to clear current terms from the Species and reapply the selected forcefield?");
	queryBox.setInformativeText("Reapply Terms?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	queryBox.setDefaultButton(QMessageBox::Cancel);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Cancel) return;

	// Set the new Forcefield on the Species
	species_->setForcefield(ff);

	if (ret == QMessageBox::Yes) species_->applyForcefieldTerms(dissolve_.coreData());

	updateControls();

	dissolveWindow_->setModified();
}

void SpeciesTab::on_ForcefieldAutoApplyCheck_clicked(bool checked)
{
	// TODO
}

void SpeciesTab::on_ForcefieldAutoUpdateIntramolecularCheck_clicked(bool checked)
{
	species_->setAutoUpdateIntramolecularTerms(checked);

	dissolveWindow_->setModified();

	updateControls();
}


void SpeciesTab::on_AtomAddButton_clicked(bool checked)
{
	SpeciesAtom* atom = species_->addAtom(&Elements::element(1), Vec3<double>());

	Locker refreshLocker(refreshLock_);

	TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this, &SpeciesTab::updateAtomTableRow);

	dissolveWindow_->setModified();
}

void SpeciesTab::on_AtomRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_AtomTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshLock_.isLocked()) return;

	// Get target SpeciesAtom from the passed widget
	SpeciesAtom* speciesAtom = w ? VariantPointer<SpeciesAtom>(w->data(Qt::UserRole)) : NULL;
	if (!speciesAtom) return;

	// Column of passed item tells us the type of data we need to change
	AtomType* atomType;
	switch (w->column())
	{
		// Element
		case (0):
			break;
		// AtomType
		case (1):
			// Check the text to see if it is an existing AtomType - if not, we should create it
			atomType = dissolve_.findAtomType(qPrintable(w->text()));
			if (!atomType)
			{
				atomType = dissolve_.addAtomType(speciesAtom->element());
				atomType->setName(qPrintable(w->text()));
			}
			speciesAtom->setAtomType(atomType);
			dissolveWindow_->setModified();
			break;
		// Coordinates
		case (2):
		case (3):
		case (4):
			speciesAtom->setCoordinate(w->column()-1, w->text().toDouble());
			dissolveWindow_->setModified();
			break;
		// Charge
		case (5):
			speciesAtom->setCharge(w->text().toDouble());
			// TODO This change needs to be propagated to all Configurations->Molecules based on this Species
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of SpeciesAtom table.\n", w->column());
			break;
	}
}

void SpeciesTab::on_AtomTable_itemSelectionChanged()
{
	if (refreshLock_.isLocked()) return;

	QTableWidgetItem* item;
	SpeciesAtom* i;

	// Set atom selection in viewer to be same as the table
	for (int n=0; n<ui_.AtomTable->rowCount(); ++n)
	{
		item = ui_.AtomTable->item(n, 0);
		i = VariantPointer<SpeciesAtom>(item->data(Qt::UserRole));

		if (item->isSelected()) species_->selectAtom(i);
		else species_->deselectAtom(i);
	}

	// Recreate selection primitive and update viewer
	ui_.ViewerWidget->speciesViewer()->recreateSelectionPrimitive();
	ui_.ViewerWidget->updateStatusBar();
}

void SpeciesTab::on_BondAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_BondRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_BondTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshLock_.isLocked() || (!species_)) return;

	// Get target SpeciesBond from the passed widget
	SpeciesBond* speciesBond = w ? VariantPointer<SpeciesBond>(w->data(Qt::UserRole)) : NULL;
	if (!speciesBond) return;

	// Column of passed item tells us the type of data we need to change
	int i, j;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
			// Get both atom indices and set the atoms in the interaction
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			if (species_->reconnectBond(speciesBond, i, j))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (2):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterBond(qPrintable(w->text()));
				speciesBond->setMasterParameters(master);
			}
			else
			{
				SpeciesBond::BondFunction bf = SpeciesBond::bondFunctions().enumeration(qPrintable(w->text()));
				speciesBond->setMasterParameters(NULL);
				speciesBond->setForm(bf);
			}
			updateRow = true;
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (3):
		case (4):
		case (5):
		case (6):
			if (speciesBond->masterParameters()) Messenger::error("Refusing to set bond parameter since it belongs to a master term.\n");
			else   speciesBond->setParameter(w->column()-3, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Bond table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		Locker refreshLocker(refreshLock_);
		updateBondTableRow(w->row(), speciesBond, false);
	}
}

void SpeciesTab::on_AngleAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_AngleRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_AngleTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshLock_.isLocked() || (!species_)) return;

	// Get target SpeciesAngle from the passed widget
	SpeciesAngle* speciesAngle = w ? VariantPointer<SpeciesAngle>(w->data(Qt::UserRole)) : NULL;
	if (!speciesAngle) return;

	// Column of passed item tells us the type of data we need to change
	int i, j, k;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
		case (2):
			// Get all atom indices and set the atoms in the interaction
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui_.BondTable->item(w->row(), 2)->text().toInt() - 1;
			if (species_->reconnectAngle(speciesAngle, i, j, k))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (3):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterAngle(qPrintable(w->text()));
				speciesAngle->setMasterParameters(master);
			}
			else
			{
				SpeciesAngle::AngleFunction af = SpeciesAngle::angleFunctions().enumeration(qPrintable(w->text()));
				speciesAngle->setMasterParameters(NULL);
				speciesAngle->setForm(af);
			}
			updateRow = true;
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (4):
		case (5):
		case (6):
		case (7):
			if (speciesAngle->masterParameters()) Messenger::error("Refusing to set angle parameter since it belongs to a master term.\n");
			else   speciesAngle->setParameter(w->column()-4, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Angle table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		Locker refreshLocker(refreshLock_);
		updateAngleTableRow(w->row(), speciesAngle, false);
	}
}

void SpeciesTab::on_TorsionAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_TorsionRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_TorsionTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshLock_.isLocked() || (!species_)) return;

	// Get target SpeciesTorsion from the passed widget
	SpeciesTorsion* speciesTorsion = w ? VariantPointer<SpeciesTorsion>(w->data(Qt::UserRole)) : NULL;
	if (!speciesTorsion) return;

	// Column of passed item tells us the type of data we need to change
	int i, j, k, l;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
		case (2):
		case (3):
			// Get all atom indices and set the atoms in the interaction
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui_.BondTable->item(w->row(), 2)->text().toInt() - 1;
			l = ui_.BondTable->item(w->row(), 3)->text().toInt() - 1;
			if (species_->reconnectTorsion(speciesTorsion, i, j, k, l))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (4):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterTorsion(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(master);
			}
			else
			{
				SpeciesTorsion::TorsionFunction tf = SpeciesTorsion::torsionFunctions().enumeration(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(NULL);
				speciesTorsion->setForm(tf);
			}
			updateRow = true;
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (5):
		case (6):
		case (7):
		case (8):
			if (speciesTorsion->masterParameters()) Messenger::error("Refusing to set torsion parameter since it belongs to a master term.\n");
			else   speciesTorsion->setParameter(w->column()-5, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Torsion table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		Locker refreshLocker(refreshLock_);
		updateTorsionTableRow(w->row(), speciesTorsion, false);
	}
}

void SpeciesTab::on_ImproperAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_ImproperRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_ImproperTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshLock_.isLocked() || (!species_)) return;

	// Get target SpeciesImproper from the passed widget
	SpeciesImproper* speciesImproper = w ? VariantPointer<SpeciesImproper>(w->data(Qt::UserRole)) : NULL;
	if (!speciesImproper) return;

	// Column of passed item tells us the type of data we need to change
	int i, j, k, l;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
		case (2):
		case (3):
			// Get all atom indices and set the atoms in the interaction
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui_.BondTable->item(w->row(), 2)->text().toInt() - 1;
			l = ui_.BondTable->item(w->row(), 3)->text().toInt() - 1;
			if (species_->reconnectImproper(speciesImproper, i, j, k, l))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (4):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterImproper(qPrintable(w->text()));
				speciesImproper->setMasterParameters(master);
			}
			else
			{
				SpeciesImproper::ImproperFunction tf = SpeciesImproper::improperFunctions().enumeration(qPrintable(w->text()));
				speciesImproper->setMasterParameters(NULL);
				speciesImproper->setForm(tf);
			}
			updateRow = true;
			dissolveWindow_->setModified();
			break;
		// Parameters
		case (5):
		case (6):
		case (7):
		case (8):
			if (speciesImproper->masterParameters()) Messenger::error("Refusing to set improper parameter since it belongs to a master term.\n");
			else   speciesImproper->setParameter(w->column()-5, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Improper table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		Locker refreshLocker(refreshLock_);
		updateImproperTableRow(w->row(), speciesImproper, false);
		refreshLocker.unlock();
	}
}

/*
 * Public Functions
 */

// Update Geometry tab
void SpeciesTab::updateGeometryTab()
{
	Locker refreshLocker(refreshLock_);

	// -- Forcefield and Terms
	ui_.ForcefieldButton->setText(species_ && species_->forcefield() ? species_->forcefield()->name() : "<None>");
	ui_.ForcefieldAutoUpdateIntramolecularCheck->setChecked(species_->autoUpdateIntramolecularTerms());

	// -- SpeciesAtom Table
	if (dissolve_.pairPotentialsIncludeCoulomb()) ui_.AtomTable->showColumn(5);
	else ui_.AtomTable->hideColumn(5);
	if (!species_) ui_.AtomTable->clearContents();
	else TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this, &SpeciesTab::updateAtomTableRow);

	// -- Intramolecular terms
	if (!species_)
	{
		ui_.BondTable->clearContents();
		ui_.AngleTable->clearContents();
		ui_.TorsionTable->clearContents();
		ui_.ImproperTable->clearContents();
	}
	else
	{
		TableWidgetUpdater<SpeciesTab,SpeciesBond> bondUpdater(ui_.BondTable, species_->bonds(), this, &SpeciesTab::updateBondTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesAngle> angleUpdater(ui_.AngleTable, species_->angles(), this, &SpeciesTab::updateAngleTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesTorsion> torsionUpdater(ui_.TorsionTable, species_->torsions(), this, &SpeciesTab::updateTorsionTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesImproper> improperUpdater(ui_.ImproperTable, species_->impropers(), this, &SpeciesTab::updateImproperTableRow);
	}
	ui_.BondTable->resizeColumnsToContents();
	ui_.AngleTable->resizeColumnsToContents();
	ui_.TorsionTable->resizeColumnsToContents();
	ui_.ImproperTable->resizeColumnsToContents();
}

