// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/renderableSpecies.h"
#include "gui/render/renderableSpeciesSite.h"
#include "gui/viewer.h"

// Forward Declarations
class Element;
class Species;
class SpeciesAtom;

// Species Rendering Widget
class SpeciesViewer : public BaseViewer
{
    Q_OBJECT

    public:
    explicit SpeciesViewer(QWidget *parent);
    ~SpeciesViewer() override = default;

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

    protected Q_SLOTS:
    // Context menu requested
    void contextMenuRequested(QPoint pos) override;

    /*
     * Interaction (BaseViewer virtuals)
     */
    public:
    // Interaction Mode enum
    enum class InteractionMode
    {
        Select,
        SelectArea,
        Draw,
        Delete,
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
    // Clicked atom index at the start of the interaction (if any)
    std::optional<int> clickedAtomIndex_;
    // Element for created atoms in DrawInteraction
    Elements::Element drawElement_;
    // Species-space coordinates at clicked position for DrawInteraction
    Vec3<double> drawCoordinateStart_;
    // Species-space coordinates at current position for DrawInteraction
    Vec3<double> drawCoordinateCurrent_;

    private:
    // Return atom at specified coordinates
    std::optional<int> atomIndexAt(int x, int y);

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
    // Set current Element for DrawInteraction
    void setDrawElement(Elements::Element Z);
    // Return current Element for DrawInteraction
    Elements::Element drawElement() const;

    /*
     * Source Species and Site
     */
    private:
    // Source Species to display
    Species *species_;
    // Source SpeciesSite to display
    SpeciesSite *site_;
    // Sites are visible
    bool sitesVisible_;
    // Site management menu actions are visible in the context menu
    bool siteActionsVisible_{true};

    public:
    // Set target Species
    void setSpecies(Species *sp);
    // Return target Species
    Species *species() const;
    // Set target SpeciesSite
    void setSite(SpeciesSite *site);
    // Return target SpeciesSite
    SpeciesSite *speciesSite() const;
    // Toggle site visibility
    void setSiteVisible(bool visible);
    // Toggle visibility of site management menu actions in the context menu
    void setSiteActionsVisible(bool visible);

    /*
     * Renderable
     */
    private:
    // Renderable for source Species
    std::shared_ptr<RenderableSpecies> speciesRenderable_;
    // Renderable for displayed SpeciesSite
    std::shared_ptr<RenderableSpeciesSite> siteRenderable_;

    public:
    // Set renderable draw style
    void setRenderableDrawStyle(RenderableSpecies::SpeciesDisplayStyle ds);
    // Return current renderable draw style
    RenderableSpecies::SpeciesDisplayStyle renderableDrawStyle() const;
    // Recreate selection primitive
    void recreateSelectionPrimitive();

    /*
     * Signals
     */
    Q_SIGNALS:
    // Atom selection changed
    void atomsChanged();
    // Site created
    void siteCreatedAndShown();
    // Site changes
    void sitesChanged();
};
