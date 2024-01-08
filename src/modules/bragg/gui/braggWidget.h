// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/braggReflection.h"
#include "gui/models/braggReflectionFilterProxy.h"
#include "gui/models/braggReflectionModel.h"
#include "modules/bragg/gui/ui_braggWidget.h"
#include "modules/widget.h"
#include "templates/optionalRef.h"

// Forward Declarations
class AtomTypeMix;
class BraggModule;
class Configuration;
class Dissolve;
class DataViewer;

// Module Widget
class BraggModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    BraggModuleWidget(QWidget *parent, BraggModule *module, Dissolve &dissolve);
    ~BraggModuleWidget() = default;

    private:
    // Associated Module
    BraggModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::BraggModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *graph_;
    // Model for BraggReflection data
    BraggReflectionModel braggModel_;
    // Filter proxy for BreggReflection data
    BraggReflectionFilterProxy braggFilterProxy_;
    // Reflection data to display
    OptionalReferenceWrapper<const std::vector<BraggReflection>> reflectionData_;
    // Reflection atom types data
    OptionalReferenceWrapper<const AtomTypeMix> reflectionAtomTypesData_;
    // Last version of reflection data displayed
    int reflectionDataDisplayVersion_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    /*
     * Widgets / Functions
     */
    private Q_SLOTS:
    void on_PartialsButton_clicked(bool checked);
    void on_TotalsButton_clicked(bool checked);
    void on_ReflectionsButton_clicked(bool checked);
    void on_HideSmallIntensitiesCheck_clicked(bool checked);
};
