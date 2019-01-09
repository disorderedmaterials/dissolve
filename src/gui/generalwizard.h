/*
	*** General Wizard
	*** src/gui/generalwizard.h
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

#ifndef DISSOLVE_GENERALWIZARD_H
#define DISSOLVE_GENERALWIZARD_H

#include "gui/ui_generalwizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
class DissolveWindow;

// General Wizard
class GeneralWizard : public WizardWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	GeneralWizard(QWidget* parent);
	~GeneralWizard();


	/*
	 * Data
	 */
	private:
	// Main instance of Dissolve that we're using as a reference
	const Dissolve* dissolveReference_;
	// Main UI object
	DissolveWindow* dissolveWindow_;

	public:
	// Set Dissolve reference
	void setMainDissolveReference(const Dissolve* dissolveReference);
	// Set main GUI window pointer
	void setDissolveWindow(DissolveWindow* dissolveWindow);


	/*
	 * Controls
	 */
	private:
	// Main form declaration
	Ui::GeneralWizard ui_;
	// Lock counter for the widget refreshing
	int lockedForRefresh_;

	private:
	// Highlight specified widget
	void setWidgetHighlight(QWidget* widget, int r, int g, int b, int a = 255);
	// Remove highlight from specified widget
	void removeWidgetHighlight(QWidget* widget);

	protected:
	// Go to specified page index in the controls widget
	bool displayControlPage(int index);
	// Return whether progression to the next page from the current page is allowed
	bool progressionAllowed(int index) const;
	// Perform any necssary actions before moving to the next page
	bool prepareForNextPage(int currentIndex);
	// Perform any necssary actions before moving to the previous page
	bool prepareForPreviousPage(int currentIndex);

	public:
	// Reset wizard, making it ready to run again
	void reset();


	/*
	 * Content Management
	 */
	private:
	// Tag for start page
	CharString startPageTag_;
	// Index for start page
	int startPageIndex_;

	public:
	// Clear current content
	void clear();
	// Set up specified content (from resource)
	bool setContent(QString resourceName);
};

#endif
