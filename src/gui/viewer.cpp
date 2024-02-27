// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/viewer.h"
#include "base/messenger.h"

BaseViewer::BaseViewer(QWidget *parent)
    : QOpenGLWidget(parent), fontInstance_(parent->palette().text().color()), view_(renderables_, fontInstance_)
{
    // Character / Setup
    contextWidth_ = 0;
    contextHeight_ = 0;
    valid_ = false;
    drawing_ = false;
    renderingOffScreen_ = false;
    offscreenBuffer_ = nullptr;
    pixelScaling_ = 1.0;

    // View
    linkedViewer_ = nullptr;

    // Options
    axesVisible_ = true;
    clipToAxesVolume_ = true;

    // Interaction
    interacting_ = false;
    mouseReleaseTimer_.setSingleShot(true);
    mouseReleaseTimer_.setInterval(400);

    // Object Querying
    queryingObjects_ = false;

    // Prevent QPainter from autofilling widget background
    setAutoFillBackground(false);

    // Set a surface format to enable multisampling
    // NOTE This used to be performed by setting the default surface format within main(), but this created severe unwanted
    // widget behaviour
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(4);
    setFormat(surfaceFormat);
}

/*
 * Main UI
 */

// Set pointer to DissolveWindow (for optional integrations with main UI)
void BaseViewer::setDissolveWindow(DissolveWindow *dissolveWindow) { dissolveWindow_ = dissolveWindow; }
