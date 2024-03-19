// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/siteRDF/gui/ui_siteRDFWidget.h"
#include "modules/widget.h"

// Forward Declarations
class SiteRDFModule;
class DataViewer;

// Module Widget
class SiteRDFModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    SiteRDFModule *module_;

    public:
    SiteRDFModuleWidget(QWidget *parent, SiteRDFModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SiteRDFModuleWidget ui_;
    // DataViewer contained within this widget
    DataViewer *rdfGraph_,*runningCNGraph_;

    public:
    // Update controls within widget
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;
};
