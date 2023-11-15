// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
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

// Add an existing Renderable
void BaseViewer::addRenderable(const std::shared_ptr<Renderable> &renderable)
{
    renderables_.emplace_back(renderable);

    Q_EMIT(renderableAdded());
}

// Remove existing Renderable
void BaseViewer::removeRenderable(const std::shared_ptr<Renderable> &renderable)
{
    auto it = std::remove_if(renderables_.begin(), renderables_.end(), [renderable](auto r) { return r == renderable; });
    if (it != renderables_.end())
    {
        renderables_.erase(it);

        postRedisplay();

        Q_EMIT(renderableRemoved());
    }
}

// Clear all Renderables
void BaseViewer::clearRenderables()
{
    groupManager_.emptyGroups();
    renderables_.clear();

    postRedisplay();

    Q_EMIT(renderableRemoved());
}

// Attempt to validate all renderables
void BaseViewer::validateRenderables(const GenericList &source)
{
    for (auto &renderable : renderables_)
        renderable->validateDataSource(source);
}

// Return list of Renderables
const std::vector<std::shared_ptr<Renderable>> &BaseViewer::renderables() const { return renderables_; }

// Return Renderable with specified tag (if it exists)
std::shared_ptr<Renderable> BaseViewer::renderableWithTag(std::string_view tag) const
{
    auto it = std::find_if(renderables_.begin(), renderables_.end(), [tag](auto &data) { return tag == data->tag(); });
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

    Q_EMIT(renderableChanged());
}

/*
 * Signals
 */

// Emit the signal to notify that the viewing style of data represented in the Viewer has changed
void BaseViewer::notifyStyleModified() { Q_EMIT(styleModified()); }

// Emit the signal to notify that the data has changed
void BaseViewer::notifyDataModified() { Q_EMIT(dataModified()); }
