// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/messenger.h"
#include "gui/viewer.hui"

// Set whether Axes are visible
void BaseViewer::setAxesVisible(bool visible) { axesVisible_ = visible; }

// Return whether Axes are visible
bool BaseViewer::axesVisible() const { return axesVisible_; }

// Set whether to apply clip planes about Axes volume for Renderables
void BaseViewer::setClipToAxesVolume(bool clip) { clipToAxesVolume_ = clip; }

// Return whether clip planes are applied about Axes volume for Renderables
bool BaseViewer::clipToAxesVolume() const { return clipToAxesVolume_; }
