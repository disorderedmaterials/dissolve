// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

// Create Renderable by type and object identifier
std::shared_ptr<Renderable> BaseViewer::createRenderable(Renderable::RenderableType type, std::string_view objectTag,
                                                         std::string_view name, std::string_view groupName)
{
    auto renderable = RenderableFactory::create(type, objectTag);
    if (renderable)
    {
        // Set Renderable name
        renderable->setName(name);

        // Own the new Renderable
        renderables_.emplace_back(renderable);

        // Set the group, if one was provided
        if (!groupName.empty())
            groupManager_.addToGroup(renderable, groupName);
    }

    emit(renderableAdded());

    return renderable;
}

// Add an existing Renderable
void BaseViewer::addRenderable(const std::shared_ptr<Renderable> &renderable)
{
    renderables_.emplace_back(renderable);

    emit(renderableAdded());
}

// Remove existing Renderable
void BaseViewer::removeRenderable(const std::shared_ptr<Renderable> &renderable)
{
    auto it = std::remove_if(renderables_.begin(), renderables_.end(), [renderable](auto r) { return r == renderable; });
    if (it != renderables_.end())
    {
        renderables_.erase(it);

        postRedisplay();

        emit(renderableRemoved());
    }
}

// Clear all Renderables
void BaseViewer::clearRenderables()
{
    groupManager_.emptyGroups();
    renderables_.clear();

    postRedisplay();

    emit(renderableRemoved());
}

// Return list of Renderables
const std::vector<std::shared_ptr<Renderable>> &BaseViewer::renderables() const { return renderables_; }

// Return Renderable with specified objectTag (if it exists)
std::shared_ptr<Renderable> BaseViewer::renderableWithTag(std::string_view objectTag) const
{
    auto it = std::find_if(renderables_.begin(), renderables_.end(),
                           [objectTag](auto &data) { return objectTag == data->objectTag(); });
    if (it != renderables_.end())
        return *it;

    return nullptr;
}

/*
 * Renderable Groups
 */

// Return the group manager for Renderables
RenderableGroupManager &BaseViewer::groupManager() { return groupManager_; }

// Add Renderable to specified group
void BaseViewer::addRenderableToGroup(std::shared_ptr<Renderable> &renderable, std::string_view group)
{
    groupManager_.addToGroup(renderable, group);

    emit(renderableChanged());
}

/*
 * Signals
 */

// Emit the signal to notify that the viewing style of data represented in the Viewer has changed
void BaseViewer::notifyStyleModified() { emit(styleModified()); }

// Emit the signal to notify that the data has changed
void BaseViewer::notifyDataModified() { emit(dataModified()); }
