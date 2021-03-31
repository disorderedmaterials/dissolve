// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/analyse/gui/ui_modulewidget.h"

// Forward Declarations
class AnalyseModule;
class CoreData;
class Module;

// Module Widget
class AnalyseModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    AnalyseModuleWidget(QWidget *parent, const GenericList &processingData, AnalyseModule *module, const CoreData &coreData);

    private:
    // Associated Module
    AnalyseModule *module_;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AnalyseModuleWidget ui_;
};
