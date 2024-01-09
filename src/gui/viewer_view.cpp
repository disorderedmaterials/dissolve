// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/viewer.hui"

/*
 * View
 */

// Return the View definition
View &BaseViewer::view()
{
    if (view_.linkedView())
        return (*view_.linkedView());
    return view_;
}

const View &BaseViewer::view() const
{
    if (view_.linkedView())
        return (*view_.linkedView());
    return view_;
}

// Register that the specified viewer depends on (links to) this one
void BaseViewer::addDependentViewer(BaseViewer *viewer) { dependentViewers_.insert(viewer); }

// Unregister the specified viewer
void BaseViewer::removeDependentViewer(BaseViewer *viewer)
{
    auto it = std::find(dependentViewers_.begin(), dependentViewers_.end(), viewer);
    dependentViewers_.erase(it);
}
