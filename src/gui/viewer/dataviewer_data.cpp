/*
	*** Data Viewer - Renderable Data
	*** src/gui/viewer/dataviewer_data.cpp
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

#include "gui/viewer/dataviewer.hui"
#include "gui/viewer/render/renderablefactory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Clear existing data
void DataViewer::clear()
{
	// Clear all current data
	renderables_.clear();

	// Reset the view
	view_.clear();
}

// Own supplied renderable and display it
void DataViewer::ownRenderable(Renderable* newRenderable)
{
	// Warn if an existing Renderable has the same name as this one
	if (renderable(newRenderable->name())) Messenger::warn("A Renderable named '%s' already exists, and another with the same name is being added...\n", newRenderable->name());

	renderables_.own(newRenderable);
}

// Create Renderable by type and object identifier
Renderable* DataViewer::createRenderable(Renderable::RenderableType type, const char* objectTag, const char* name, const char* title)
{
	Renderable* renderable = RenderableFactory::create(type, objectTag);
	if (renderable)
	{
		renderable->setName(name);
		renderable->setTitle(title ? title : name);
	}

	return renderable;
}

// Remove existing Renderable
void DataViewer::removeRenderable(Renderable* data)
{
	renderables_.remove(data);

	postRedisplay();
}

// Clear all Renderables
void DataViewer::clearRenderables()
{
	renderables_.clear();

	postRedisplay();
}

// Return number of Renderables
int DataViewer::nRenderables() const
{
	return renderables_.nItems();
}

// Return list of Renderables
const List<Renderable>& DataViewer::renderables() const
{
	return renderables_;
}

// Return named Renderable
Renderable* DataViewer::renderable(const char* name) const
{
	ListIterator<Renderable> renderableIterator(renderables_);
	while (Renderable* rend = renderableIterator.iterate()) if (DissolveSys::sameString(name, rend->name())) return rend;

	return NULL;
}

// Set visibility of named Renderable
void DataViewer::setRenderableVisible(const char* name, bool visible)
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
bool DataViewer::isRenderableVisible(const char* name) const
{
	Renderable* rend = renderable(name);
	if (!rend)
	{
		Messenger::error("No Renderable named '%s' exists, so can'' return its visibility.\n", name);
		return false;
	}
	
	return rend->isVisible();
}

// Return the group manager for the data
RenderableGroupManager& DataViewer::groupManager()
{
	return groupManager_;
}

// Return the view
View& DataViewer::view()
{
	return view_;
}
