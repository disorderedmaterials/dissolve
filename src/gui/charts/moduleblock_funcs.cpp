/*
    *** Module Block Widget - Functions
    *** src/gui/charts/moduleblock_funcs.cpp
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

#include "gui/charts/moduleblock.h"
#include "gui/charts/modulelistmetrics.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/modulewidget.h"
#include "main/dissolve.h"
#include "module/module.h"
#include "templates/variantpointer.h"
#include <QFile>
#include <QPainter>
#include <QPainterPath>

ModuleBlock::ModuleBlock(QWidget *parent, Module *module, Dissolve &dissolve)
    : QWidget(parent), ChartBlock(), dissolve_(dissolve)
{
    // Set up user interface
    ui_.setupUi(this);

    refreshing_ = false;

    // Set necessary values on the widget itself
    ModuleListChartMetrics metrics;
    setContentsMargins(metrics.blockMargins());
    dragHandleRect_ = QRect(0, 0, width(), height());

    // Set the Module pointer
    module_ = module;

    // Set the icon and module type label
    ui_.TopLabel->setText(QString::fromStdString(std::string(module_->type())));
    ui_.IconLabel->setPixmap(modulePixmap(module_));

    // Set event filtering so that we do not blindly accept mouse wheel events in the frequency spin (problematic since we
    // will exist in a QScrollArea)
    ui_.FrequencySpin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.FrequencySpin));

    // Update our controls
    updateControls();

    updateGeometry();
}

ModuleBlock::~ModuleBlock() {}

/*
 * Module Target
 */

// Return displayed Module
Module *ModuleBlock::module() const { return module_; }

/*
 * Controls
 */

// Return suitable QPixmap for supplied Module
QPixmap ModuleBlock::modulePixmap(const Module *module)
{
    if (module)
        return modulePixmap(QString::fromStdString(std::string(module->type())));

    return QPixmap(":/modules/icons/modules_generic.svg");
}

// Return suitable QPixmap for supplied Module type
QPixmap ModuleBlock::modulePixmap(QString moduleType)
{
    // Construct the name of the icon for this module in our resource file
    QString iconName = QString(":/modules/icons/modules_%1.svg").arg(moduleType.toLower());
    if (QFile::exists(iconName))
        return QPixmap(iconName);

    return QPixmap(":/modules/icons/modules_generic.svg");
}

void ModuleBlock::on_RemoveButton_clicked(bool checked)
{
    emit(remove(QString::fromStdString(std::string(module_->uniqueName()))));
}

void ModuleBlock::on_NameEdit_editingFinished()
{
    if (refreshing_)
        return;

    // If the name is the same, return now
    if (DissolveSys::sameString(qPrintable(ui_.NameEdit->text()), module_->uniqueName(), true))
        return;

    // Check that the new name is unique
    std::string uniqueName = dissolve_.uniqueModuleName(qPrintable(ui_.NameEdit->text()), module_);

    module_->setUniqueName(uniqueName);

    ui_.NameEdit->setText(QString::fromStdString(uniqueName));

    emit(dataModified());
}

void ModuleBlock::on_NameEdit_returnPressed()
{
    // Call the editingFinished() function
    on_NameEdit_editingFinished();
}

void ModuleBlock::on_EnabledButton_clicked(bool checked)
{
    if (refreshing_)
        return;

    module_->setEnabled(checked);

    ui_.IconFrame->setEnabled(checked);

    emit(dataModified());
}

void ModuleBlock::on_FrequencySpin_valueChanged(int value)
{
    if (refreshing_)
        return;

    module_->setFrequency(value);

    emit(dataModified());
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleBlock::paintEvent(QPaintEvent *event)
{
    if (!module_)
        return;

    ModuleListChartMetrics metrics;

    QPainter painter(this);

    QPen borderPen;
    borderPen.setWidth(metrics.blockBorderWidth());
    painter.setPen(borderPen);

    QPainterPath borderPath;
    const auto blockDentLeft = width() * 0.5 - metrics.blockDentRadius();
    borderPath.moveTo(metrics.blockBorderMidPoint(), metrics.blockBorderMidPoint());
    borderPath.arcTo(blockDentLeft, metrics.blockBorderMidPoint() - metrics.blockDentRadius(), metrics.blockDentRadius() * 2,
                     metrics.blockDentRadius() * 2, 180, 180);
    borderPath.lineTo(width() - metrics.blockBorderMidPoint(), metrics.blockBorderMidPoint());
    borderPath.lineTo(width() - metrics.blockBorderMidPoint(), height() - metrics.blockBorderMidPoint());
    borderPath.lineTo(metrics.blockBorderMidPoint(), height() - metrics.blockBorderMidPoint());
    borderPath.closeSubpath();

    painter.setBrush(Qt::white);

    // Ready - draw the border + fill!
    painter.drawPath(borderPath);
}

/*
 * Type (ChartBlock Reimplementations)
 */

// Return type of this block
const QString ModuleBlock::blockType() const { return "Module"; }

/*
 * Widget (ChartBlock Reimplementations)
 */

// Return underlying widget
QWidget *ModuleBlock::widget() { return this; }

// Return width of underlying widget
int ModuleBlock::widgetWidth() const { return sizeHint().width(); }

// Return height of underlying widget
int ModuleBlock::widgetHeight() const { return sizeHint().height(); }

// Return whether the supplied point (on the parent chart) allows a drag operation to begin
bool ModuleBlock::isDragPoint(QPoint point) const
{
    if (dragHandleRect_.translated(geometry().left(), geometry().top()).contains(point))
        return true;

    return false;
}

/*
 * Update (ChartBlock Reimplementations)
 */

// Update controls within widget
void ModuleBlock::updateControls()
{
    if (!module_)
        return;

    refreshing_ = true;

    // Set unique name
    ui_.NameEdit->setText(QString::fromStdString(std::string(module_->uniqueName())));

    // Set 'enabled' button status
    ui_.EnabledButton->setChecked(module_->isEnabled());
    ui_.IconFrame->setEnabled(module_->isEnabled());

    // Set frequency spin
    ui_.FrequencySpin->setValue(module_->frequency());

    // Update Configuration label and icon tooltip
    if (module_->nRequiredTargets() == Module::ZeroTargets)
    {
        ui_.ConfigurationsLabel->setText("-");
        ui_.ConfigurationsIconLabel->setToolTip("This module does not accept configuration targets.");
    }
    else
    {
        // Construct the tooltip
        QString toolTip;

        if (module_->nRequiredTargets() == Module::OneOrMoreTargets)
            toolTip = "This module may target any number of configurations.\n";
        else
            toolTip = QString("This module must target exactly %1 %2.\n")
                          .arg(module_->nRequiredTargets())
                          .arg(module_->nRequiredTargets() == 1 ? "configuration" : "configurations");

        if (module_->nTargetConfigurations() == 0)
            toolTip += "No configuration targets set.";
        else
        {
            toolTip += "Current configuration targets:\n";
            for (const auto *cfg : module_->targetConfigurations())
                toolTip += QString("- %1\n").arg(QString::fromStdString(std::string(cfg->name())));
        }

        ui_.ConfigurationsLabel->setText(QString::number(module_->nTargetConfigurations()));
        ui_.ConfigurationsIconLabel->setToolTip(toolTip);
    }

    refreshing_ = false;
}

// Disable sensitive controls
void ModuleBlock::disableSensitiveControls()
{
    ui_.RemoveButton->setEnabled(false);
    ui_.NameEdit->setEnabled(false);
    ui_.EnabledButton->setEnabled(false);
    ui_.FrequencySpin->setEnabled(false);
}

// Enable sensitive controls
void ModuleBlock::enableSensitiveControls()
{
    ui_.RemoveButton->setEnabled(true);
    ui_.NameEdit->setEnabled(true);
    ui_.EnabledButton->setEnabled(true);
    ui_.FrequencySpin->setEnabled(true);
}
