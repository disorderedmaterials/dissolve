// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/histogramCN/gui/ui_histogramCNWidget.h"
#include "modules/widget.h"

// Forward Declarations
class HistogramCNModule;
class DataViewer;

// Module Widget
class HistogramCNModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    HistogramCNModule *module_;

    public:
    HistogramCNModuleWidget(QWidget *parent, HistogramCNModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::HistogramCNModuleWidget ui_;

    // DataViewer contained within this widget
    DataViewer *cnGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
