// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "gui/render/renderablefactory.h"
#include "gui/viewer.hui"

// Clear existing data
void BaseViewer::clear()
{
    // Reset the group manager
    groupManager_.clear();

    // Clear all current data
    renderables_.clear();

    // Reset the view
    view_.clear();
}

// Own supplied Renderable and display it
void BaseViewer::ownRenderable(Renderable *newRenderable)
{
    // Warn if an existing Renderable has the same name as this one
    if (renderable(newRenderable->name()))
        fmt::print("A Renderable named '{}' already exists, and another with the same name is being added...\n",
                   newRenderable->name());

    // Own the new Renderable
    renderables_.own(newRenderable);

    emit(renderableAdded());
}

// Create Renderable by type and object identifier
Renderable *BaseViewer::createRenderable(Renderable::RenderableType type, std::string_view objectTag, std::string_view name,
                                         std::string_view groupName)
{
    Renderable *renderable = RenderableFactory::create(type, objectTag);
    if (renderable)
    {
        // Set Renderable name
        renderable->setName(name);

        // Own the new Renderable
        renderables_.own(renderable);

        // Set the group, if one was provided
        if (!groupName.empty())
            groupManager_.addToGroup(renderable, groupName);
    }

    emit(renderableAdded());

    return renderable;
}

// Remove existing Renderable
void BaseViewer::removeRenderable(Renderable *data)
{
    renderables_.remove(data);

    postRedisplay();

    emit(renderableRemoved());
}

// Clear all Renderables
void BaseViewer::clearRenderables()
{
    groupManager_.emptyGroups();
    renderables_.clear();

    postRedisplay();

    emit(renderableRemoved());
}

// Return number of Renderables
int BaseViewer::nRenderables() const { return renderables_.nItems(); }

// Return list of Renderables
const List<Renderable> &BaseViewer::renderables() const { return renderables_; }

// Return named Renderable
Renderable *BaseViewer::renderable(std::string_view name) const
{
    ListIterator<Renderable> renderableIterator(renderables_);
    while (Renderable *rend = renderableIterator.iterate())
        if (name == rend->name())
            return rend;

    return nullptr;
}

// Return Renderable with specified objectTag (if it exists)
Renderable *BaseViewer::renderableWithTag(std::string_view objectTag) const
{
    ListIterator<Renderable> renderableIterator(renderables_);
    while (Renderable *rend = renderableIterator.iterate())
        if (objectTag == rend->objectTag())
            return rend;

    return nullptr;
}

// Set visibility of named Renderable
void BaseViewer::setRenderableVisible(std::string_view name, bool visible)
{
    Renderable *rend = renderable(name);
    if (!rend)
    {
        fmt::print("No Renderable named '{}' exists, so can't set its visibility.\n", name);
        return;
    }

    rend->setVisible(visible);

    postRedisplay();
}

// Return visibility of named Renderable
bool BaseViewer::isRenderableVisible(std::string_view name) const
{
    Renderable *rend = renderable(name);
    if (!rend)
    {
        fmt::print("No Renderable named '{}' exists, so can't get its visibility.\n", name);
        return false;
    }

    return rend->isVisible();
}

/*
 * Renderable Groups
 */

// Return the group manager for Renderables
RenderableGroupManager &BaseViewer::groupManager() { return groupManager_; }

// Add Renderable to specified group
void BaseViewer::addRenderableToGroup(Renderable *rend, std::string_view group)
{
    groupManager_.addToGroup(rend, group);

    emit(renderableChanged());
}

/*
 * Signals
 */

// Emit the signal to notify that the viewing style of data represented in the Viewer has changed
void BaseViewer::notifyStyleModified() { emit(styleModified()); }

// Emit the signal to notify that the data has changed
void BaseViewer::notifyDataModified() { emit(dataModified()); }
