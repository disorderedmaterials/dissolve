/*
    *** Data Viewer - Context Menu
    *** src/gui/dataviewer_contextmenu.cpp
    Copyright T. Youngs 2013-2020

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

#include "base/messenger.h"
#include "gui/dataviewer.hui"
#include "gui/gizmo.h"
#include "gui/selectgenericitemdialog.h"
#include "io/export/data1d.h"
#include "io/export/data2d.h"
#include "io/export/data3d.h"
#include "math/data2d.h"
#include <QFileDialog>

/*
 * Private Functions
 */

// Show general context menu
void DataViewer::showGeneralContextMenu(QPoint pos)
{
    QMenu menu;
    QAction *action;
    menu.setFont(font());

    // Reset View
    QAction *resetViewAction = menu.addAction("&Reset View");

    // Copy to clipboard
    QAction *copyToClipboardAction = menu.addAction("&Copy to clipboard");

    // If the user can add data, add a new section allowing it
    RefDataList<QAction, int> addDataAction;
    if (hasFlag(DataViewer::UserCanAddDataFlag))
    {
        menu.addSeparator();
        addDataAction.append(menu.addAction("Add Data&1D..."), 1);
        addDataAction.append(menu.addAction("Add Data&2D..."), 2);
        addDataAction.append(menu.addAction("Add Data&3D..."), 3);
    }

    // Execute the menu
    QAction *selectedAction = menu.exec(mapToGlobal(pos));

    // Act on the action!
    if (selectedAction == resetViewAction)
        view_.resetViewMatrix();
    else if (selectedAction == copyToClipboardAction)
        copyViewToClipboard(true);
    else if (addDataAction.contains(selectedAction))
    {
        auto dimensionality = addDataAction.dataForItem(selectedAction);
        SelectGenericItemDialog genericItemDialog(this, *dissolve_);
        if (dimensionality == 1)
        {
            Data1D *item = genericItemDialog.selectGenericItem<Data1D>();
            if (item)
                createRenderable(Renderable::Data1DRenderable, item->objectTag(), item->name(), "Default");
        }
        else if (dimensionality == 2)
        {
            Data2D *item = genericItemDialog.selectGenericItem<Data2D>();
            if (item)
                createRenderable(Renderable::Data2DRenderable, item->objectTag(), item->name(), "Default");
        }
        else if (dimensionality == 3)
        {
            Data3D *item = genericItemDialog.selectGenericItem<Data3D>();
            if (item)
                createRenderable(Renderable::Data3DRenderable, item->objectTag(), item->name(), "Default");
        }
    }
}

// Show renderable context menu
void DataViewer::showRenderableContextMenu(QPoint pos, Renderable *rend)
{
    QMenu menu;
    QAction *action;
    RefDataList<QAction, Gizmo *> copyToActions;
    menu.setFont(font());
    QFont italicFont(menu.font());
    italicFont.setItalic(true);

    // Add header
    action = menu.addAction(QString::fromStdString(std::string(queryObjectInfo())));
    action->setEnabled(false);
    action->setFont(italicFont);
    menu.addSeparator();

    // -- Hide Renderable
    QAction *hideAction = menu.addAction("&Hide");

    // -- Save As...
    QAction *saveAsAction = menu.addAction("&Save as...");
    saveAsAction->setEnabled(rend->type() >= Renderable::Data1DRenderable && rend->type() <= Renderable::Data3DRenderable);

    // -- Copy To...
    QMenu *copyToMenu = menu.addMenu("&Copy to...");
    copyToMenu->setFont(menu.font());
    // Get list of viable destinations that will accept our data
    RefList<Gizmo> destinations =
        Gizmo::allThatAccept(QString::fromStdString(std::string(Renderable::renderableTypes().keyword(rend->type()))));
    if (destinations.nItems() == 0)
        copyToMenu->setEnabled(false);
    else
    {
        for (Gizmo *destination : destinations)
        {
            action = copyToMenu->addAction(destination->uniqueName());
            copyToActions.append(action, destination);
        }
    }

    // -- Remove Renderable
    QAction *removeAction = nullptr;
    if (hasFlag(DataViewer::UserCanRemoveDataFlag))
    {
        menu.addSeparator();
        removeAction = menu.addAction("&Remove");
    }

    // Execute the menu
    QAction *selectedAction = menu.exec(mapToGlobal(pos));

    // Act on the action!
    if (selectedAction)
    {
        if (selectedAction == hideAction)
        {
            rend->setVisible(false);
            emit(renderableChanged());
        }
        else if (selectedAction == saveAsAction)
        {
            // Get save file name
            QString filename =
                QFileDialog::getSaveFileName(this, "Select Exported Data File", QDir::currentPath(), "All Files (*.*)");
            if (!filename.isEmpty())
            {
                if (rend->type() == Renderable::Data1DRenderable)
                {
                    Data1DExportFileFormat exportFormat(qPrintable(filename));
                    Data1D *data = Data1D::findObject(rend->objectTag());
                    if (!data)
                        fmt::print("Failed to locate data to export (tag = {}).\n", rend->objectTag());
                    else
                        exportFormat.exportData(*data);
                }
                else if (rend->type() == Renderable::Data2DRenderable)
                {
                    Data2DExportFileFormat exportFormat(qPrintable(filename));
                    Data2D *data = Data2D::findObject(rend->objectTag());
                    if (!data)
                        fmt::print("Failed to locate data to export (tag = {}).\n", rend->objectTag());
                    else
                        exportFormat.exportData(*data);
                }
                else if (rend->type() == Renderable::Data3DRenderable)
                {
                    Data3DExportFileFormat exportFormat(qPrintable(filename));
                    Data3D *data = Data3D::findObject(rend->objectTag());
                    if (!data)
                        fmt::print("Failed to locate data to export (tag = {}).", rend->objectTag());
                    else
                        exportFormat.exportData(*data);
                }
            }
        }
        else if (copyToActions.contains(selectedAction))
        {
            Gizmo *destination = copyToActions.dataForItem(selectedAction);
            if (!destination)
                return;
            destination->sendData(QString::fromStdString(std::string(Renderable::renderableTypes().keyword(rend->type()))),
                                  rend->objectTag(), rend->name());
        }
        else if (selectedAction == removeAction)
            removeRenderable(rend);
    }

    // Done
    postRedisplay();
}
