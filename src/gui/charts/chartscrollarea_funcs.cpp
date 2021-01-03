// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/charts/chartscrollarea.hui"
#include <QScrollBar>

ChartScrollArea::ChartScrollArea(QWidget *parent) : QScrollArea(parent) {}

ChartScrollArea::~ChartScrollArea() {}

/*
 * QScrollArea Reimplementations
 */

// Return size hint
QSize ChartScrollArea::sizeHint() const
{
    QSize s = viewportSizeHint();
    if (horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOn)
        s.setHeight(s.height() + horizontalScrollBar()->sizeHint().height());
    if (verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn)
        s.setWidth(s.width() + verticalScrollBar()->sizeHint().width());
    return s;
}

// Return viewport size hint
QSize ChartScrollArea::viewportSizeHint() const { return (widget() ? widget()->sizeHint() : QSize(50, 50)); }
