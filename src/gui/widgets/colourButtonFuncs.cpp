// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/widgets/colourButton.hui"
#include <QColorDialog>
#include <QPainter>

/*
 * ColourButton
 */

ColourButton::ColourButton(QWidget *parent) : QPushButton(parent) {}

// Set displayed colour
void ColourButton::setColour(QColor colour)
{
    colour_ = colour;
    repaint();
}

// Return displayed colour
QColor ColourButton::colour() { return colour_; }

// Select new colour
bool ColourButton::selectColour()
{
    // Request a colour dialog
    QColor newcol = QColorDialog::getColor(colour_.rgba(), this, "Select new colour", QColorDialog::ShowAlphaChannel);
    if (!newcol.isValid())
        return false;

    colour_ = newcol;
    repaint();
    return true;
}

// Paint event callback
void ColourButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Draw single rectangle and we're done
    QBrush brush(colour_);
    QRectF rect(3.0, 3.0, width() - 3.0, height() - 3.0);
    painter.setBrush(brush);
    painter.drawRect(rect);
    painter.end();
}
