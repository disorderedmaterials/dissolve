/*
    *** WorkspaceTab Functions
    *** src/gui/workspacetab_funcs.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "gui/gettabnamedialog.h"
#include "gui/gizmos.h"
#include "gui/gui.h"
#include "gui/tmdiarea.hui"
#include "gui/workspacetab.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QMdiSubWindow>
#include <QMenu>

WorkspaceTab::WorkspaceTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title)
    : ListItem<WorkspaceTab>(), MainTab(dissolveWindow, dissolve, parent, title, this)
{
    ui.setupUi(this);

    Locker refreshLocker(refreshLock_);

    // Add a TMdiArea to the main layout
    mdiArea_ = new TMdiArea(dissolveWindow);
    ui.verticalLayout->addWidget(mdiArea_);
    connect(mdiArea_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(mdiArea_, SIGNAL(subWindowActivated(QMdiSubWindow *)), dissolveWindow,
            SLOT(currentWorkspaceGizmoChanged(QMdiSubWindow *)));
}

WorkspaceTab::~WorkspaceTab() {}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType WorkspaceTab::type() const { return MainTab::WorkspaceTabType; }

// Raise suitable dialog for entering / checking new tab name
QString WorkspaceTab::getNewTitle(bool &ok)
{
    // Get a new, valid name for the workspace
    GetTabNameDialog nameDialog(this, dissolveWindow_->allTabs());
    ok = nameDialog.get(this, title());

    if (ok)
    {
        // Rename our Workspace, and flag that our data has been modified
        title_ = qPrintable(nameDialog.newName());
    }

    return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool WorkspaceTab::canChangeTitle() const { return true; }

/*
 * Update
 */

// Update controls in tab
void WorkspaceTab::updateControls()
{
    // Update our MDI subwindows
    ListIterator<Gizmo> gizmoIterator(gizmos_);
    while (Gizmo *gizmo = gizmoIterator.iterate())
        gizmo->updateControls();
}

// Disable sensitive controls within tab
void WorkspaceTab::disableSensitiveControls()
{
    // Disable sensitive controls in subwindows
    ListIterator<Gizmo> gizmoIterator(gizmos_);
    while (Gizmo *gizmo = gizmoIterator.iterate())
        gizmo->disableSensitiveControls();
}

// Enable sensitive controls within tab
void WorkspaceTab::enableSensitiveControls()
{
    // Enable sensitive controls in subwindows
    ListIterator<Gizmo> gizmoIterator(gizmos_);
    while (Gizmo *gizmo = gizmoIterator.iterate())
        gizmo->enableSensitiveControls();
}

/*
 * Gizmo Management
 */

// Remove Gizmo with specified unique name
void WorkspaceTab::removeGizmo(QString uniqueName)
{
    // Find the Gizmo...
    Gizmo *gizmo = Gizmo::find(qPrintable(uniqueName));
    if (!gizmo)
    {
        Messenger::error("Received signal to remove gizmo '{}' but it cannot be found...\n", qPrintable(uniqueName));
        return;
    }

    gizmos_.remove(gizmo);
}

// Create Gizmo with specified type
Gizmo *WorkspaceTab::createGizmo(QString type)
{
    Gizmo *gizmo = nullptr;
    QWidget *widget = nullptr;

    // Check the type of the provided gizmo...
    if (type == "Graph")
    {
        GraphGizmo *graph = new GraphGizmo(dissolveWindow_->dissolve(), Gizmo::uniqueName("Graph"));
        connect(graph, SIGNAL(windowClosed(QString)), this, SLOT(removeGizmo(QString)));
        gizmo = graph;
        widget = graph;
    }
    else if (type == "Integrator1D")
    {
        Integrator1DGizmo *integrator1D = new Integrator1DGizmo(dissolveWindow_->dissolve(), Gizmo::uniqueName("Integrator1D"));
        connect(integrator1D, SIGNAL(windowClosed(QString)), this, SLOT(removeGizmo(QString)));
        gizmo = integrator1D;
        widget = integrator1D;
    }
    else
    {
        Messenger::error("Couldn't add gizmo to workspace '{} - unrecognised type '{}' encountered.\n", qPrintable(title()),
                         qPrintable(type));
        return nullptr;
    }

    // Create a new window for the Gizmo's widget and show it
    QMdiSubWindow *window = mdiArea_->addSubWindow(widget);
    window->setWindowTitle(gizmo->uniqueName());
    window->setFont(font());
    window->show();
    gizmo->setWindow(window);

    // Update the Gizmo's controls, and add it to our lists
    gizmo->updateControls();
    gizmos_.own(gizmo);

    mdiArea_->setActiveSubWindow(window);
    dissolveWindow_->currentWorkspaceGizmoChanged(window);

    return gizmo;
}

/*
 * Context Menu
 */

// Custom context menu requested
void WorkspaceTab::showContextMenu(const QPoint &pos)
{
    // Check that we are not over an existing gizmo
    ListIterator<Gizmo> gizmoIterator(gizmos_);
    while (Gizmo *gizmo = gizmoIterator.iterate())
        if (gizmo->window()->geometry().contains(pos))
            return;

    QMenu menu;
    menu.setFont(font());
    QFont italicFont = font();
    italicFont.setItalic(true);

    // Gizmos
    QMenu *gizmoMenu = menu.addMenu("Add &Gizmo...");
    gizmoMenu->setFont(font());
    connect(gizmoMenu->addAction("Graph"), SIGNAL(triggered(bool)), this, SLOT(contextMenuAddGizmo(bool)));
    connect(gizmoMenu->addAction("Integrator1D"), SIGNAL(triggered(bool)), this, SLOT(contextMenuAddGizmo(bool)));

    menu.exec(mapToGlobal(pos));
}

// Create Gizmo from context menu item
void WorkspaceTab::contextMenuAddGizmo(bool checked)
{
    // Get the sender QAction
    QAction *action = dynamic_cast<QAction *>(sender());
    if (!action)
        return;

    // The text of the sender QAction is the type of the Gizmo we need to create
    createGizmo(qPrintable(action->text()));
}

/*
 * State
 */

// Read widget state through specified LineParser
bool WorkspaceTab::readState(LineParser &parser, const CoreData &coreData)
{
    // Read tab state information:   nGizmos
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    const auto nGizmos = parser.argi(0);

    // Read in widgets
    for (int n = 0; n < nGizmos; ++n)
    {
        // Read line from the file, which should contain the gizmo type
        if (parser.getArgsDelim() != LineParser::Success)
            return false;
        Gizmo *gizmo = createGizmo(QString::fromStdString(std::string(parser.argsv(0))));
        if (gizmo == nullptr)
            return Messenger::error("Unrecognised gizmo type '{}' in workspace '{}'.\n", parser.argsv(0), qPrintable(title()));

        // Set the unique name
        gizmo->setUniqueName(Gizmo::uniqueName(QString::fromStdString(std::string(parser.argsv(1)))));

        // Read in the widget's geometry / state / flags
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        gizmo->window()->setGeometry(parser.argi(0), parser.argi(1), parser.argi(2), parser.argi(3));
        // -- Is the window maximised, or shaded?
        if (parser.argb(4))
            gizmo->window()->showMaximized();
        else if (parser.argb(5))
            gizmo->window()->showShaded();

        // Now call the widget's local readState()
        if (!gizmo->readState(parser))
            return false;
    }

    // Ensure workspace 'rename' menu item is set correctly
    dissolveWindow_->currentWorkspaceGizmoChanged(mdiArea_->currentSubWindow());

    return true;
}

// Write widget state through specified LineParser
bool WorkspaceTab::writeState(LineParser &parser) const
{
    // Write tab state information:   nGizmos
    if (!parser.writeLineF("{}      # NGizmos\n", gizmos_.nItems()))
        return false;

    // Loop over our subwindow list
    ListIterator<Gizmo> gizmoIterator(gizmos_);
    while (Gizmo *gizmo = gizmoIterator.iterate())
    {
        // Write Gizmo type
        if (!parser.writeLineF("{}  '{}'\n", qPrintable(gizmo->type()), qPrintable(gizmo->uniqueName())))
            return false;

        // Write window geometry / state
        QRect geometry = gizmo->window()->geometry();
        if (!parser.writeLineF("{} {} {} {} {} {}\n", geometry.x(), geometry.y(), geometry.width(), geometry.height(),
                               DissolveSys::btoa(gizmo->window()->isMaximized()),
                               DissolveSys::btoa(gizmo->window()->isShaded())))
            return false;

        // Write gizmo-specific state information
        if (!gizmo->writeState(parser))
            return false;
    }

    return true;
}
