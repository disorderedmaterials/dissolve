// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/widgets/gradientBar.h"
#include <QPainter>

/*
 * Gradient Bar
 */

GradientBar::GradientBar(QWidget *parent) : QWidget(parent)
{
    // Private variables
}

// Set local colourscale
void GradientBar::setColourScale(const ColourScale &colourScale)
{
    // If the source colourscale uses deltaHSV_, we must construct an interpolated gradient manually since QGradient doesn't
    // support HSV interpolation
    if (colourScale.useHSV())
    {
        colourScale_.clear();
        colourScale_.setUseHSV(false);
        if (colourScale.nPoints() > 0)
        {
            const auto nPoints = 101;
            double value = colourScale.firstPoint().value();
            double delta = (colourScale.lastPoint().value() - value) / nPoints;
            for (auto n = 0; n < nPoints; ++n)
            {
                colourScale_.addPoint(value, colourScale.colour(value));
                value += delta;
            }
        }
    }
    else
        colourScale_ = colourScale;

    // Setup QGradient - in ObjectBoundingMode, 0.0 = top of rectangle, and 1.0 is bottom
    gradient_ = QLinearGradient(0.0, 1.0, 0.0, 0.0);
    gradient_.setCoordinateMode(QGradient::ObjectBoundingMode);

    // -- Loop backwards through points
    if (colourScale_.nPoints() == 0)
    {
        gradient_.setColorAt(0.0, QColor(0, 0, 0));
        gradient_.setColorAt(1.0, QColor(0, 0, 0));
    }
    else if (colourScale_.nPoints() == 1)
    {
        gradient_.setColorAt(0.0, colourScale_.firstPoint().colour());
        gradient_.setColorAt(1.0, colourScale_.firstPoint().colour());
    }
    else
    {
        double zero = colourScale_.firstPoint().value();
        double span = colourScale_.lastPoint().value() - zero;
        for (auto n = 0; n < colourScale_.nPoints(); ++n)
            gradient_.setColorAt((colourScale_.point(n).value() - zero) / span, colourScale_.point(n).colour());
    }

    repaint();
}

// Paint event callback
void GradientBar::paintEvent(QPaintEvent *event)
{
    if (colourScale_.nPoints() == 0)
        return;

    QPainter painter(this);

    // Draw single rectangle and we're done
    QBrush brush(gradient_);
    QPen pen(Qt::NoPen);
    QRectF rect(0.0, 0.0, width() - 1.0, height() - 1.0);
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRect(rect);
    painter.end();
}
