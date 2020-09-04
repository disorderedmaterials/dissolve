/*
    *** Configuration Viewer - Functions
    *** src/gui/configurationviewer_funcs.cpp
    Copyright T. Youngs 2019-2020

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

#include "classes/configuration.h"
#include "data/elements.h"
#include "gui/configurationviewer.hui"
#include "gui/render/renderableconfiguration.h"

ConfigurationViewer::ConfigurationViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    configuration_ = nullptr;

    // Interaction
    setInteractionMode(ConfigurationViewer::DefaultInteraction);

    // Set up the view
    view_.setViewType(View::NormalView);
    view_.axes().setTitle(0, "X");
    view_.axes().setTitle(1, "Y");
    view_.axes().setTitle(2, "Z");
    view_.setAutoFollowType(View::AllAutoFollow);

    // Tweak the options of the underlying BaseViewer
    setClipToAxesVolume(false);
}

ConfigurationViewer::~ConfigurationViewer() {}

/*
 * Target Configuration
 */

// Set target Configuration
void ConfigurationViewer::setConfiguration(Configuration *sp)
{
    configuration_ = sp;
    configurationRenderable_ = nullptr;

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
Configuration *ConfigurationViewer::configuration() const { return configuration_; }

/*
 * Renderable
 */

// Set renderable draw style
void ConfigurationViewer::setRenderableDrawStyle(RenderableConfiguration::ConfigurationDisplayStyle ds)
{
    if (configurationRenderable_)
        configurationRenderable_->setDisplayStyle(ds);
}

// Return current renderable draw style
RenderableConfiguration::ConfigurationDisplayStyle ConfigurationViewer::renderableDrawStyle() const
{
    if (configurationRenderable_)
        return configurationRenderable_->displayStyle();

    return RenderableConfiguration::LinesStyle;
}
