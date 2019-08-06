/*
	*** Configuration Viewer - Functions
	*** src/gui/viewer/configurationviewer_funcs.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/configurationviewer.hui"
#include "gui/viewer/render/renderableconfiguration.h"
#include "classes/configuration.h"
#include "data/elements.h"

// Constructor
ConfigurationViewer::ConfigurationViewer(QWidget* parent) : BaseViewer(parent)
{
	// Source data
	configuration_ = NULL;

	// Interaction
	setInteractionMode(ConfigurationViewer::DefaultInteraction);

	// Set up the view
	view_.setViewType(View::NormalView);
	view_.axes().setTitle(0, "X");
	view_.axes().setTitle(1, "Y");
	view_.axes().setTitle(2, "Z");

	// Tweak the options of the underlying BaseViewer
	setClipToAxesVolume(false);
}

// Destructor
ConfigurationViewer::~ConfigurationViewer()
{
}

/*
 * Target Configuration
 */

// Set target Configuration
void ConfigurationViewer::setConfiguration(Configuration *sp)
{
	configuration_ = sp;
	configurationRenderable_ = NULL;

	// Clear Renderables
	clearRenderables();

	// Create a new Renderable for the supplied Configuration
	if (configuration_)
	{
		configurationRenderable_ = new RenderableConfiguration(configuration_, configuration_->objectTag());
		ownRenderable(configurationRenderable_);
		view_.showAllData();
	}
}

// Return target Configuration
Configuration* ConfigurationViewer::configuration() const
{
	return configuration_;
}
