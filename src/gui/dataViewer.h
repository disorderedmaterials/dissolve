// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/viewer.h"

// Forward Declarations
class Dissolve;

// Data Viewer
class DataViewer : public BaseViewer
{
    Q_OBJECT

    public:
    DataViewer(QWidget *parent);
    ~DataViewer() override;

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
    // Mouse 'wheeled'
    void mouseWheeled(int delta) override;
    // Mouse double clicked
    void mouseDoubleClicked() override;
    // Context menu requested
    void contextMenuRequested(QPoint pos) override;
    // Key pressed
    bool keyPressed(int key) override;
    // Key released
    bool keyReleased(int key) override;

    Q_SIGNALS:
    void currentCoordinateChanged();
    void controlAspectChanged();

    /*
     * Interaction (BaseViewer Virtuals)
     */
    public:
    // Interaction Mode enum
    enum class InteractionMode
    {
        Default,
        ZoomToArea,
        ZoomXRange,
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

    protected:
    // Start interaction
    void startInteraction() override;
    // End interaction
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
    // Return clicked interaction coordinate on axis
    double clickedInteractionCoordinate();
    // Return current interaction coordinate on axis
    double currentInteractionCoordinate();

    /*
     * Menus
     */
    private:
    // Show general context menu
    void showGeneralContextMenu(QPoint pos);
    // Show renderable context menu
    void showRenderableContextMenu(QPoint pos, std::shared_ptr<Renderable> rend);

    /*
     * Control Flags
     */
    private:
    // Pointer to Dissolve
    Dissolve *dissolve_;
    // Control flags
    int controlFlags_;

    public:
    // Control Flags Enum
    enum ControlFlag
    {
        NoFlags = 0,
        UserCanRenameDataFlag = 1,
        UserCanRemoveDataFlag = 2
    };
    // Set pointer to Dissolve
    void setDissolve(Dissolve *dissolve);
    // Set control flags
    void setFlags(int flags);
    // Return whether the specified flag is set
    bool hasFlag(ControlFlag flag) const;
};
