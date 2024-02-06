// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/modifierOSites/gui/ui_modifierOSitesWidget.h"
#include "modules/widget.h"

// Forward Declarations
class ModifierOSitesModule;
class DataViewer;

// Module Widget
class ModifierOSitesModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    ModifierOSitesModule *module_;

    public:
    ModifierOSitesModuleWidget(QWidget *parent, ModifierOSitesModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ModifierOSitesModuleWidget ui_;

    // DataViewer contained within this widget
    DataViewer *qSpeciesGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
