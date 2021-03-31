// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <QWidget>

// Forward Declarations
class GenericList;
class LineParser;

// ModuleWidget, base class for any Module-specific control widget
class ModuleWidget : public QWidget
{
    public:
    ModuleWidget(QWidget *parent, const GenericList &processingData);
    virtual ~ModuleWidget();

    /*
     * UI
     */
    protected:
    // Processing data source
    const GenericList &processingData_;
    // Whether widget is currently refreshing
    bool refreshing_;

    public:
    // Update flags
    enum UpdateFlags
    {
        DefaultUpdateFlag = 0,        /* Standard update */
        ResetGraphDataTargetsFlag = 1 /* Any renderables should be cleared and regenerated in viewers */
    };
    // Update controls within widget
    virtual void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);
    // Disable sensitive controls within widget
    virtual void disableSensitiveControls();
    // Enable sensitive controls within widget
    virtual void enableSensitiveControls();
};
