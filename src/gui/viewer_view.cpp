// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
void BaseViewer::addDependentViewer(BaseViewer *viewer) { dependentViewers_.addUnique(viewer); }

// Unregister the specified viewer
void BaseViewer::removeDependentViewer(BaseViewer *viewer) { dependentViewers_.remove(viewer); }

// Link this viewer to the one specified
void BaseViewer::linkView(BaseViewer *viewToLinkTo)
{
    // Check if a link is already set...
    if (linkedViewer_)
    {
        // Perform consistency check on linked view...
        if ((&linkedViewer_->view()) != view_.linkedView())
            Messenger::warn("Mangled view links present...\n");

        linkedViewer_->removeDependentViewer(this);
    }

    linkedViewer_ = viewToLinkTo;
    linkedViewer_->addDependentViewer(this);
    view_.setLinkedView(&linkedViewer_->view());
}

// Unlink this viewer from the one specified
void BaseViewer::unlinkView()
{
    if (!linkedViewer_)
    {
        Messenger::warn("No linked viewer set, so can't unlink.\n");
        return;
    }

    linkedViewer_->removeDependentViewer(this);
    linkedViewer_ = nullptr;
    view_.setLinkedView(nullptr);
}
