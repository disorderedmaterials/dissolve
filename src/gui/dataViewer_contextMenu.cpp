// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "gui/dataViewer.h"
#include "gui/render/renderableData1D.h"
#include "gui/render/renderableData2D.h"
#include "gui/render/renderableData3D.h"
#include "gui/selectGenericItemDialog.h"
#include "io/export/data1D.h"
#include "io/export/data2D.h"
#include "io/export/data3D.h"
#include "math/data2D.h"
#include <QFileDialog>

/*
 * Private Functions
 */

// Show general context menu
void DataViewer::showGeneralContextMenu(QPoint pos)
{
    QMenu menu;

    // Reset View
    QAction *resetViewAction = menu.addAction("&Reset View");

    // Copy to clipboard
    QAction *copyToClipboardAction = menu.addAction("&Copy to clipboard");

    // Execute the menu
    QAction *selectedAction = menu.exec(mapToGlobal(pos));

    // Act on the action!
    if (selectedAction == resetViewAction)
        view_.resetViewMatrix();
    else if (selectedAction == copyToClipboardAction)
        copyViewToClipboard(true);
}

// Show renderable context menu
void DataViewer::showRenderableContextMenu(QPoint pos, std::shared_ptr<Renderable> renderable)
{
    QMenu menu;
    QFont italicFont(menu.font());
    italicFont.setItalic(true);

    // Add header
    auto *headerAction = menu.addAction(QString::fromStdString(std::string(queryObjectInfo())));
    headerAction->setEnabled(false);
    headerAction->setFont(italicFont);
    menu.addSeparator();

    // -- Hide Renderable
    auto *hideAction = menu.addAction("&Hide");

    // -- Save As...
    auto *saveAsAction = menu.addAction("&Save as...");
    saveAsAction->setEnabled(renderable->type() >= Renderable::Data1DRenderable &&
                             renderable->type() <= Renderable::Data3DRenderable);

    // -- Remove Renderable
    QAction *removeAction{nullptr};
    if (hasFlag(DataViewer::UserCanRemoveDataFlag))
    {
        menu.addSeparator();
        removeAction = menu.addAction("&Remove");
    }

    // Execute the menu
    auto *selectedAction = menu.exec(mapToGlobal(pos));

    // Act on the action!
    if (selectedAction)
    {
        if (selectedAction == hideAction)
        {
            renderable->setVisible(false);
            Q_EMIT(renderableChanged());
        }
        else if (selectedAction == saveAsAction)
        {
            // Get save file name
            auto filename =
                QFileDialog::getSaveFileName(this, "Select Exported Data File", QDir::currentPath(), "All Files (*)");
            if (!filename.isEmpty())
            {
                if (renderable->type() == Renderable::Data1DRenderable)
                {
                    Data1DExportFileFormat exportFormat(qPrintable(filename));
                    auto *r1d = static_cast<RenderableData1D *>(renderable.get());
                    if (!r1d->source())
                        Messenger::error("Failed to locate 1D data to export.\n");
                    else
                        exportFormat.exportData(r1d->source()->get());
                }
                else if (renderable->type() == Renderable::Data2DRenderable)
                {
                    Data2DExportFileFormat exportFormat(qPrintable(filename));
                    auto *r2d = static_cast<RenderableData2D *>(renderable.get());
                    if (!r2d->source())
                        Messenger::error("Failed to locate 2D data to export.\n");
                    else
                        exportFormat.exportData(r2d->source()->get());
                }
                else if (renderable->type() == Renderable::Data3DRenderable)
                {
                    Data3DExportFileFormat exportFormat(qPrintable(filename));
                    auto *r3d = static_cast<RenderableData3D *>(renderable.get());
                    if (!r3d->source())
                        Messenger::error("Failed to locate 3D data to export\n");
                    else
                        exportFormat.exportData(r3d->source()->get());
                }
            }
        }
        else if (selectedAction == removeAction)
            removeRenderable(renderable);
    }

    // Done
    postRedisplay();
}
