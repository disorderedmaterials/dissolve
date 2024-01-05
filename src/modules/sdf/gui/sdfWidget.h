// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/sdf/gui/ui_sdfWidget.h"
#include "modules/widget.h"

// Forward Declarations
class SDFModule;
class DataViewer;
class Dissolve;
class RenderableData3D;
class RenderableSpecies;
class Species;

// Module Widget
class SDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    SDFModuleWidget(QWidget *parent, SDFModule *module, Dissolve &dissolve);
    ~SDFModuleWidget() override = default;

    private:
    // Associated Module
    SDFModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SDFModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *sdfGraph_;
    // Renderable for our SDF
    std::shared_ptr<RenderableData3D> sdfRenderable_;
    // Reference molecule (Species) to display alongside density
    Species *referenceMolecule_;
    // Renderable for reference molecule
    std::shared_ptr<RenderableSpecies> referenceMoleculeRenderable_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private slots:
    void on_LowerCutoffSpin_valueChanged(double value);
    void on_UpperCutoffSpin_valueChanged(double value);
    void on_ReferenceMoleculeCombo_currentIndexChanged(int index);
};
