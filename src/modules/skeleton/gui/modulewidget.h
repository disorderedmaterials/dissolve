// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/skeleton/gui/ui_modulewidget.h"

// Forward Declarations
class SkeletonModule;

// Module Widget
class SkeletonModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    SkeletonModule *module_;

    public:
    SkeletonModuleWidget(QWidget *parent, const GenericList &processingData, SkeletonModule *module);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SkeletonModuleWidget ui_;
};
