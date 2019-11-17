/*
	*** Gizmo
	*** src/gui/gizmos/gizmo.h
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

#ifndef DISSOLVE_GIZMO_H
#define DISSOLVE_GIZMO_H

#include "base/charstring.h"
#include "templates/listitem.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class DissolveWindow;
class LineParser;
class QMdiSubWindow;

// Gizmo - Base  class for any widget to be displayed in a QMdiSubWindow
class Gizmo : public ListItem<Gizmo>
{
	public:
	// Constructor / Destructor
	Gizmo(DissolveWindow* dissolveWindow);
	virtual ~Gizmo();


	/*
	 * Core
	 */
	protected:
	// Title of widget
	CharString title_;
	// Pointer to DissolveWindow
	DissolveWindow* dissolveWindow_;
	// Reference to Dissolve
	Dissolve& dissolve_;
	// QMdiSubWindow containing the Gizmo
	QMdiSubWindow* window_;

	public:
	// Return string specifying Gizmo type
	virtual const char* type() const = 0;
	// Set title of widget
	void setTitle(const char* title);
	// Return title of widget
	const char* title();
	// Set QMdiSubWindow containing the Gizmo
	void setWindow(QMdiSubWindow* window);
	// Return QMdiSubWindow containing the Gizmo
	QMdiSubWindow* window();


	/*
	 * UI
	 */
	protected:
	// Whether the gizmo is currently refreshing
	bool refreshing_;

	protected:
	// Window close event
	virtual void closeEvent(QCloseEvent* event) = 0;

	public:
	// Update controls within widget
	virtual void updateControls() = 0;
	// Disable sensitive controls within widget
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls within widget
	virtual void enableSensitiveControls() = 0;


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;
};

#endif
