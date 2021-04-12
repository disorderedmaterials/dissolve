// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/graphgizmo.h"
#include "gui/selectgenericitemdialog.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <render/renderabledata1d.h>
#include <render/renderabledata2d.h>
#include <render/renderabledata3d.h>

GraphGizmo::GraphGizmo(Dissolve &dissolve, const QString uniqueName) : Gizmo(dissolve, uniqueName)
{
    // Set up user interface
    ui_.setupUi(this);

    // Grab the data viewer target and set our view
    dataViewer_ = ui_.DataView->dataViewer();
    dataViewer_->view().setAutoFollowType(View::AllAutoFollow);
    dataViewer_->view().setViewType(View::FlatXYView);
    dataViewer_->groupManager().setGroupColouring("Default", RenderableGroup::AutomaticIndividualColouring);

    // Permit the user to add data to the DataViewer
    dataViewer_->setFlags(DataViewer::UserCanRenameDataFlag + DataViewer::UserCanRemoveDataFlag);
    dataViewer_->setDissolve(&dissolve);

    // Update associated toolbar
    ui_.DataView->updateToolbar();

    refreshing_ = false;
}

GraphGizmo::~GraphGizmo() {}

/*
 * Core
 */

// Return string specifying Gizmo type
const QString GraphGizmo::type() const { return "Graph"; }

/*
 * UI
 */

// Window close event
void GraphGizmo::closeEvent(QCloseEvent *event) { emit(windowClosed(uniqueName_)); }

// Update controls within widget
void GraphGizmo::updateControls()
{
    refreshing_ = true;

    // Refresh the graph
    dataViewer_->postRedisplay();

    refreshing_ = false;
}

// Disable sensitive controls within widget
void GraphGizmo::disableSensitiveControls() {}

// Enable sensitive controls within widget
void GraphGizmo::enableSensitiveControls() {}

/*
 * Data Handling
 */

// Return whether this Gizmo accepts data of the specified type
bool GraphGizmo::acceptsData(std::string_view dataType)
{
    if (DissolveSys::sameString("Data1D", dataType))
        return true;
    if (DissolveSys::sameString("Data2D", dataType))
        return true;
    if (DissolveSys::sameString("Data3D", dataType))
        return true;

    return false;
}

// Send data (referenced by its object tag) to the Gizmo
bool GraphGizmo::sendData(std::string_view dataType, std::string_view tag, std::string_view name)
{
    switch (Renderable::renderableTypes().enumeration(dataType))
    {
        case (Renderable::Data1DRenderable):
            dataViewer_->createRenderable<RenderableData1D>(tag, name);
            break;
        case (Renderable::Data2DRenderable):
            dataViewer_->createRenderable<RenderableData2D>(tag, name);
            break;
        case (Renderable::Data3DRenderable):
            dataViewer_->createRenderable<RenderableData3D>(tag, name);
            break;
        default:
            return false;
    }

    return true;
}

/*
 * Data
 */

// Return pointer to the contained DataViewer
DataViewer *GraphGizmo::dataViewer() const { return dataViewer_; }

/*
 * Widget Signals / Slots
 */
