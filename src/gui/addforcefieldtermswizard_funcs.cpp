/*
	*** Add Forcefield Terms Wizard Functions
	*** src/gui/addforcefieldtermswizard_funcs.cpp
	Copyright T. Youngs 2012-2020

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

#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/fflibrary.h"
#include "gui/addforcefieldtermswizard.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/selectforcefieldwidget.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

// Constructor / Destructor
AddForcefieldTermsWizard::AddForcefieldTermsWizard(QWidget *parent) : temporaryDissolve_(temporaryCoreData_)
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
	registerPage(AddForcefieldTermsWizard::SelectForcefieldPage, "Select Forcefield", AddForcefieldTermsWizard::AtomTypesPage);
	registerPage(AddForcefieldTermsWizard::AtomTypesPage, "Determine Atom Types", AddForcefieldTermsWizard::AtomTypesConflictsPage);
	registerPage(AddForcefieldTermsWizard::AtomTypesConflictsPage, "Check Atom Types", AddForcefieldTermsWizard::IntramolecularPage);
	registerPage(AddForcefieldTermsWizard::IntramolecularPage, "Assign Intramolecular Terms");
	registerPage(AddForcefieldTermsWizard::MasterTermsPage, "Check Master Terms", WizardWidgetPageInfo::FinishHereFlag);

	// Connect signals / slots
	connect(ui_.AtomTypesConflictsList->itemDelegate(), SIGNAL(commitData(QWidget *)), this, SLOT(atomTypesConflictsListEdited(QWidget *)));
	connect(ui_.MasterTermsTree->itemDelegate(), SIGNAL(commitData(QWidget *)), this, SLOT(masterTermsTreeEdited(QWidget *)));

	lockedForRefresh_ = 0;
}

AddForcefieldTermsWizard::~AddForcefieldTermsWizard() {}

/*
 * Data
 */

// Return (mapped) name to use for specified type
const char *AddForcefieldTermsWizard::mappedName(const AtomType *at)
{
	RefDataItem<const AtomType, CharString> *item = typeNameMappings_.contains(at);
	if (!item)
		return "???";
	else
		return item->data();
}

// Set Dissolve reference
void AddForcefieldTermsWizard::setMainDissolveReference(const Dissolve *dissolveReference) { dissolveReference_ = dissolveReference; }

// Set target Species that we are acquiring forcefield terms for
void AddForcefieldTermsWizard::setTargetSpecies(Species *sp)
{
	targetSpecies_ = sp;

	// Set initial state of controls
	if (sp->nSelectedAtoms() != 0)
	{
		ui_.AtomTypesAssignSelectionRadio->setChecked(true);
		ui_.IntramolecularTermsAssignSelectionRadio->setChecked(true);
	}
}

// Apply our Forcefield terms to the targetted Species within the specified Dissolve object
bool AddForcefieldTermsWizard::applyForcefieldTerms(Dissolve &dissolve)
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

	bool typesSelectionOnly = ui_.AtomTypesAssignSelectionRadio->isChecked();
	bool intraSelectionOnly = ui_.IntramolecularTermsAssignSelectionRadio->isChecked();

	// 1) Set AtomTypes
	ListIterator<SpeciesAtom> originalAtomIterator(targetSpecies_->atoms());
	ListIterator<SpeciesAtom> modifiedAtomIterator(modifiedSpecies_->atoms());
	while (SpeciesAtom *i = originalAtomIterator.iterate())
	{
		const SpeciesAtom *modifiedI = modifiedAtomIterator.iterate();

		// Selection only?
		if (typesSelectionOnly && (!i->isSelected()))
			continue;

		// Copy AtomType
		dissolve.copyAtomType(modifiedI, i);

		// Overwrite existing parameters?
		if (ui_.AtomTypesOverwriteParametersCheck->isChecked())
		{
			i->atomType()->parameters() = modifiedI->atomType()->parameters();
			i->atomType()->setShortRangeType(modifiedI->atomType()->shortRangeType());
			i->setCharge(modifiedI->charge());
			dissolve.coreData().bumpAtomTypesVersion();
		}
	}

	// Copy intramolecular terms
	if (!ui_.IntramolecularTermsAssignNoneRadio->isChecked())
	{
		DynamicArrayIterator<SpeciesBond> originalBondIterator(targetSpecies_->bonds());
		DynamicArrayConstIterator<SpeciesBond> modifiedBondIterator(modifiedSpecies_->constBonds());
		while (SpeciesBond *originalBond = originalBondIterator.iterate())
		{
			const SpeciesBond *modifiedBond = modifiedBondIterator.iterate();

			// Selection only?
			if (intraSelectionOnly && (!originalBond->isSelected()))
				continue;

			// Copy interaction parameters, including MasterIntra if necessary
			dissolve.copySpeciesIntra(modifiedBond, originalBond);
		}

		DynamicArrayIterator<SpeciesAngle> originalAngleIterator(targetSpecies_->angles());
		DynamicArrayConstIterator<SpeciesAngle> modifiedAngleIterator(modifiedSpecies_->constAngles());
		while (SpeciesAngle *originalAngle = originalAngleIterator.iterate())
		{
			const SpeciesAngle *modifiedAngle = modifiedAngleIterator.iterate();

			// Selection only?
			if (intraSelectionOnly && (!originalAngle->isSelected()))
				continue;

			// Copy interaction parameters, including MasterIntra if necessary
			dissolve.copySpeciesIntra(modifiedAngle, originalAngle);
		}

		DynamicArrayIterator<SpeciesTorsion> originalTorsionIterator(targetSpecies_->torsions());
		DynamicArrayConstIterator<SpeciesTorsion> modifiedTorsionIterator(modifiedSpecies_->constTorsions());
		while (SpeciesTorsion *originalTorsion = originalTorsionIterator.iterate())
		{
			const SpeciesTorsion *modifiedTorsion = modifiedTorsionIterator.iterate();

			// Selection only?
			if (intraSelectionOnly && (!originalTorsion->isSelected()))
				continue;

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
	if ((index < 0) || (index >= AddForcefieldTermsWizard::nPages))
		return Messenger::error("Page index %i not recognised.\n", index);

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
	Forcefield *ff = ui_.ForcefieldWidget->currentForcefield();
	switch (currentIndex)
	{
	case (AddForcefieldTermsWizard::SelectForcefieldPage):
		ui_.AtomTypesAssignSelectionRadio->setEnabled(targetSpecies_->nSelectedAtoms() != 0);
		break;
	case (AddForcefieldTermsWizard::AtomTypesPage):
		// Sanity check the current Forcefield
		if (!ff)
			return false;

		// Copy selected Species to our temporary instance
		modifiedSpecies_ = temporaryDissolve_.copySpecies(targetSpecies_);

		// Determine atom types
		if (ui_.AtomTypesAssignAllRadio->isChecked())
		{
			// Remove all previous AtomType association from the Species, and subsequently from the main object
			modifiedSpecies_->clearAtomTypes();
			temporaryDissolve_.clearAtomTypes();

			if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeAll) != 0)
				return false;
		}
		else if (ui_.AtomTypesAssignSelectionRadio->isChecked())
		{
			if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeSelection) != 0)
				return false;
		}
		else if (ui_.AtomTypesAssignMissingRadio->isChecked())
		{
			if (ff->assignAtomTypes(modifiedSpecies_, temporaryCoreData_, Forcefield::TypeMissing) != 0)
				return false;
		}

		updateAtomTypesConflictsPage();
		checkForAtomTypeConflicts();
		break;
	case (AddForcefieldTermsWizard::AtomTypesConflictsPage):
		// Detach any MasterTerm references, and delete the MasterTerms
		modifiedSpecies_->detachFromMasterTerms();
		temporaryCoreData_.clearMasterTerms();

		// Update any type name mappings we have
		typeNameMappings_.clear();
		for (int i = 0; i < ui_.AtomTypesConflictsList->count(); ++i)
		{
			QListWidgetItem *item = ui_.AtomTypesConflictsList->item(i);
			typeNameMappings_.append(VariantPointer<AtomType>(item->data(Qt::UserRole)), qPrintable(item->text()));
		}

		// Assign intramolecular terms
		if (!ui_.IntramolecularTermsAssignNoneRadio->isChecked())
		{
			int flags = 0;
			if (ui_.IgnoreCurrentTypesCheck->isChecked())
				flags += Forcefield::DetermineTypesFlag;
			if (!ui_.NoImproperTermsCheck->isChecked())
				flags += Forcefield::GenerateImpropersFlag;
			if (ui_.IntramolecularTermsAssignSelectionRadio->isChecked())
				flags += Forcefield::SelectionOnlyFlag;

			if (!ff->assignIntramolecular(modifiedSpecies_, flags))
				return false;

			// Reduce to master terms?
			if (!ui_.NoMasterTermsCheck->isChecked())
			{
				CharString termName;

				// Loop over bonds in the modified species
				DynamicArrayIterator<SpeciesBond> bondIterator(modifiedSpecies_->bonds());
				while (SpeciesBond *bond = bondIterator.iterate())
				{
					// Selection only?
					if ((flags & Forcefield::SelectionOnlyFlag) && (!bond->isSelected()))
						continue;

					// Construct a name for the master term based on the atom types - order atom types alphabetically for consistency
					if (QString(mappedName(bond->i()->atomType())) < QString(mappedName(bond->j()->atomType())))
						termName.sprintf("%s-%s", mappedName(bond->i()->atomType()), mappedName(bond->j()->atomType()));
					else
						termName.sprintf("%s-%s", mappedName(bond->j()->atomType()), mappedName(bond->i()->atomType()));

					// Search for an existing master term by this name
					MasterIntra *master = temporaryCoreData_.hasMasterBond(termName);
					if (!master)
					{
						// Create it now
						master = temporaryCoreData_.addMasterBond(termName);
						master->setForm(bond->form());
						master->setParameters(bond->parameters());
					}
					bond->setMasterParameters(master);
				}

				// Loop over angles in the modified species
				DynamicArrayIterator<SpeciesAngle> angleIterator(modifiedSpecies_->angles());
				while (SpeciesAngle *angle = angleIterator.iterate())
				{
					// Selection only?
					if ((flags & Forcefield::SelectionOnlyFlag) && (!angle->isSelected()))
						continue;

					// Construct a name for the master term based on the atom types - order atom types alphabetically for consistency
					if (QString(mappedName(angle->i()->atomType())) < QString(mappedName(angle->k()->atomType())))
						termName.sprintf("%s-%s-%s", mappedName(angle->i()->atomType()), mappedName(angle->j()->atomType()), mappedName(angle->k()->atomType()));
					else
						termName.sprintf("%s-%s-%s", mappedName(angle->k()->atomType()), mappedName(angle->j()->atomType()), mappedName(angle->i()->atomType()));

					// Search for an existing master term by this name
					MasterIntra *master = temporaryCoreData_.hasMasterAngle(termName);
					if (!master)
					{
						// Create it now
						master = temporaryCoreData_.addMasterAngle(termName);
						master->setForm(angle->form());
						master->setParameters(angle->parameters());
					}
					angle->setMasterParameters(master);
				}

				// Loop over torsions in the modified species
				DynamicArrayIterator<SpeciesTorsion> torsionIterator(modifiedSpecies_->torsions());
				while (SpeciesTorsion *torsion = torsionIterator.iterate())
				{
					// Selection only?
					if ((flags & Forcefield::SelectionOnlyFlag) && (!torsion->isSelected()))
						continue;

					// Construct a name for the master term based on the atom types - order atom types alphabetically for consistency
					if (QString(mappedName(torsion->i()->atomType())) < QString(mappedName(torsion->l()->atomType())))
						termName.sprintf("%s-%s-%s-%s", mappedName(torsion->i()->atomType()), mappedName(torsion->j()->atomType()), mappedName(torsion->k()->atomType()),
								 mappedName(torsion->l()->atomType()));
					else
						termName.sprintf("%s-%s-%s-%s", mappedName(torsion->l()->atomType()), mappedName(torsion->k()->atomType()), mappedName(torsion->j()->atomType()),
								 mappedName(torsion->i()->atomType()));

					// Search for an existing master term by this name
					MasterIntra *master = temporaryCoreData_.hasMasterTorsion(termName);
					if (!master)
					{
						// Create it now
						master = temporaryCoreData_.addMasterTorsion(termName);
						master->setForm(torsion->form());
						master->setParameters(torsion->parameters());
					}
					torsion->setMasterParameters(master);
				}

				// Loop over impropers in the modified species
				DynamicArrayIterator<SpeciesImproper> improperIterator(modifiedSpecies_->impropers());
				while (SpeciesImproper *improper = improperIterator.iterate())
				{
					// Selection only?
					if ((flags & Forcefield::SelectionOnlyFlag) && (!improper->isSelected()))
						continue;

					// Construct a name for the master term based on the atom types - order atom types alphabetically for consistency
					if (QString(mappedName(improper->i()->atomType())) < QString(mappedName(improper->l()->atomType())))
						termName.sprintf("%s-%s-%s-%s", mappedName(improper->i()->atomType()), mappedName(improper->j()->atomType()), mappedName(improper->k()->atomType()),
								 mappedName(improper->l()->atomType()));
					else
						termName.sprintf("%s-%s-%s-%s", mappedName(improper->l()->atomType()), mappedName(improper->k()->atomType()), mappedName(improper->j()->atomType()),
								 mappedName(improper->i()->atomType()));

					// Search for an existing master term by this name
					MasterIntra *master = temporaryCoreData_.hasMasterImproper(termName);
					if (!master)
					{
						// Create it now
						master = temporaryCoreData_.addMasterImproper(termName);
						master->setForm(improper->form());
						master->setParameters(improper->parameters());
					}
					improper->setMasterParameters(master);
				}
			}
		}

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
	case (AddForcefieldTermsWizard::IntramolecularPage):
		if ((!ui_.IntramolecularTermsAssignNoneRadio->isChecked()) && (!ui_.NoMasterTermsCheck->isChecked()))
			return AddForcefieldTermsWizard::MasterTermsPage;
		else
			return WizardWidgetPageInfo::FinishHereFlag;
		break;
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
	case (AddForcefieldTermsWizard::AtomTypesConflictsPage):
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

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_ForcefieldWidget_forcefieldDoubleClicked()
{
	goToPage(AddForcefieldTermsWizard::AtomTypesPage);
	if (!ui_.ForcefieldWidget->currentForcefield())
		return;
}

/*
 * Terms Page
 */

/*
 * AtomTypes Page
 */

// Row update function for AtomTypesConflictsList
void AddForcefieldTermsWizard::updateAtomTypesConflictsListRow(int row, AtomType *atomType, bool createItem)
{
	QListWidgetItem *item;
	if (createItem)
	{
		item = new QListWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui_.AtomTypesConflictsList->insertItem(row, item);
	}
	else
		item = ui_.AtomTypesConflictsList->item(row);

	// Set item data
	item->setText(atomType->name());
	item->setIcon(QIcon(dissolveReference_->findAtomType(atomType->name()) ? ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Update page with AtomTypes in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateAtomTypesConflictsPage()
{
	// Update the list against the global AtomType list
	ListWidgetUpdater<AddForcefieldTermsWizard, AtomType> listUpdater(ui_.AtomTypesConflictsList, temporaryCoreData_.constAtomTypes(), this,
									  &AddForcefieldTermsWizard::updateAtomTypesConflictsListRow);

	typeNameMappings_.clear();
}

// Check for atom type naming conflicts
void AddForcefieldTermsWizard::checkForAtomTypeConflicts()
{
	// Determine whether we have any naming conflicts
	int nConflicts = 0;
	for (int i = 0; i < ui_.AtomTypesConflictsList->count(); ++i)
	{
		QListWidgetItem *item = ui_.AtomTypesConflictsList->item(i);

		if (dissolveReference_->findAtomType(qPrintable(item->text())))
		{
			item->setIcon(QIcon(":/general/icons/general_warn.svg"));
			++nConflicts;
		}
		else
			item->setIcon(QIcon(":/general/icons/general_true.svg"));
	}

	ui_.AtomTypesIndicator->setNotOK(nConflicts > 0);

	if (nConflicts > 0)
		ui_.AtomTypesIndicatorLabel->setText("One or more AtomTypes in the imported Species conflict with existing types");
	else
		ui_.AtomTypesIndicatorLabel->setText("There are no naming conflicts with the imported AtomTypes");
}

void AddForcefieldTermsWizard::on_AtomTypesConflictsList_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.AtomTypesConflictsList->selectedItems().count() > 0;
	ui_.AtomTypesPrefixButton->setEnabled(isSelection);
	ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::atomTypesConflictsListEdited(QWidget *lineEdit) { checkForAtomTypeConflicts(); }

void AddForcefieldTermsWizard::on_AtomTypesPrefixButton_clicked(bool checked)
{
	bool ok;
	QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok)
		return;

	QList<QListWidgetItem *> selectedItems = ui_.AtomTypesConflictsList->selectedItems();
	QList<QListWidgetItem *>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
		(*i)->setText(QString("%1%2").arg(prefix, (*i)->text()));

	checkForAtomTypeConflicts();
}

void AddForcefieldTermsWizard::on_AtomTypesSuffixButton_clicked(bool checked)
{
	bool ok;
	QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok)
		return;

	QList<QListWidgetItem *> selectedItems = ui_.AtomTypesConflictsList->selectedItems();
	QList<QListWidgetItem *>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
		(*i)->setText(QString("%1%2").arg((*i)->text()));

	checkForAtomTypeConflicts();
}

/*
 * Intramolecular Page
 */

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignAllRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignSelectionRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_IntramolecularTermsAssignNoneRadio_clicked(bool checked) { updateProgressionControls(); }

void AddForcefieldTermsWizard::on_NoMasterTermsCheck_clicked(bool checked) { updateProgressionControls(); }

/*
 * MasterTerms Page
 */

// Row update function for MasterTermsList
void AddForcefieldTermsWizard::updateMasterTermsTreeChild(QTreeWidgetItem *parent, int childIndex, MasterIntra *masterIntra, bool createItem)
{
	QTreeWidgetItem *item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<MasterIntra>(masterIntra));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		parent->insertChild(childIndex, item);
	}
	else
		item = parent->child(childIndex);

	// Set item data
	item->setText(0, masterIntra->name());
	item->setIcon(0, QIcon(dissolveReference_->constCoreData().findMasterTerm(masterIntra->name()) ? ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Update page with MasterTerms in our temporary Dissolve reference
void AddForcefieldTermsWizard::updateMasterTermsPage()
{
	// Update the list against the global MasterTerm tree
	TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> bondUpdater(masterBondItemParent_, temporaryCoreData_.masterBonds(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> angleUpdater(masterAngleItemParent_, temporaryCoreData_.masterAngles(), this, &AddForcefieldTermsWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddForcefieldTermsWizard, MasterIntra> torsionUpdater(masterTorsionItemParent_, temporaryCoreData_.masterTorsions(), this,
										&AddForcefieldTermsWizard::updateMasterTermsTreeChild);

	// Determine whether we have any naming conflicts
	bool conflicts = false;
	ListIterator<MasterIntra> bondIterator(temporaryCoreData_.masterBonds());
	while (MasterIntra *intra = bondIterator.iterate())
		if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
		{
			conflicts = true;
			break;
		}
	ListIterator<MasterIntra> angleIterator(temporaryCoreData_.masterAngles());
	while (MasterIntra *intra = angleIterator.iterate())
		if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
		{
			conflicts = true;
			break;
		}
	ListIterator<MasterIntra> torsionIterator(temporaryCoreData_.masterTorsions());
	while (MasterIntra *intra = torsionIterator.iterate())
		if (dissolveReference_->constCoreData().findMasterTerm(intra->name()))
		{
			conflicts = true;
			break;
		}
	ui_.MasterTermsIndicator->setNotOK(conflicts);
	if (conflicts)
		ui_.MasterTermsIndicatorLabel->setText("One or more MasterTerms in the imported Species conflict with existing ones");
	else
		ui_.MasterTermsIndicatorLabel->setText("There are no naming conflicts with the imported MasterTerms");
}

void AddForcefieldTermsWizard::on_MasterTermsTree_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.MasterTermsTree->selectedItems().count() > 0;
	ui_.MasterTermsPrefixButton->setEnabled(isSelection);
	ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void AddForcefieldTermsWizard::masterTermsTreeEdited(QWidget *lineEdit)
{
	// Since the signal that leads us here does not tell us the item that was edited, update all MasterTerm names here before updating the page
	for (int n = 0; n < masterBondItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem *item = masterBondItemParent_->child(n);
		MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra)
			continue;

		intra->setName(qPrintable(item->text(0)));
	}
	for (int n = 0; n < masterAngleItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem *item = masterAngleItemParent_->child(n);
		MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra)
			continue;

		intra->setName(qPrintable(item->text(0)));
	}
	for (int n = 0; n < masterTorsionItemParent_->childCount(); ++n)
	{
		QTreeWidgetItem *item = masterTorsionItemParent_->child(n);
		MasterIntra *intra = VariantPointer<MasterIntra>(item->data(0, Qt::UserRole));
		if (!intra)
			continue;

		intra->setName(qPrintable(item->text(0)));
	}

	updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsPrefixButton_clicked(bool checked)
{
	bool ok;
	QString prefix = QInputDialog::getText(this, "Prefix MasterTerms", "Enter prefix to apply to all selected MasterTerms", QLineEdit::Normal, "", &ok);
	if (!ok)
		return;

	QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
	QList<QTreeWidgetItem *>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		MasterIntra *intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
		intra->setName(CharString("%s%s", qPrintable(prefix), intra->name()));
	}

	updateMasterTermsPage();
}

void AddForcefieldTermsWizard::on_MasterTermsSuffixButton_clicked(bool checked)
{
	bool ok;
	QString suffix = QInputDialog::getText(this, "Suffix MasterTerms", "Enter suffix to apply to all selected MasterTerms", QLineEdit::Normal, "", &ok);
	if (!ok)
		return;

	QList<QTreeWidgetItem *> selectedItems = ui_.MasterTermsTree->selectedItems();
	QList<QTreeWidgetItem *>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		MasterIntra *intra = VariantPointer<MasterIntra>((*i)->data(0, Qt::UserRole));
		intra->setName(CharString("%s%s", intra->name(), qPrintable(suffix)));
	}

	updateMasterTermsPage();
}
