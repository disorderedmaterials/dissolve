/*
    *** Gizmo
    *** src/gui/gizmo.cpp
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

#include "gui/gizmo.h"
#include "base/sysfunc.h"
#include <QMdiSubWindow>

// Static Singletons
RefList<Gizmo> Gizmo::allGizmos_;

Gizmo::Gizmo(Dissolve &dissolve, const QString uniqueName) : ListItem<Gizmo>(), dissolve_(dissolve)
{
    window_ = nullptr;
    uniqueName_ = uniqueName;
    refreshing_ = false;
    allGizmos_.append(this);
}

Gizmo::~Gizmo() { allGizmos_.remove(this); }

/*
 * Core
 */

// Set unique name of widget
void Gizmo::setUniqueName(const QString uniqueName)
{
    uniqueName_ = uniqueName;
    if (window_)
        window_->setWindowTitle(uniqueName_);
}

// Return unique name for Gizmo based on basename provided
const QString Gizmo::uniqueName(const QString base)
{
    QString uniqueName = base;

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (find(uniqueName))
        uniqueName = QString("%1%2").arg(base).arg(++suffix);

    return uniqueName;
}

// Return unique name of widget
const QString Gizmo::uniqueName() const { return uniqueName_; }

// Set QMdiSubWindow containing the Gizmo
void Gizmo::setWindow(QMdiSubWindow *window) { window_ = window; }

// Return SubWindow in which this widget is displayed
QMdiSubWindow *Gizmo::window() { return window_; }

// Find Gizmo with unique name provided
Gizmo *Gizmo::find(const QString uniqueName, const Gizmo *excludeThis)
{
    for (Gizmo *gizmo : allGizmos_)
    {
        if (gizmo == excludeThis)
            continue;

        if (gizmo->uniqueName() == uniqueName)
            return gizmo;
    }

    return nullptr;
}

// Find Gizmo contained in specified subwindow
Gizmo *Gizmo::find(QMdiSubWindow *window)
{
    for (Gizmo *gizmo : allGizmos_)
        if (window == gizmo->window())
            return gizmo;

    return nullptr;
}

/*
 * Data Handling
 */

// Return whether this Gizmo accepts data of the specified type
bool Gizmo::acceptsData(const QString &dataType) { return false; }

// Return all Gizmos that accept data of the specified type
RefList<Gizmo> Gizmo::allThatAccept(const QString &dataType)
{
    RefList<Gizmo> gizmos;

    for (Gizmo *gizmo : allGizmos_)
        if (gizmo->acceptsData(dataType))
            gizmos.append(gizmo);

    return gizmos;
}

// Send data (referenced by its object tag) to the Gizmo
bool Gizmo::sendData(const QString &dataType, std::string_view objectTag, std::string_view name) { return false; }
