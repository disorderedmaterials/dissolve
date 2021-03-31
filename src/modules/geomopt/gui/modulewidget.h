// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/geomopt/gui/ui_modulewidget.h"

// Forward Declarations
class GeometryOptimisationModule;

// Module Widget
class GeometryOptimisationModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    GeometryOptimisationModule *module_;

    public:
    GeometryOptimisationModuleWidget(QWidget *parent, const GenericList &processingData, GeometryOptimisationModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::GeometryOptimisationModuleWidget ui_;
};
