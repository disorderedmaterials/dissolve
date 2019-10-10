/*
	*** Add Forcefield Terms Wizard
	*** src/gui/addforcefieldtermswizard.h
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

#ifndef DISSOLVE_WIZARD_ADDFORCEFIELDTERMS_H
#define DISSOLVE_WIZARD_ADDFORCEFIELDTERMS_H

#include "gui/ui_addforcefieldtermswizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
class Forcefield;

// Add Forcefield Terms Wizard
class AddForcefieldTermsWizard : public WizardWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	AddForcefieldTermsWizard(QWidget* parent);
	~AddForcefieldTermsWizard();


	/*
	 * Data
	 */
	private:
	// Main instance of Dissolve that we're using as a reference
	const Dissolve* dissolveReference_;
	// Temporary core data for applying Forcefield terms
	CoreData temporaryCoreData_;
	// Temporary Dissolve reference for creating / importing layers
	Dissolve temporaryDissolve_;
	// Target Species that we are acquiring forcefield terms for
	Species* targetSpecies_;
	// Species pointer with newly-applied Forcefield terms
	Species* modifiedSpecies_;

	public:
	// Set Dissolve reference
	void setMainDissolveReference(const Dissolve* dissolveReference);
	// Set target Species that we are acquiring forcefield terms for
	void setTargetSpecies(Species* sp);
	// Apply our Forcefield terms to the targetted Species within the specified Dissolve object
	bool applyForcefieldTerms(Dissolve& dissolve);


	/*
	 * Controls
	 */
	private:
	// Main form declaration
	Ui::AddForcefieldTermsWizard ui_;
	// Lock counter for the widget refreshing
	int lockedForRefresh_;
	// Pages Enum
	enum WidgetPage
	{
		SelectForcefieldPage,		/* Select Forcefield to apply to Species */
		SelectTermsPage,		/* Select terms to generate */
		AtomTypesPage,			/* AtomTypes page - check / re-map AtomTypes */
		MasterTermsPage,		/* MasterTerms page - check / re-map MasterTerms */
		FinishPage,			/* Finish page */
		nPages
	};

	protected:
	// Go to specified page index in the controls widget
	bool displayControlPage(int index);
	// Return whether progression to the next page from the current page is allowed
	bool progressionAllowed(int index) const;
	// Perform any necessary actions before moving to the next page
	bool prepareForNextPage(int currentIndex);
	// Determine next page for the current page, based on current data
	int determineNextPage(int currentIndex);
	// Perform any necessary actions before moving to the previous page
	bool prepareForPreviousPage(int currentIndex);

	public:
	// Reset, ready for adding new forcefield terms
	void reset();


	/*
	 * Select Forcefield Page
	 */
	private slots:
	// ForcefieldList row update function
	void updateForcefieldListRow(int row, Forcefield* ff, bool createItem);
	// Update Select Forcefield page
	void updateSelectForcefieldPage();
	// Return currently-selected Forcefield
	Forcefield* currentForcefield() const;

	private slots:
	void on_ForcefieldList_currentRowChanged(int row);


	/*
	 * Select Terms Page
	 */
	private:


	/*
	 * AtomTypes Page
	 */
	private:
	// Row update function for AtomTypesList
	void updateAtomTypesListRow(int row, AtomType* atomType, bool createItem);
	// Update page with AtomTypes in our temporary Dissolve reference
	void updateAtomTypesPage();

	private slots:
	void on_AtomTypesList_itemSelectionChanged();
	void atomTypesListEdited(QWidget* lineEdit);
	void on_AtomTypesPrefixButton_clicked(bool checked);
	void on_AtomTypesSuffixButton_clicked(bool checked);


	/*
	 * MasterTerms Page
	 */
	private:
	// Parental tree widgets for master terms
	QTreeWidgetItem* masterBondItemParent_, *masterAngleItemParent_, *masterTorsionItemParent_;
	
	private:
	// Row update function for MasterTermsTree
	void updateMasterTermsTreeChild(QTreeWidgetItem* parent, int childIndex, MasterIntra* masterIntra, bool createItem);
	void updateMasterTermsPage();

	private slots:
	void on_MasterTermsTree_itemSelectionChanged();
	void masterTermsTreeEdited(QWidget* lineEdit);
	void on_MasterTermsPrefixButton_clicked(bool checked);
	void on_MasterTermsSuffixButton_clicked(bool checked);
};

#endif
