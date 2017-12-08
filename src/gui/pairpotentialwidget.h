/*
	*** PairPotential Widget
	*** src/gui/pairpotentialwidget.h
	Copyright T. Youngs 2007-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_PAIRPOTENTIALWIDGET_H
#define DUQ_PAIRPOTENTIALWIDGET_H

#include "gui/ui_pairpotentialwidget.h"
#include "gui/subwidget.h"

// Forward Declarations
class Collection;
class DUQ;
class PairPotential;
class UChromaViewWidget;

// PairPotential Widget
class PairPotentialWidget : public SubWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated PairPotential
	PairPotential* pairPotential_;
	// Reference to dUQ
	DUQ& duq_;
	// UChromaView contained within this widget
	UChromaViewWidget* uChromaView_;


	public:
	// Constructor / Destructor
	PairPotentialWidget(QWidget* parent, PairPotential* pp, DUQ& dUQ);
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
	void windowClosed(void*);

	private slots:
	void on_FullEnergyCheck_clicked(bool checked);
	void on_OriginalEnergyCheck_clicked(bool checked);
	void on_AdditionalEnergyCheck_clicked(bool checked);
	void on_FullForceCheck_clicked(bool checked);
};

#endif
