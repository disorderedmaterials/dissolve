// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/renderableConfiguration.h"
#include "gui/viewer.h"

// Forward Declarations
class Configuration;
class Atom;

// Configuration Rendering Widget
class ConfigurationViewer : public BaseViewer
{
    Q_OBJECT

    public:
    ConfigurationViewer(QWidget *parent);
    ~ConfigurationViewer();

    /*
     * Rendering (BaseViewer virtuals)
     */
    private:
    // Perform post-initialisation operations
    void postInitialiseGL() override;
    // Perform post-resize operations
    void postResizeGL() override;
    // Render 2D overlay content
    void render2DOverlay() override;

    /*
     * Mouse / Keyboard Input (BaseViewer virtuals)
     */
    protected:
    // Mouse moved
    void mouseMoved(int dx, int dy) override;

    /*
     * Interaction (BaseViewer virtuals)
     */
    public:
    // Interaction Mode enum
    enum class InteractionMode
    {
        Default,
        nInteractionModes
    };
    // Secondary Interaction Mode enum
    enum class TransientInteractionMode
    {
        None,
        TranslateView,
        RotateView
    };

    private:
    // Primary interaction mode (left mouse button)
    InteractionMode interactionMode_;
    // Transient interaction mode (i.e. one not invoked by primary button)
    TransientInteractionMode transientInteractionMode_;

    private:
    // Return atom at specified coordinates
    const Atom *atomAt(int x, int y);

    protected:
    // Start interaction at the specified screen coordinates
    void startInteraction() override;
    // End interaction at the specified screen coordinates
    void endInteraction() override;
    // Cancel current interaction
    void cancelInteraction() override;

    public:
    // Set current interaction mode
    void setInteractionMode(InteractionMode mode);
    // Return current interaction mode
    InteractionMode interactionMode() const;
    // Return text describing current interaction mode
    const QString interactionModeText() const override;

    /*
     * Source Configuration
     */
    private:
    // Source Configuration to display
    Configuration *configuration_;

    public:
    // Set target Configuration
    void setConfiguration(Configuration *cfg);
    // Return target Configuration
    Configuration *configuration() const;

    /*
     * Renderable
     */
    private:
    // Renderable for source Configuration
    std::shared_ptr<RenderableConfiguration> configurationRenderable_;

    public:
    // Set renderable draw style
    void setRenderableDrawStyle(RenderableConfiguration::ConfigurationDisplayStyle ds);
    // Return current renderable draw style
    RenderableConfiguration::ConfigurationDisplayStyle renderableDrawStyle() const;
};
