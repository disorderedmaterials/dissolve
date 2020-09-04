/*
    *** Wizard Widget Highlight Functions
    *** src/gui/wizardwidgethighlight.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
