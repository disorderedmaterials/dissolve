/*
	*** Gizmo
	*** src/gui/gizmo.h
	Copyright T. Youngs 2012-2020

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
#include "base/sysfunc.h"
#include "templates/listitem.h"
#include "templates/reflist.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class LineParser;
class QMdiSubWindow;

// Gizmo - Base class for any widget to be displayed in a QMdiSubWindow
class Gizmo : public ListItem<Gizmo>
{
	public:
	// Constructor / Destructor
	Gizmo(Dissolve& dissolve, const char* uniqueName);
	virtual ~Gizmo();


	/*
	 * Core
	 */
	protected:
	// Unique name of gizmo
	CharString uniqueName_;
	// Reference to Dissolve
	Dissolve& dissolve_;
	// QMdiSubWindow containing the Gizmo
	QMdiSubWindow* window_;
	// Reference list of all Gizmos
	static RefList<Gizmo> allGizmos_;

	public:
	// Return string specifying Gizmo type
	virtual const char* type() const = 0;
	// Set unique name of gizmo
	void setUniqueName(const char* uniqueName);
	// Return unique name for Gizmo based on basename provided
	static const char* uniqueName(const char* base);
	// Return unique name of gizmo
	const char* uniqueName();
	// Set QMdiSubWindow containing the Gizmo
	void setWindow(QMdiSubWindow* window);
	// Return QMdiSubWindow containing the Gizmo
	QMdiSubWindow* window();
	// Find Gizmo with unique name provided
	static Gizmo* find(const char* uniqueName, const Gizmo* excludeThis = NULL);
	// Find Gizmo contained in specified subwindow
	static Gizmo* find(QMdiSubWindow* window);
	// Find all Gizmos of the specified type
	template <class G> static RefList<G> findAll(const char* gizmoType)
	{
		RefList<G> gizmos;
		for (Gizmo* gizmo : allGizmos_) if (DissolveSys::sameString(gizmo->type(), gizmoType)) gizmos.append(dynamic_cast<G*>(gizmo));
		return gizmos;
	}


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
	 * Data Handling
	 */
	public:
	// Return whether this Gizmo accepts data of the specified type
	virtual bool acceptsData(const char* dataType);
	// Return all Gizmos that accept data of the specified type
	static RefList<Gizmo> allThatAccept(const char* dataType);
	// Send data (referenced by its object tag) to the Gizmo
	virtual bool sendData(const char* dataType, const char* objectTag, const char* name = NULL);


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) const = 0;
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;
};

#endif
