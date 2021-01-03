// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <QPalette>

// Forward Declarations
class QWidget;

// Wizard Widget Highlight Definition
class WizardWidgetHighlight : public ListItem<WizardWidgetHighlight>
{
    public:
    WizardWidgetHighlight();
    ~WizardWidgetHighlight();

    /*
     * Data
     */
    private:
    // Target widget
    QWidget *widget_;
    // Original palette
    QPalette originalPalette_;
    // Highlight colour
    QColor colour_;

    public:
    // Set target widget
    void setWidget(QWidget *widget);
    // Set highlight colour
    void setColour(QColor colour);

    /*
     * Highlighting
     */
    public:
    // Highlight the target widget
    void highlight();
    // Remove highlight from the target widget
    void unHighlight();
};
