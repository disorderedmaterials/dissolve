// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/calculate_sdf/gui/ui_modulewidget.h"

// Forward Declarations
class CalculateSDFModule;
class CoreData;
class DataViewer;
class RenderableData3D;
class RenderableSpecies;
class Species;

// Module Widget
class CalculateSDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    CalculateSDFModuleWidget(QWidget *parent, CalculateSDFModule *module, const CoreData &coreData);

    private:
    // Associated Module
    CalculateSDFModule *module_;
    // CoreData reference
    const CoreData &coreData_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::CalculateSDFModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *sdfGraph_;
    // Renderable for our SDF
    std::shared_ptr<Renderable> sdfRenderable_;
    // Reference molecule (Species) to display alongside density
    Species *referenceMolecule_;
    // Renderable for reference molecule
    std::shared_ptr<Renderable> referenceMoleculeRenderable_;

    public:
    // Update controls within widget
    void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);

    /*
     * Widgets / Functions
     */
    private:
    // Set data targets in graphs
    void setGraphDataTargets();

    private slots:
    void on_LowerCutoffSpin_valueChanged(double value);
    void on_UpperCutoffSpin_valueChanged(double value);
    void on_ReferenceMoleculeCombo_currentIndexChanged(int index);
};
