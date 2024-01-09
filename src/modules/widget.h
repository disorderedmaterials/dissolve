// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class LineParser;

// ModuleWidget, base class for any Module-specific control widget
class ModuleWidget : public QWidget
{
    public:
    ModuleWidget(QWidget *parent, Dissolve &dissolve_);
    virtual ~ModuleWidget() = default;

    /*
     * UI
     */
    protected:
    // Dissolve instance
    Dissolve &dissolve_;
    // Whether widget is currently refreshing
    bool refreshing_;

    public:
    // Update Flags
    enum UpdateFlags
    {
        RecreateRenderablesFlag /* Update as normal, but any existing renderables must be cleared and regenerated */
    };
    // Update controls within widget
    virtual void updateControls(const Flags<UpdateFlags> &updateFlags = {});
    // Disable editing within widget
    virtual void preventEditing();
    // Allow editing within widget
    virtual void allowEditing();
};
