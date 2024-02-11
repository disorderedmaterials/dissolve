// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/colourScale.h"
#include <QWidget>

// Gradient Bar
class GradientBar : public QWidget
{
    Q_OBJECT

    public:
    GradientBar(QWidget *parent);

    /*
    // ColourScale
    */
    private:
    // Local ColourScale to display
    ColourScale colourScale_;
    // Local QGradient, constructed from ColourScale
    QLinearGradient gradient_;

    public:
    // Set colourscale to display
    void setColourScale(const ColourScale &colourScale);

    protected:
    // Paint event callback
    void paintEvent(QPaintEvent *event);
};
