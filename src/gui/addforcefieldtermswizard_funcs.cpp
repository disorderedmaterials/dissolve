/*
	*** Add Forcefield Terms Wizard Functions
	*** src/gui/addforcefieldtermswizard_funcs.cpp
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

#include "gui/addforcefieldtermswizard.h"
#include "gui/selectforcefieldwidget.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/fflibrary.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

// Constructor / Destructor
AddForcefieldTermsWizard::AddForcefieldTermsWizard(QWidget* parent) : temporaryDissolve_(temporaryCoreData_)
{
	dissolveReference_ = NULL;
	targetSpecies_ = NULL;
	modifiedSpecies_ = NULL;

	// Set up our UI, and attach the wizard's widgets to placeholder widgets (if available)
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Create parent items for MasterTerms tree
	masterBondItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
	masterBondItemParent_->setFlags(Qt::ItemIsEnabled);
	masterBondItemParent_->setExpanded(true);
	masterBondItemParent_->setText(0, "Bonds");
	masterAngleItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
	masterAngleItemParent_->setFlags(Qt::ItemIsEnabled);
	masterAngleItemParent_->setText(0, "Angles");
	masterAngleItemParent_->setExpanded(true);
	masterTorsionItemParent_ = new QTreeWidgetItem(ui_.MasterTermsTree);
	masterTorsionItemParent_->setFlags(Qt::ItemIsEnabled);
	masterTorsionItemParent_->setText(0, "Torsions");
	masterTorsionItemParent_->setExpanded(true);

	// Register pages with the wizard
	registerPage(AddForcefieldTermsWizard::SelectForcefieldPage, "Select Forcefield", AddForcefieldTermsWizard::SelectTermsPage);
	registerPage(AddForcefieldTermsWizard::SelectTermsPage, "Select Terms", AddForcefieldTermsWizard::AtomTypesPage);
	registerPage(AddForcefieldTermsWizard::AtomTypesPage, "Check AtomTypes", AddForcefieldTermsWizard::MasterTermsPage);
	registerFinishPage(AddForcefieldTermsWizard::MasterTermsPage, "Check MasterTerms");

	// Connect signals / slots
	connect(ui_.AtomTypesList->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(atomTypesListEdited(QWidget*)));
	connect(ui_.MasterTermsTree->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(masterTermsTreeEdited(QWidget*)));

	lockedForRefresh_ = 0;
}

AddForcefieldTermsWizard::~AddForcefieldTermsWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void AddForcefieldTermsWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Set target Species that we are acquiring forcefield terms for
void AddForcefieldTermsWizard::setTargetSpecies(Species* sp)
{
	targetSpecies_ = sp;
}

// Apply our Forcefield terms to the targetted Species within the specified Dissolve object
bool AddForcefieldTermsWizard::applyForcefieldTerms(Dissolve& dissolve)
{
	/*
	 * We have the original Species (which should exist in the provided Dissolve object) in currentSpecies()
	 * and the temporary species modifiedSpecies_ which contains the new Forcefield terms.
	 *
	 * So, we will assume that the ordering of all data (atoms, bonds etc.) in the two are the same, and then:
	 *
	 * 1) Loop over atoms and copy / reassign AtomTypes
	 * 2) Loop over bonds and create / assign parameters / MasterTerms (if BondTermsCheck is checked)
	 * 3) Loop over angles and create / assign parameters / MasterTerms (if AngleTermsCheck is checked)
	 * 4) Loop over torsions and create / assign parameters / MasterTerms (if TorsionTermsCheck is checked)
	 */

	// 1) Set AtomTypes
	ListIterator<SpeciesAtom> atomIterator(targetSpecies_->atoms());
	SpeciesAtom* modifiedI = modifiedSpecies_->atoms().first();
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		// Copy AtomType
		dissolve.copyAtomType(modifiedI, i);

		// Overwrite existing parameters?
		if (ui_.AtomTypesOverwriteParametersCheck->isChecked())
		{
			i->atomType()->parameters() = modifiedI->atomType()->parameters();
			i->atomType()->setShortRangeType(modifiedI->atomType()->shortRangeType());
		}

		// Move to next modified atom
		modifiedI = modifiedI->next();
	}

	// Copy bond terms
	if (ui_.BondTermsCheck->isChecked())
	{
		ListIterator<SpeciesBond> originalBondIterator(targetSpecies_->bonds());
		ListIterator<SpeciesBond> modifiedBondIterator(modifiedSpecies_->bonds());
		while (SpeciesBond* originalBond = originalBondIterator.iterate())
		{
			SpeciesBond* modifiedBond = modifiedBondIterator.iterate();

			// Copy interaction parameters, including MasterIntra if necessary
			dissolve.copySpeciesIntra(modifiedBond, originalBond);
		}
	}

	// Copy angle terms
	if (ui_.AngleTermsCheck->isChecked())
	{
		ListIterator<SpeciesAngle> originalAngleIterator(targetSpecies_->angles());
		ListIterator<SpeciesAngle> modifiedAngleIterator(modifiedSpecies_->angles());
		while (SpeciesAngle* originalAngle = originalAngleIterator.iterate())
		{
			SpeciesAngle* modifiedAngle = modifiedAngleIterator.iterate();

			// Copy interaction parameters, including MasterIntra if necessary
			dissolve.copySpeciesIntra(modifiedAngle, originalAngle);
		}
	}

	// Copy torsion terms
	if (ui_.TorsionTermsCheck->isChecked())
	{
		ListIterator<SpeciesTorsion> originalTorsionIterator(targetSpecies_->torsions());
		ListIterator<SpeciesTorsion> modifiedTorsionIterator(modifiedSpecies_->torsions());
		while (SpeciesTorsion* originalTorsion = originalTorsionIterator.iterate())
		{
			SpeciesTorsion* modifiedTorsion = modifiedTorsionIterator.iterate();

			// Copy interaction parameters, including MasterIntra if necessary
			dissolve.copySpeciesIntra(modifiedTorsion, originalTorsion);
		}
	}

	return true;
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddForcefieldTermsWizard::displayControlPage(int index)
{
	// Check page index given
	if ((index < 0) || (index >= AddForcefieldTermsWizard::nPages)) return Messenger::error("Page index %i not recognised.\n", index);

	// Page index is valid, so show it - no need to switch/case
	ui_.MainStack->setCurrentIndex(index);

	// Update controls in the target page if necessary
	switch (index)
	{
		default:
			break;
	}

	return true;
}

// Return whether progression to the next page from the current page is allowed
bool AddForcefieldTermsWizard::progressionAllowed(int index) const
{
	// Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
	switch (index)
	{
		case (AddForcefieldTermsWizard::SelectForcefieldPage):
			return (ui_.ForcefieldWidget->currentForcefield());
		default:
			break;
	}

	return true;
}

// Perform any necessary actions before moving to the next page
bool AddForcefieldTermsWizard::prepareForNextPage(int currentIndex)
{
	Forcefield* ff;
	switch (currentIndex)
	{
		case (AddForcefieldTermsWizard::SelectTermsPage):
			// Sanity check the current Forcefield
			ff = ui_.ForcefieldWidget->currentForcefield();
			if (!ff) return false;

			// Copy selected Species to our temporary instance, detach any MasterTerm references, and delete the MasterTerms
			modifiedSpecies_ = temporaryDissolve_.copySpecies(targetSpecies_);
			modifiedSpecies_->detachFromMasterTerms();
			temporaryCoreData_.clearMasterTerms();

			// Assign AtomTypes
			if (ui_.AtomTypesAssignAllRadio->isChecked())
			{
				// Remove all previous AtomType association from the Species, and subsequently from the main object
				modifiedSpecies_->clearAtomTypes();
				temporaryDissolve_.clearAtomTypes();

				if (!ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_)) return false;
			}
			else if (ui_.AtomTypesAssignMissingRadio->isChecked()) if (!ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, true)) return false;

			// Assign intramolecular terms
			if (!ff->assignIntramolecular(modifiedSpecies_, ui_.UseTypesFromSpeciesCheck->isChecked(), ui_.BondTermsCheck->isChecked(), ui_.AngleTermsCheck->isChecked(), ui_.TorsionTermsCheck->isChecked())) return false;

			updateAtomTypesPage();
			updateMasterTermsPage();
		default:
			break;
	}

	return true;
}

// Determine next page for the current page, based on current data
int AddForcefieldTermsWizard::determineNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddForcefieldTermsWizard::AtomTypesPage):
			// If there are master terms present, go to that page first. Otherwise, skip straight to naming
			if (temporaryCoreData_.nMasterBonds() || temporaryCoreData_.nMasterAngles() || temporaryCoreData_.nMasterTorsions()) return AddForcefieldTermsWizard::MasterTermsPage;
			else return AddForcefieldTermsWizard::FinishPage;
		default:
			break;
	}

	return -1;
}

// Perform any necessary actions before moving to the previous page
bool AddForcefieldTermsWizard::prepareForPreviousPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddForcefieldTermsWizard::AtomTypesPage):
			// Clear the temporary dissolve instance
			temporaryDissolve_.clear();
		default:
			break;
	}

	return true;
}

/*
 * Controls
 */

// Reset widget, ready for generating new terms for a Species
void AddForcefieldTermsWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(AddForcefieldTermsWizard::SelectForcefieldPage);
}

/*
 * Select Forcefield Page
 */

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid)
{
	updateProgressionControls();
}

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldDoubleClicked()
{
	goToPage(AddForcefieldTermsWizard::SelectTermsPage);
	if (!ui_.ForcefieldWidget->currentForcefield()) return;
}

/*
 * Terms Page
 */

/*
 * AtomTypes Page
 */

// Row update function for AtomTypesList
void AddForcefieldTermsWizard::updateAtomTypesListRow(int row, AtomType* atomType, bool createItem)
{
	QListWidgetItem* item;
	if (createItem)
	{
		item = new QListWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui_.AtomTypesList->insertItem(row, item);
	}
	else item = ui_.AtomTypesList->item(row);

	// Set item data
	item->setText(atomType->name());
	item->setIcon(QIcon(dissolveReference_->findAtomType(atomType->name()) ?  ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Update page with AtomTypes in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateAtomTypesPage()
{
	// Update the list against the global AtomType list
	ListWidgetUpdater<AddForcefieldTermsWizard,AtomType> listUpdater(ui_.AtomTypesList, temporaryCoreData_.constAtomTypes(), this, &AddForcefieldTermsWizard::updateAtomTypesListRow);

	// Determine whether we have any naming conflicts
	bool conflicts = false;
	ListIterator<AtomType> typeIterator(temporaryCoreData_.constAtomTypes());
	while (AtomType* at = typeIterator.iterate()) if (dissolveReference_->findAtomType(at->name()))
	{
		conflicts = true;
		break;
	}
	ui_.AtomTypesIndicator->setNotOK(conflicts);
	if (conflicts) ui_.AtomTypesIndicatorLabel->setText("One or more AtomTypes in the imported Species conflict with existing types");
	else ui_.AtomTypesIndicatorLabel->setText("There are no naming conflicts with the imported AtomTypes");
}

void AddForcefieldTermsWizard::on_AtomTypesList_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.AtomTypesList->selectedItems().count() > 0;
	ui_.AtomTypesPrefixButton->setEnabled(isSelection);
	ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::atomTypesListEdited(QWidget* lineEdit)
{
	// Since the signal that leads us here does not tell us the item that was edited, update all AtomType names here before updating the page
	for (int n=0; n<ui_.AtomTypesList->count(); ++n)
	{
		QListWidgetItem* item = ui_.AtomTypesList->item(n);
		AtomType* at = VariantPointer<AtomType>(item->data(Qt::UserRole));
		if (!at) continue;

		at->setName(qPrintable(item->text()));
	}

	updateAtomTypesPage();
}

void AddForcefieldTermsWizard::on_AtomTypesPrefixButton_clicked(bool checked)
{
	bool ok;
	QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QListWidgetItem*> selectedItems = ui_.AtomTypesList->selectedItems();
	QList<QListWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		AtomType* at = VariantPointer<AtomType>((*i)->data(Qt::UserRole));
		at->setName(CharString("%s%s", qPrintable(prefix), at->name()));
	}

	updateAtomTypesPage();
}

void AddForcefieldTermsWizard::on_AtomTypesSuffixButton_clicked(bool checked)
{
	bool ok;
	QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QListWidgetItem*> selectedItems = ui_.AtomTypesList->selectedItems();
	QList<QListWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		AtomType* at = VariantPointer<AtomType>((*i)->data(Qt::UserRole));
		at->setName(CharString("%s%s", at->name(), qPrintable(suffix)));
	}

	updateAtomTypesPage();
}

/*
 * MasterTerms Page
 */

// Row update function for MasterTermsList
void AddForcefieldTermsWizard::updateMasterTermsTreeChild(QTreeWidgetItem* parent, int childIndex, MasterIntra* masterIntra, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<MasterIntra>(masterIntra));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		parent->insertChild(childIndex, item);
	}
	else item = parent->child(childIndex);

	// Set item data
	item->setText(0, masterIntra->name());
	item->setIcon(0, QIcon(dissolveReference_->constCoreData().findMasterTerm(masterIntra->name()) ?  ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Update page with MasterTerms in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateMasterTermsPage()
{
	// Update the list against the global MasterTerm tree
	TreeWidgetUpdater<AddForcefieldTermsWizard,MasterIntra> bondUpdater(masterBondItemParent_, temporaryCoreData_.masterBonds(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddForcefieldTermsWizard,MasterIntra> angleUpdater(masterAngleItemParent_, temporaryCoreData_.masterAngles(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddForcefieldTermsWizard,MasterIntra> torsionUpdater(masterTorsionItemParent_, temporaryCoreData_.masterTorsions(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);

	// Determine whether we have any naming conflicts
	bool conflicts = false;
	ListIterator<MasterIntra> bondIterator(temporaryCoreData_.masterBonds());
	while (MasterIntra* intra = bondIterator.iterate()) if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ListIterator<MasterIntra> angleIterator(temporaryCoreData_.masterAngles());
	while (MasterIntra* intra = angleIterator.iterate()) if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ListIterator<MasterIntra> torsionIterator(temporaryCoreData_.masterTorsions());
	while (MasterIntra* intra = torsionIterator.iterate()) if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ui_.MasterTermsIndicator->setNotOK(conflicts);
	if (conflicts) ui_.MasterTermsIndicatorLabel->setText("One or more MasterTerms in the imported Species conflict with existing ones");
	else ui_.MasterTermsIndicatorLabel->setText("There are no naming conflicts with the imported MasterTerms");
}

void AddForcefieldTermsWizard::on_MasterTermsTree_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.MasterTermsTree->selectedItems().count() > 0;
	ui_.MasterTermsPrefixButton->setEnabled(isSelection);
	ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::masterTermsTreeEdited(QWidget* lineEdit)
{
	// Since the signal that leads us here does not tell us the item that was edited, update all MasterTerm names here before updating the page
	for (int n=0; n<masterBondItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem* item = masterBondItemParent_->child(n);
		MasterIntra* intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra) continue;

		intra->setName(qPrintable(item->text(0)));
	}
	for (int n=0; n<masterAngleItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem* item = masterAngleItemParent_->child(n);
		MasterIntra* intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra) continue;

		intra->setName(qPrintable(item->text(0)));
	}
	for (int n=0; n<masterTorsionItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem* item = masterTorsionItemParent_->child(n);
		MasterIntra* intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra) continue;

		intra->setName(qPrintable(item->text(0)));
	}

	updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsPrefixButton_clicked(bool checked)
{
	bool ok;
	QString prefix = QInputDialog::getText(this, "Prefix MasterTerms", "Enter prefix to apply to all selected MasterTerms", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QTreeWidgetItem*> selectedItems = ui_.MasterTermsTree->selectedItems();
	QList<QTreeWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		MasterIntra* intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
		intra->setName(CharString("%s%s", qPrintable(prefix), intra->name()));
	}

	updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsSuffixButton_clicked(bool checked)
{
	bool ok;
	QString suffix = QInputDialog::getText(this, "Suffix MasterTerms", "Enter suffix to apply to all selected MasterTerms", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QTreeWidgetItem*> selectedItems = ui_.MasterTermsTree->selectedItems();
	QList<QTreeWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		MasterIntra* intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
		intra->setName(CharString("%s%s", intra->name(), qPrintable(suffix)));
	}

	updateMasterTermsPage();
}
