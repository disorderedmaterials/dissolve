/*
	*** Add Species Wizard Functions
	*** src/gui/addspecieswizard_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/addspecieswizard.h"
#include "gui/helpers/listwidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QInputDialog>

// Constructor / Destructor
AddSpeciesWizard::AddSpeciesWizard(QWidget* parent) : temporaryDissolve_(temporaryCoreData_)
{
	dissolveReference_ = NULL;
	importTarget_ = NULL;

	// Set up our UI, and attach the wizard's widgets to our vacant QFrame
	ui_.setupUi(this);
	setUpHeaderAndFooter(this);

	// Register pages with the wizard
	registerChoicePage(AddSpeciesWizard::StartPage, "Create Species");
	registerPage(AddSpeciesWizard::ImportSpeciesSelectFilePage, "Choose Input File", AddSpeciesWizard::ImportSpeciesSelectSpeciesPage);
	registerPage(AddSpeciesWizard::ImportSpeciesSelectSpeciesPage, "Select Species", AddSpeciesWizard::AtomTypesPage);
	registerPage(AddSpeciesWizard::AtomTypesPage, "Atom Types", AddSpeciesWizard::SpeciesNamePage);
	registerFinishPage(AddSpeciesWizard::SpeciesNamePage, "Species Name");

	// Connect signals / slots
	connect(ui_.AtomTypesList->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(atomTypesListEdited(QWidget*)));

	lockedForRefresh_ = 0;
}

AddSpeciesWizard::~AddSpeciesWizard()
{
}

/*
 * Data
 */

// Copy interaction parameters, adding MasterIntra if necessary
void AddSpeciesWizard::copyIntra(SpeciesIntra* sourceIntra, SpeciesIntra* destIntra, Dissolve& mainDissolve)
{
	// We can always copy the form of the interaction, regardless of whether it is a MasterIntra or not

	// If sourceIntra referneces a MasterIntra, check for its presence in the supplied Dissolve reference, and create it if necessary
	if (sourceIntra->masterParameters())
	{
		// Search for MasterIntra by the same name in our main Dissolve instance
		MasterIntra* master = NULL;
		if (sourceIntra->type() == SpeciesIntra::IntramolecularBond)
		{
			master = mainDissolve.hasMasterBond(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = mainDissolve.addMasterBond(sourceIntra->masterParameters()->name());
				master->setParametersFromArray(sourceIntra->parametersAsArray());
			}
		}
		else if (sourceIntra->type() == SpeciesIntra::IntramolecularAngle)
		{
			master = mainDissolve.hasMasterAngle(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = mainDissolve.addMasterAngle(sourceIntra->masterParameters()->name());
				master->setParametersFromArray(sourceIntra->parametersAsArray());
			}
		}
		else if (sourceIntra->type() == SpeciesIntra::IntramolecularTorsion)
		{
			master = mainDissolve.hasMasterTorsion(sourceIntra->masterParameters()->name());
			if (!master)
			{
				master = mainDissolve.addMasterTorsion(sourceIntra->masterParameters()->name());
				master->setParametersFromArray(sourceIntra->parametersAsArray());
			}
		}

		master->setForm(sourceIntra->masterParameters()->form());

		// Set the master pointer in the interaction
		destIntra->setMasterParameters(master);
	}
	else
	{
		// Just copy over form / parameters
		destIntra->setForm(sourceIntra->form());
		destIntra->setParametersFromArray(sourceIntra->parametersAsArray());
	}
}


// Set Dissolve reference
void AddSpeciesWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Copy imported Species over to the specified Dissolve object
bool AddSpeciesWizard::importSpecies(Dissolve& dissolve)
{
	// Check that we have a target Species (in temporaryDissolve_) to import
	if (!importTarget_) return Messenger::error("No target Species to import!\n");

	// Create our new Species
	Species* newSpecies = dissolve.addSpecies();
	newSpecies->setName(speciesName());

	// Duplicate atoms
	ListIterator<SpeciesAtom> atomIterator(importTarget_->atoms());
	while (SpeciesAtom* i = atomIterator.iterate())
	{
		// Create the Atom in our new Species
		SpeciesAtom* newAtom = newSpecies->addAtom(i->element(), i->r());

		// Search for the existing atom's AtomType by name, and create it if it doesn't exist
		if (i->atomType())
		{
			AtomType* at = dissolve.findAtomType(i->atomType()->name());
			if (!at)
			{
				at = dissolve.addAtomType(i->element());
				at->setName(i->atomType()->name());
				at->parameters() = i->atomType()->parameters();
			}

			newAtom->setAtomType(at);
		}
	}

	// Duplicate bonds
	ListIterator<SpeciesBond> bondIterator(importTarget_->bonds());
	while (SpeciesBond* b = bondIterator.iterate())
	{
		// Create the bond in the new Species
		SpeciesBond* newBond = newSpecies->addBond(b->indexI(), b->indexJ());

		// Copy interaction parameters, including MasterIntra if necessary
		copyIntra(b, newBond, dissolve);
	}

	// Duplicate angles
	ListIterator<SpeciesAngle> angleIterator(importTarget_->angles());
	while (SpeciesAngle* a = angleIterator.iterate())
	{
		// Create the angle in the new Species
		SpeciesAngle* newAngle = newSpecies->addAngle(a->indexI(), a->indexJ(), a->indexK());

		// Copy interaction parameters, including MasterIntra if necessary
		copyIntra(a, newAngle, dissolve);
	}

	// Duplicate torsions
	ListIterator<SpeciesTorsion> torsionIterator(importTarget_->torsions());
	while (SpeciesTorsion* t = torsionIterator.iterate())
	{
		// Create the torsion in the new Species
		SpeciesTorsion* newTorsion = newSpecies->addTorsion(t->indexI(), t->indexJ(), t->indexK(), t->indexL());

		// Copy interaction parameters, including MasterIntra if necessary
		copyIntra(t, newTorsion, dissolve);
	}

	// Finalise the new Species
	newSpecies->updateUsedAtomTypes();
	newSpecies->addNaturalIsotopologue(dissolve.atomTypes());
	newSpecies->updateGrains();
	newSpecies->centreAtOrigin();
	newSpecies->orderAtomsWithinGrains();
}

/*
 * Controls
 */

// Go to specified page index in the controls widget
bool AddSpeciesWizard::displayControlPage(int index)
{
	// Check page index given
	if ((index < 0) || (index >= AddSpeciesWizard::nPages)) return Messenger::error("Page index %i not recognised.\n", index);

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
bool AddSpeciesWizard::progressionAllowed(int index) const
{
	// Check widget validity in the specified page, returning if progression (i.e. pushing 'Next' or 'Finish') is allowed
	switch (index)
	{
		case (AddSpeciesWizard::ImportSpeciesSelectFilePage):
			return ((!ui_.InputFileEdit->text().isEmpty()) && (QFile::exists(ui_.InputFileEdit->text())));
		case (AddSpeciesWizard::ImportSpeciesSelectSpeciesPage):
			return (ui_.SpeciesList->currentRow() != -1);
		case (AddSpeciesWizard::SpeciesNamePage):
			return (ui_.SpeciesNameIndicator->state());
		default:
			break;
	}

	return true;
}

// Perform any necssary actions before moving to the next page
bool AddSpeciesWizard::prepareForNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddSpeciesWizard::ImportSpeciesSelectFilePage):
			// Check that the input/species file exists, and can be read in successfully
			if (!temporaryDissolve_.loadInput(qPrintable(ui_.InputFileEdit->text())))
			{
				return false;
			}
			// Update the Species and AtomTypes lists
			ui_.SpeciesList->clear();
			for (Species* sp = temporaryDissolve_.species().first(); sp != NULL; sp = sp->next)
			{
				QListWidgetItem* item = new QListWidgetItem(sp->name());
				item->setData(Qt::UserRole, VariantPointer<Species>(sp));
				ui_.SpeciesList->addItem(item);
			}
			if (ui_.SpeciesList->count() > 0)
			{
				ui_.SpeciesList->setCurrentRow(0);
				importTarget_ = temporaryDissolve_.species().first();
			}

			updateAtomTypesPage();
			break;
		default:
			break;
	}

	return true;
}

// Perform any necssary actions before moving to the previous page
bool AddSpeciesWizard::prepareForPreviousPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddSpeciesWizard::ImportSpeciesSelectSpeciesPage):
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

// Reset widget, ready for adding a new Species
void AddSpeciesWizard::reset()
{
	// Reset the underlying WizardWidget
	resetToPage(AddSpeciesWizard::StartPage);

	// Clear our temporary Dissolve instance
	temporaryDissolve_.clear();

	// Set a new, unique name ready on the final page
	ui_.SpeciesNameEdit->setText(dissolveReference_->coreData().uniqueSpeciesName("NewSpecies"));
}

/*
 * Start Page
 */

void AddSpeciesWizard::on_StartCreateEmptyButton_clicked(bool checked)
{
	// Create an empty Species in our temporary data
	importTarget_ = temporaryCoreData_.addSpecies();

	// Done - go to the final, naming page
	goToPage(AddSpeciesWizard::SpeciesNamePage);
}

void AddSpeciesWizard::on_StartCreateAtomicButton_clicked(bool checked)
{
}

void AddSpeciesWizard::on_StartAddPredefinedButton_clicked(bool checked)
{
}

void AddSpeciesWizard::on_StartImportCoordinatesButton_clicked(bool checked)
{
}

void AddSpeciesWizard::on_StartImportSpeciesButton_clicked(bool checked)
{
	// Go to the input file selector
	goToPage(AddSpeciesWizard::ImportSpeciesSelectFilePage);
}

/*
 * Import Species Pages
 */

void AddSpeciesWizard::on_InputFileEdit_textChanged(const QString text)
{
	updateProgressionControls();
}

void AddSpeciesWizard::on_InputFileSelectButton_clicked(bool checked)
{
	QString inputFile = QFileDialog::getOpenFileName(this, "Choose species / input file to open", QDir().absolutePath(), "Dissolve input / species files (*.txt *.dsp)");
	if (inputFile.isEmpty()) return;

	// Set input file in edit
	ui_.InputFileEdit->setText(inputFile);

	updateProgressionControls();
}

void AddSpeciesWizard::on_SpeciesList_currentRowChanged(int currentRow)
{
	// Set import target from current row
	if (currentRow == -1) importTarget_ = NULL;
	else importTarget_ = VariantPointer<Species>(ui_.SpeciesList->currentItem()->data(Qt::UserRole));

	updateProgressionControls();
}

/*
 * AtomTypes Page
 */

// Row update function for AtomTypesList
void AddSpeciesWizard::updateAtomTypesListRow(int row, AtomType* atomType, bool createItem)
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
void AddSpeciesWizard::updateAtomTypesPage()
{
	// Update the list against the global AtomType list
	ListWidgetUpdater<AddSpeciesWizard,AtomType> listUpdater(ui_.AtomTypesList, temporaryCoreData_.constAtomTypes(), this, &AddSpeciesWizard::updateAtomTypesListRow);
}

void AddSpeciesWizard::on_AtomTypesList_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.AtomTypesList->selectedItems().count() > 0;
	ui_.AtomTypesPrefixButton->setEnabled(isSelection);
	ui_.AtomTypesSuffixButton->setEnabled(isSelection);
}

void AddSpeciesWizard::atomTypesListEdited(QWidget* lineEdit)
{
	// Since the signal that leads us here does not tell us the item that was edited, update all AtomType names here before updating the page
	for (int n=0; n<ui_.AtomTypesList->count(); ++n)
	{
		QListWidgetItem* item = ui_.AtomTypesList->item(n);
		AtomType* at = (AtomType*) VariantPointer<AtomType>(item->data(Qt::UserRole));
		if (!at) continue;

		at->setName(qPrintable(item->text()));
	}

	updateAtomTypesPage();
}

void AddSpeciesWizard::on_AtomTypesPrefixButton_clicked(bool checked)
{
	bool ok;
	QString prefix = QInputDialog::getText(this, "Prefix AtomTypes", "Enter prefix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QListWidgetItem*> selectedItems = ui_.AtomTypesList->selectedItems();
	QList<QListWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		AtomType* at = (AtomType*) VariantPointer<AtomType>((*i)->data(Qt::UserRole));
		at->setName(CharString("%s%s", qPrintable(prefix), at->name()));
	}

	updateAtomTypesPage();
}

void AddSpeciesWizard::on_AtomTypesSuffixButton_clicked(bool checked)
{
	bool ok;
	QString suffix = QInputDialog::getText(this, "Suffix AtomTypes", "Enter suffix to apply to all selected AtomTypes", QLineEdit::Normal, "", &ok);
	if (!ok) return;

	QList<QListWidgetItem*> selectedItems = ui_.AtomTypesList->selectedItems();
	QList<QListWidgetItem*>::iterator i;
	for (i = selectedItems.begin(); i != selectedItems.end(); ++i)
	{
		AtomType* at = (AtomType*) VariantPointer<AtomType>((*i)->data(Qt::UserRole));
		at->setName(CharString("%s%s", at->name(), qPrintable(suffix)));
	}

	updateAtomTypesPage();
}

/*
 * Species Name Page
 */

// Species name edited
void AddSpeciesWizard::on_SpeciesNameEdit_textChanged(const QString text)
{
	if (!dissolveReference_)
	{
		printf("Internal Error: Dissolve pointer has not been set in AddSpeciesWizard.\n");
		return;
	}

	// Name of the prospective Species has been changed, so make sure it is valid
	bool readyForImport;
	if (text.isEmpty()) readyForImport = false;
	else readyForImport = dissolveReference_->findSpecies(qPrintable(text)) == NULL;

	ui_.SpeciesNameIndicator->setState(readyForImport);

	// Update state of progression controls
	updateProgressionControls();
}

// Return name of new Species to be
const char* AddSpeciesWizard::speciesName() const
{
	return qPrintable(ui_.SpeciesNameEdit->text());
}
