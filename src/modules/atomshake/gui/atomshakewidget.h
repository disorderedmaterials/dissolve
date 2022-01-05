// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "modules/atomshake/gui/ui_atomshakewidget.h"
#include "modules/widget.h"

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
    AtomShakeModuleWidget(QWidget *parent, AtomShakeModule *module, Dissolve &dissolve);
    ~AtomShakeModuleWidget() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::AtomShakeModuleWidget ui_;
};
