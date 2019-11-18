/*
	*** Integrator1D Gizmo
	*** src/gui/gizmos/integrator1d.h
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

#ifndef DISSOLVE_GIZMOS_INTEGRATOR1D_H
#define DISSOLVE_GIZMOS_INTEGRATOR1D_H

#include "gui/gizmos/ui_integrator1d.h"
#include "gui/gizmos/gizmo.h"

// Forward Declarations
class Collection;
class Dissolve;
class PairPotential;
class DataWidget;

// Integrator1D Gizmo
class Integrator1DGizmo : public QWidget, public Gizmo
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	Integrator1DGizmo(DissolveWindow* dissolveWindow);
	~Integrator1DGizmo();


	/*
	 * Core
	 */
	public:
	// Return string specifying Gizmo type
	const char* type() const;


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::Integrator1DGizmo ui_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget
	void disableSensitiveControls();
	// Enable sensitive controls within widget
	void enableSensitiveControls();


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widget Signals / Slots
	 */
// 	signals:
// 	void windowClosed(QString windowTitle);

// 	private slots:
};

#endif
