// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/dataViewer.h"
#include "base/messenger.h"

DataViewer::DataViewer(QWidget *parent) : BaseViewer(parent)
{
    // Interaction
    setInteractionMode(DataViewer::InteractionMode::Default);
    transientInteractionMode_ = TransientInteractionMode::None;

    // Set up the view
    view_.setViewType(View::FlatXYView);

    // Set options
    dissolve_ = nullptr;
    controlFlags_ = NoFlags;
}

DataViewer::~DataViewer() {}

/*
 * Options
 */

// Set pointer to Dissolve
void DataViewer::setDissolve(Dissolve *dissolve) { dissolve_ = dissolve; }

// Set control flags
void DataViewer::setFlags(int flags) { controlFlags_ = flags; }

// Return whether the specified flag is set
bool DataViewer::hasFlag(ControlFlag flag) const { return (controlFlags_ & flag); }
