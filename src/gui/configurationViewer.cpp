// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/configurationViewer.h"
#include "classes/configuration.h"
#include "data/elements.h"
#include "gui/render/renderableConfiguration.h"

ConfigurationViewer::ConfigurationViewer(QWidget *parent) : BaseViewer(parent)
{
    // Source data
    configuration_ = nullptr;

    // Interaction
    setInteractionMode(ConfigurationViewer::InteractionMode::Default);
    transientInteractionMode_ = TransientInteractionMode::None;

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
void ConfigurationViewer::setConfiguration(Configuration *cfg)
{
    configuration_ = cfg;
    configurationRenderable_ = nullptr;

    // Clear Renderables
    clearRenderables();

    // Create a new Renderable for the supplied Configuration
    if (configuration_)
    {
        configurationRenderable_ = std::make_shared<RenderableConfiguration>(configuration_);
        addRenderable(configurationRenderable_);
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
