// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

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
    Gizmo(Dissolve &dissolve, const QString uniqueName);
    virtual ~Gizmo();

    /*
     * Core
     */
    protected:
    // Unique name of gizmo
    QString uniqueName_;
    // Reference to Dissolve
    Dissolve &dissolve_;
    // QMdiSubWindow containing the Gizmo
    QMdiSubWindow *window_;
    // Reference list of all Gizmos
    static RefList<Gizmo> allGizmos_;

    public:
    // Return string specifying Gizmo type
    virtual const QString type() const = 0;
    // Set unique name of gizmo
    void setUniqueName(const QString uniqueName);
    // Return unique name for Gizmo based on basename provided
    static const QString uniqueName(const QString base);
    // Return unique name of gizmo
    const QString uniqueName() const;
    // Set QMdiSubWindow containing the Gizmo
    void setWindow(QMdiSubWindow *window);
    // Return QMdiSubWindow containing the Gizmo
    QMdiSubWindow *window();
    // Find Gizmo with unique name provided
    static Gizmo *find(const QString uniqueName, const Gizmo *excludeThis = nullptr);
    // Find Gizmo contained in specified subwindow
    static Gizmo *find(QMdiSubWindow *window);
    // Find all Gizmos of the specified type
    template <class G> static RefList<G> findAll(const QString gizmoType)
    {
        RefList<G> gizmos;
        for (Gizmo *gizmo : allGizmos_)
            if (gizmo->type() == gizmoType)
                gizmos.append(dynamic_cast<G *>(gizmo));
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
    virtual void closeEvent(QCloseEvent *event) = 0;

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
    virtual bool acceptsData(const QString &dataType);
    // Return all Gizmos that accept data of the specified type
    static RefList<Gizmo> allThatAccept(const QString &ataType);
    // Send data (referenced by its object tag) to the Gizmo
    virtual bool sendData(const QString &dataType, std::string_view tag, std::string_view name = "");
};
