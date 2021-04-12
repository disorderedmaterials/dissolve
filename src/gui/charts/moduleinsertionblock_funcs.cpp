// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/charts/moduleinsertionblock.h"
#include "gui/charts/modulelistmetrics.h"
#include <QPainter>
#include <QPainterPath>

ModuleInsertionBlock::ModuleInsertionBlock(QWidget *parent) : QWidget(parent)
{
    // Set up user interface
    ui_.setupUi(this);

    updateControls();
}

ModuleInsertionBlock::~ModuleInsertionBlock() {}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleInsertionBlock::paintEvent(QPaintEvent *event)
{
    ModuleListChartMetrics metrics;

    QPainter painter(this);

    QPen borderPen;
    borderPen.setWidth(metrics.blockBorderWidth());
    borderPen.setStyle(Qt::DashLine);
    painter.setPen(borderPen);

    QPainterPath borderPath;
    borderPath.moveTo(metrics.blockBorderMidPoint(), metrics.blockBorderMidPoint());
    borderPath.lineTo(metrics.blockBorderMidPoint(), metrics.blockDentOffset());
    borderPath.arcTo(metrics.blockBorderMidPoint() - metrics.blockDentRadius(),
                     metrics.blockDentOffset() + metrics.blockBorderMidPoint(), metrics.blockDentRadius() * 2,
                     metrics.blockDentRadius() * 2, 90, -180);
    borderPath.lineTo(metrics.blockBorderMidPoint(), height() - metrics.blockBorderWidth());
    borderPath.lineTo(width() - metrics.blockBorderWidth(), height() - metrics.blockBorderWidth());
    borderPath.lineTo(width() - metrics.blockBorderWidth(), metrics.blockBorderMidPoint());
    borderPath.closeSubpath();

    // Ready - draw the border + fill!
    painter.drawPath(borderPath);
}

/*
 * Widget Functions
 */

// Return underlying widget
QWidget *ModuleInsertionBlock::widget() { return this; }

// Update controls within widget
void ModuleInsertionBlock::updateControls() {}

// Disable sensitive controls
void ModuleInsertionBlock::disableSensitiveControls() {}

// Enable sensitive controls
void ModuleInsertionBlock::enableSensitiveControls() {}

/*
 * Geometry
 */

// Return width of underlying widget
int ModuleInsertionBlock::widgetWidth() const { return minimumSize().width(); }

// Return height of underlying widget
int ModuleInsertionBlock::widgetHeight() const { return minimumSize().height(); }

// Set underlying widget geometry
void ModuleInsertionBlock::setWidgetGeometry(int left, int top, int width, int height)
{
    setGeometry(left, top, width, height);
}

/*
 * Icons
 */

// Set icons to reflect insertion of specified Module
void ModuleInsertionBlock::setDisplayModuleInsertion(QString moduleType)
{
    // TODO
    // 	ui_.ModuleIconLabel->setPixmap(ModuleBlock::modulePixmap(qPrintable(moduleType)));
    // 	ui_.ModuleIconLabel->setVisible(true);
}

// Set icons to reflect moving existing Module to new position
void ModuleInsertionBlock::setDisplayModuleMove()
{
    ui_.ActionLabel->setPixmap(QPixmap(":/general/icons/general_movemodule.svg"));
    ui_.ModuleIconLabel->setVisible(true);
}
