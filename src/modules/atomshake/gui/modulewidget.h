// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/modulewidget.h"
#include "modules/atomshake/gui/ui_modulewidget.h"

// Forward Declarations
class AtomShakeModule;

// Module Widget
class AtomShakeModuleWidget : public ModuleWidget
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    private:
    // Associated Module
    AtomShakeModule *module_;

    public:
    AtomShakeModuleWidget(QWidget *parent, const GenericList &processingData, AtomShakeModule *module);
    ~AtomShakeModuleWidget();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AtomShakeModuleWidget ui_;
};
