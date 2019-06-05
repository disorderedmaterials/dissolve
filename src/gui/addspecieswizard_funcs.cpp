/*
	*** Add Species Wizard Functions
	*** src/gui/addspecieswizard_funcs.cpp
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

#include "gui/addspecieswizard.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
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
	registerChoicePage(AddSpeciesWizard::StartPage, "Create Species");
	registerPage(AddSpeciesWizard::CreateAtomicPage, "Create Atomic Species", AddSpeciesWizard::SpeciesNamePage);
	registerPage(AddSpeciesWizard::ImportSpeciesSelectFilePage, "Choose Input File", AddSpeciesWizard::ImportSpeciesSelectSpeciesPage);
	registerPage(AddSpeciesWizard::ImportSpeciesSelectSpeciesPage, "Select Species", AddSpeciesWizard::AtomTypesPage);
	registerPage(AddSpeciesWizard::AtomTypesPage, "Atom Types");
	registerPage(AddSpeciesWizard::MasterTermsPage, "Master Terms", AddSpeciesWizard::SpeciesNamePage);
	registerFinishPage(AddSpeciesWizard::SpeciesNamePage, "Species Name");

	// Connect signals / slots
	connect(ui_.AtomTypesList->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(atomTypesListEdited(QWidget*)));
	connect(ui_.MasterTermsTree->itemDelegate(), SIGNAL(commitData(QWidget*)), this, SLOT(masterTermsTreeEdited(QWidget*)));
	connect(ui_.CreateAtomicElementSelector, SIGNAL(elementSelectionChanged()), this, SLOT(createAtomicElementChanged()));
	if (footerAvailable()) connect(ui_.CreateAtomicElementSelector, SIGNAL(elementDoubleClicked()), this, SLOT(goToNextPage()));

	lockedForRefresh_ = 0;
}

AddSpeciesWizard::~AddSpeciesWizard()
{
}

/*
 * Data
 */

// Set Dissolve reference
void AddSpeciesWizard::setMainDissolveReference(const Dissolve* dissolveReference)
{
	dissolveReference_ = dissolveReference;
}

// Copy imported Species over to the specified Dissolve object
Species* AddSpeciesWizard::importSpecies(Dissolve& dissolve)
{
	// Check that we have a target Species (in temporaryDissolve_) to import
	if (!importTarget_)
	{
		Messenger::error("No target Species to import!\n");
		return NULL;
	}

	// Copy the importTarget_ over
	Species* newSpecies = dissolve.copySpecies(importTarget_);

	return newSpecies;
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
		case (AddSpeciesWizard::CreateAtomicPage):
			return (ui_.CreateAtomicElementSelector->currentElement());
		case (AddSpeciesWizard::ImportSpeciesSelectFilePage):
			return ((!ui_.InputFileEdit->text().isEmpty()) && (QFile::exists(ui_.InputFileEdit->text())));
		case (AddSpeciesWizard::ImportSpeciesSelectSpeciesPage):
			return (ui_.SpeciesList->currentRow() != -1);
		case (AddSpeciesWizard::SpeciesNamePage):
			return (ui_.SpeciesNameIndicator->state() == CheckIndicator::OKState);
		default:
			break;
	}

	return true;
}

// Perform any necessary actions before moving to the next page
bool AddSpeciesWizard::prepareForNextPage(int currentIndex)
{
	SpeciesAtom* atomicAtom;

	switch (currentIndex)
	{
		case (AddSpeciesWizard::CreateAtomicPage):
			// Create our atomic species here
			importTarget_ = temporaryDissolve_.addSpecies();
			atomicAtom = importTarget_->addAtom(ui_.CreateAtomicElementSelector->currentElement(), Vec3<double>());

			// Add associated AtomType?
			if (ui_.CreateAtomicAddAtomTypeCheck->isChecked())
			{
				AtomType* at = temporaryDissolve_.addAtomType(ui_.CreateAtomicElementSelector->currentElement());
				at->setName(dissolveReference_->coreData().uniqueAtomTypeName(ui_.CreateAtomicElementSelector->currentElement()->symbol()));
				atomicAtom->setAtomType(at);
			}

			// Set a suitable name
			ui_.SpeciesNameEdit->setText(dissolveReference_->coreData().uniqueSpeciesName(ui_.CreateAtomicElementSelector->currentElement()->symbol()));
			break;
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
			updateMasterTermsPage();
			break;
		default:
			break;
	}

	return true;
}

// Determine next page for the current page, based on current data
int AddSpeciesWizard::determineNextPage(int currentIndex)
{
	switch (currentIndex)
	{
		case (AddSpeciesWizard::AtomTypesPage):
			// If there are master terms present, go to that page first. Otherwise, skip straight to naming
			if (temporaryDissolve_.nMasterBonds() || temporaryDissolve_.nMasterAngles() || temporaryDissolve_.nMasterTorsions()) return AddSpeciesWizard::MasterTermsPage;
			else return AddSpeciesWizard::SpeciesNamePage;
		default:
			break;
	}

	return -1;
}

// Perform any necessary actions before moving to the previous page
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
	goToPage(AddSpeciesWizard::CreateAtomicPage);
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
 * Create Atomic Page
 */

void AddSpeciesWizard::createAtomicElementChanged()
{
	updateProgressionControls();
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
	else
	{
		importTarget_ = VariantPointer<Species>(ui_.SpeciesList->currentItem()->data(Qt::UserRole));
		ui_.SpeciesNameEdit->setText(importTarget_->name());
	}

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
		AtomType* at = VariantPointer<AtomType>(item->data(Qt::UserRole));
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
		AtomType* at = VariantPointer<AtomType>((*i)->data(Qt::UserRole));
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
		AtomType* at = VariantPointer<AtomType>((*i)->data(Qt::UserRole));
		at->setName(CharString("%s%s", at->name(), qPrintable(suffix)));
	}

	updateAtomTypesPage();
}

/*
 * MasterTerms Page
 */

// Row update function for MasterTermsList
void AddSpeciesWizard::updateMasterTermsTreeChild(QTreeWidgetItem* parent, int childIndex, MasterIntra* masterIntra, bool createItem)
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
	item->setIcon(0, QIcon(dissolveReference_->findMasterTerm(masterIntra->name()) ?  ":/general/icons/general_warn.svg" : ":/general/icons/general_true.svg"));
}

// Update page with MasterTerms in our temporary Dissolve reference
void AddSpeciesWizard::updateMasterTermsPage()
{
	// Update the list against the global MasterTerm tree
	TreeWidgetUpdater<AddSpeciesWizard,MasterIntra> bondUpdater(masterBondItemParent_, temporaryDissolve_.masterBonds(), this, &AddSpeciesWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddSpeciesWizard,MasterIntra> angleUpdater(masterAngleItemParent_, temporaryDissolve_.masterAngles(), this, &AddSpeciesWizard::updateMasterTermsTreeChild);
	TreeWidgetUpdater<AddSpeciesWizard,MasterIntra> torsionUpdater(masterTorsionItemParent_, temporaryDissolve_.masterTorsions(), this, &AddSpeciesWizard::updateMasterTermsTreeChild);

	// Determine whether we have any naming conflicts
	bool conflicts = false;
	ListIterator<MasterIntra> bondIterator(temporaryDissolve_.masterBonds());
	while (MasterIntra* intra = bondIterator.iterate()) if (dissolveReference_->findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ListIterator<MasterIntra> angleIterator(temporaryDissolve_.masterAngles());
	while (MasterIntra* intra = angleIterator.iterate()) if (dissolveReference_->findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ListIterator<MasterIntra> torsionIterator(temporaryDissolve_.masterTorsions());
	while (MasterIntra* intra = torsionIterator.iterate()) if (dissolveReference_->findMasterTerm(intra->name()))
	{
		conflicts = true;
		break;
	}
	ui_.MasterTermsIndicator->setNotOK(conflicts);
	if (conflicts) ui_.MasterTermsIndicatorLabel->setText("One or more MasterTerms in the imported Species conflict with existing ones");
	else ui_.MasterTermsIndicatorLabel->setText("There are no naming conflicts with the imported MasterTerms");
}

void AddSpeciesWizard::on_MasterTermsTree_itemSelectionChanged()
{
	// Enable / disable prefix and suffix buttons as appropriate
	bool isSelection = ui_.MasterTermsTree->selectedItems().count() > 0;
	ui_.MasterTermsPrefixButton->setEnabled(isSelection);
	ui_.MasterTermsSuffixButton->setEnabled(isSelection);
}

void AddSpeciesWizard::masterTermsTreeEdited(QWidget* lineEdit)
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

void AddSpeciesWizard::on_MasterTermsPrefixButton_clicked(bool checked)
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

void AddSpeciesWizard::on_MasterTermsSuffixButton_clicked(bool checked)
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

	ui_.SpeciesNameIndicator->setOK(readyForImport);

	// Update state of progression controls
	updateProgressionControls();
}

// Return name of new Species to be
const char* AddSpeciesWizard::speciesName() const
{
	return qPrintable(ui_.SpeciesNameEdit->text());
}
