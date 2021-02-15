// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "gui/viewer.hui"

BaseViewer::BaseViewer(QWidget *parent) : QOpenGLWidget(parent), view_(renderables_, fontInstance_)
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

BaseViewer::~BaseViewer() {}
