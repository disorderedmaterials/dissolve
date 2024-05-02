// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/qSpecies/gui/ui_qSpeciesWidget.h"
#include "modules/widget.h"

// Forward Declarations
class QSpeciesModule;
class DataViewer;

// Module Widget
class QSpeciesModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    QSpeciesModule *module_;

    public:
    QSpeciesModuleWidget(QWidget *parent, QSpeciesModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::QSpeciesModuleWidget ui_;

    // DataViewer contained within this widget
    DataViewer *qSpeciesGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
