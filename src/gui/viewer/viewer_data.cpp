/*
	*** Base Viewer - Renderable Data
	*** src/gui/viewer/viewer_data.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/viewer.hui"
#include "gui/viewer/render/renderablefactory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

/*
 * Renderable Data
 */

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
void BaseViewer::ownRenderable(Renderable* newRenderable)
{
	// Warn if an existing Renderable has the same name as this one
	if (renderable(newRenderable->name())) Messenger::warn("A Renderable named '%s' already exists, and another with the same name is being added...\n", newRenderable->name());

	// Own the new Renderable
	renderables_.own(newRenderable);

	emit(renderableAdded());
}

// Create Renderable by type and object identifier
Renderable* BaseViewer::createRenderable(Renderable::RenderableType type, const char* objectTag, const char* name, const char* groupName)
{
	Renderable* renderable = RenderableFactory::create(type, objectTag);
	if (renderable)
	{
		// Set Renderable name
		renderable->setName(name);

		// Own the new Renderable
		renderables_.own(renderable);

		// Set the group, if one was provided
		if (groupName) groupManager_.addToGroup(renderable, groupName);
	}

	emit(renderableAdded());

	return renderable;
}

// Remove existing Renderable
void BaseViewer::removeRenderable(Renderable* data)
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
int BaseViewer::nRenderables() const
{
	return renderables_.nItems();
}

// Return list of Renderables
const List<Renderable>& BaseViewer::renderables() const
{
	return renderables_;
}

// Return named Renderable
Renderable* BaseViewer::renderable(const char* name) const
{
	ListIterator<Renderable> renderableIterator(renderables_);
	while (Renderable* rend = renderableIterator.iterate()) if (DissolveSys::sameString(name, rend->name())) return rend;

	return NULL;
}

// Return Renderable with specified objectTag (if it exists)
Renderable* BaseViewer::renderableWithTag(const char* objectTag) const
{
	ListIterator<Renderable> renderableIterator(renderables_);
	while (Renderable* rend = renderableIterator.iterate()) if (DissolveSys::sameString(objectTag, rend->objectTag())) return rend;

	return NULL;
}

// Set visibility of named Renderable
void BaseViewer::setRenderableVisible(const char* name, bool visible)
{
	Renderable* rend = renderable(name);
	if (!rend)
	{
		Messenger::error("No Renderable named '%s' exists, so can't set its visibility.\n", name);
		return;
	}
	
	rend->setVisible(visible);

	postRedisplay();
}

// Return visibility of named Renderable
bool BaseViewer::isRenderableVisible(const char* name) const
{
	Renderable* rend = renderable(name);
	if (!rend)
	{
		Messenger::error("No Renderable named '%s' exists, so can't return its visibility.\n", name);
		return false;
	}
	
	return rend->isVisible();
}

/*
 * Renderable Groups
 */

// Return the group manager for Renderables
RenderableGroupManager& BaseViewer::groupManager()
{
	return groupManager_;
}

// Add Renderable to specified group
void BaseViewer::addRenderableToGroup(Renderable* rend, const char* group)
{
	groupManager_.addToGroup(rend, group);

	emit(renderableChanged());
}

/*
 * Options
 */

// Return the View definition
View& BaseViewer::view()
{
	if (view_.linkedView()) return (*view_.linkedView());
	return view_;
}

// Return the View definition (const)
const View& BaseViewer::constView() const
{
	if (view_.linkedView()) return (*view_.linkedView());
	return view_;
}

/*
 * Signals
 */

// Emit the signal to notify that the viewing style of data represented in the Viewer has changed
void BaseViewer::notifyStyleModified()
{
	emit(styleModified());
}

// Emit the signal to notify that the data has changed
void BaseViewer::notifyDataModified()
{
	emit(dataModified());
}
