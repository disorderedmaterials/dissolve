/*
	*** Add Processing Layer Wizard
	*** src/gui/addprocessinglayerwizard.h
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

#ifndef DISSOLVE_ADDPROCESSINGLAYERWIZARD_H
#define DISSOLVE_ADDPROCESSINGLAYERWIZARD_H

#include "gui/ui_addprocessinglayerwizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
/* none */

// Add Processing Layer Wizard
class AddProcessingLayerWizard : public WizardWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	AddProcessingLayerWizard(QWidget* parent);
	~AddProcessingLayerWizard();


	/*
	 * Data
	 */
	private:
	// Main instance of Dissolve that we're using as a reference
	const Dissolve* dissolveReference_;
	// Temporary core data for creating / importing layers
	CoreData temporaryCoreData_;
	// Temporary Dissolve reference for creating / importing layers
	Dissolve temporaryDissolve_;
	// Target ModuleLayer for import
	ModuleLayer* importTarget_;

	public:
	// Set Dissolve reference
	void setMainDissolveReference(const Dissolve* dissolveReference);
	// Move created ModuleLayer to the specified Dissolve object, returning the new pointer to it
	ModuleLayer* importModuleLayer(Dissolve& dissolve);


	/*
	 * Controls
	 */
	private:
	// Main form declaration
	Ui::AddProcessingLayerWizard ui_;
	// Lock counter for the widget refreshing
	int lockedForRefresh_;
	// Pages Enum
	enum WidgetPage
	{
		StartPage,			/* Starting page, offering choices of how to proceed */
		FinishPage,			/* Final page, setting name for layer */
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
	// Reset, ready for adding a new layer
	void reset();


	/*
	 * Start Page
	 */
	private slots:
	void on_StartCreateEmptyButton_clicked(bool checked);
	void on_StartAddPredefinedButton_clicked(bool checked);
	void on_StartImportButton_clicked(bool checked);


	/*
	 * Layer Name Page (final page)
	 */
	private slots:
	void on_FinishNameEdit_textChanged(const QString text);
};

#endif
