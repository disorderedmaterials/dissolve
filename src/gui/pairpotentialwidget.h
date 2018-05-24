/*
	*** PairPotential Widget
	*** src/gui/pairpotentialwidget.h
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

#ifndef DISSOLVE_PAIRPOTENTIALWIDGET_H
#define DISSOLVE_PAIRPOTENTIALWIDGET_H

#include "gui/ui_pairpotentialwidget.h"
#include "gui/widgets/subwidget.h"

// Forward Declarations
class Collection;
class Dissolve;
class PairPotential;
class UChromaViewWidget;

// PairPotential Widget
class PairPotentialWidget : public SubWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Reference to Dissolve
	Dissolve& dissolve_;
	// Index of displayed PairPotential
	int pairPotentialIndex_;
	// Last displayed pair potential
	PairPotential* lastPairPotential_;
	// UChromaView contained within this widget
	UChromaViewWidget* uChromaView_;

	private:
	// Set data targets for specified PairPotential in UChromaView
	void setDataTargets(PairPotential* pp);

	public:
	// Constructor / Destructor
	PairPotentialWidget(DissolveWindow* dissolveWindow, const char* title);
	~PairPotentialWidget();
	// Main form declaration
	Ui::PairPotentialWidget ui;
	// Initialise window
	void initialiseWindow(PairPotential* pp);
	// Initialise controls
	void initialiseControls(PairPotential* pp, bool addDefaults);


	/*
	 * SubWidget Reimplementations / Definitions
	 */
	protected:
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();
	// Return string specifying widget type
	const char* widgetType();
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widget Signals / Slots
	 */
	signals:
	void windowClosed(QString windowTitle);

	private slots:
	void on_PreviousPotentialButton_clicked(bool checked);
	void on_NextPotentialButton_clicked(bool checked);
	void on_FullEnergyCheck_clicked(bool checked);
	void on_OriginalEnergyCheck_clicked(bool checked);
	void on_AdditionalEnergyCheck_clicked(bool checked);
	void on_FullForceCheck_clicked(bool checked);
	void on_ResetGraphButton_clicked(bool checked);
	void on_ZeroUAdditionalButton_clicked(bool checked);
};

#endif
