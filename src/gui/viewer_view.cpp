/*
    *** Base Viewer - View
    *** src/gui/viewer_view.cpp
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

// Return the View definition (const)
const View &BaseViewer::constView() const
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
