// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QPushButton>

// ColourButton
class ColourButton : public QPushButton
{
    Q_OBJECT

    public:
    ColourButton(QWidget *parent);

    /*
    // Content
    */
    private:
    // Colour currently displayed in button
    QColor colour_;

    public:
    // Set colour displayed in button
    void setColour(QColor colour);
    // Retrieve colour displayed in button
    QColor colour();
    // Select new colour
    bool selectColour();

    protected:
    // Paint event callback
    void paintEvent(QPaintEvent *event);
};
