/*
	*** Add Configuration Wizard
	*** src/gui/addconfigurationwizard.h
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

#ifndef DISSOLVE_ADDCONFIGURATIONWIZARD_H
#define DISSOLVE_ADDCONFIGURATIONWIZARD_H

#include "gui/ui_addconfigurationwizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
/* none */

// Add Configuration Wizard
class AddConfigurationWizard : public WizardWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	AddConfigurationWizard(QWidget* parent);
	~AddConfigurationWizard();


	/*
	 * Data
	 */
	private:
	// Main instance of Dissolve that we're using as a reference
	const Dissolve* dissolveReference_;
	// Target Configuration for import
	Configuration* importTarget_;

	public:
	// Set Dissolve reference
	void setMainDissolveReference(const Dissolve* dissolveReference);
	// Get Box details from controls and put into target Configuration
	void getBoxDetails();
	// Move constructed Configuration over to the specified Dissolve object, returning the new pointer to it
	Configuration* importConfiguration(Dissolve& dissolve);


	/*
	 * Controls
	 */
	private:
	// Main form declaration
	Ui::AddConfigurationWizard ui_;
	// Lock counter for the widget refreshing
	int lockedForRefresh_;
	// Pages Enum
	enum WidgetPage
	{
		StartPage,			/* Starting page, offering choices of how to proceed */
		MonoSpeciesPage,		/* Create from single Species */
		MultiSpeciesPage,		/* Create from multiple Species */
		BoxTypePage,			/* Choose Box type */
		BoxGeometryPage,		/* Set relative box lengths and absolute angles */
		MultiplierPage,			/* Multiplier page, with calculation of final nAtoms etc. */
		FinishPage,			/* Final page, setting name and temperature for Configuration */
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
	// Reset, ready for adding a new Configuration
	void reset();


	/*
	 * Start Page
	 */
	private slots:
	void on_StartCreateEmptyButton_clicked(bool checked);
	void on_StartMonoSpeciesButton_clicked(bool checked);
	void on_StartMultiSpeciesButton_clicked(bool checked);


	/*
	 * Multiplier Page
	 */
	private:
	// Species population row update function
	void updatePopulationTableRow(int row, Species* sp, int population, bool createItems);
	// Update controls on MultiplierPage
	void updateMultiplierPage();

	private slots:
	void on_MultiplierSpin_valueChanged(int value);


	/*
	 * Configuration Name Page (final page)
	 */
	private slots:
	void on_FinishNameEdit_textChanged(const QString text);
};

#endif
