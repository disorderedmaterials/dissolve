// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/wizardwidgethighlight.h"
#include <QWidget>

WizardWidgetHighlight::WizardWidgetHighlight() : ListItem<WizardWidgetHighlight>() { widget_ = nullptr; }

WizardWidgetHighlight::~WizardWidgetHighlight() {}

/*
 * Data
 */

// Set target widget
void WizardWidgetHighlight::setWidget(QWidget *widget)
{
    widget_ = widget;

    if (widget_)
        originalPalette_ = widget_->palette();
}

// Set highlight colour
void WizardWidgetHighlight::setColour(QColor colour) { colour_ = colour; }

/*
 * Highlighting
 */

// Highlight the target widget
void WizardWidgetHighlight::highlight()
{
    if (!widget_)
        return;

    QPalette palette = widget_->palette();
    palette.setColor(QPalette::Base, colour_);
    palette.setColor(QPalette::AlternateBase, colour_);
    palette.setColor(QPalette::Window, colour_);
    widget_->setAutoFillBackground(true);
    widget_->setPalette(palette);
}

// Remove highlight from the target widget
void WizardWidgetHighlight::unHighlight()
{
    if (!widget_)
        return;

    widget_->setAutoFillBackground(false);
    widget_->setPalette(originalPalette_);
}
