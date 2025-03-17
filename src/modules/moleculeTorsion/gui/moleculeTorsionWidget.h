// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "modules/moleculeTorsion/gui/ui_moleculeTorsionWidget.h"
#include "modules/widget.h"

// Forward Declarations
class MoleculeTorsionModule;
class DataViewer;

// Module Widget
class MoleculeTorsionModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    MoleculeTorsionModule *module_;

    public:
    MoleculeTorsionModuleWidget(QWidget *parent, MoleculeTorsionModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::MoleculeTorsionModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *histogramGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
