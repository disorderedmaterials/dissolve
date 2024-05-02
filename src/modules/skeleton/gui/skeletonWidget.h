// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "modules/skeleton/gui/ui_skeletonWidget.h"
#include "modules/widget.h"

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
    SkeletonModuleWidget(QWidget *parent, SkeletonModule *module, Dissolve &dissolve);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SkeletonModuleWidget ui_;
};
