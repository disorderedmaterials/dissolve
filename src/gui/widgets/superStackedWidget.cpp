// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/widgets/superStackedWidget.h"
#include <QPainter>

SuperStackedWidget::SuperStackedWidget(QWidget *parent) : QStackedWidget(parent) {}

// Paint event
void SuperStackedWidget::paintEvent(QPaintEvent *event)
{
    if (currentIndex() != 0)
        return;

    QPainter painter(this);

    // Draw top-left hexagons
    painter.drawPixmap(0, 0, QPixmap(":/images/images/hexagons1.png"));

    // Draw the background before we do anything else
    painter.drawPixmap(width() - 512, height() - 527, QPixmap(":/images/images/big.png"));
}
