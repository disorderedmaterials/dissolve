/*
    *** Module Insertion Block Widget - Functions
    *** src/gui/charts/moduleinsertionblock_funcs.cpp
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

    // 	painter.setBrush(Qt::white);

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
