// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/angle/gui/ui_angleWidget.h"
#include "modules/widget.h"

// Forward Declarations
class AngleModule;
class DataViewer;

// Module Widget
class AngleModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    AngleModule *module_;

    public:
    AngleModuleWidget(QWidget *parent, AngleModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AngleModuleWidget ui_;
    // DataViewers contained within this widget
    DataViewer *rdfABGraph_, *rdfBCGraph_, *angleGraph_, *dAngleABGraph_, *dAngleBCGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
